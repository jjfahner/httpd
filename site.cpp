#include "precomp.h"
#include "site.h"
#include "httpd.h"
#include "context.h"
#include "context.h"
#include "request.h"
#include "response.h"
#include "handler.h"
#include "resolver.h"
#include "fsresolver.h"

http_site::http_site(String const& name, bool autostart) :
class_state<site_states>(ss_stopped),
m_name      (name),
m_autostart (autostart),
m_resolver  (0)
{
}

http_site::~http_site()
{
  if(m_mime_default.m_handler)
  {
    delete m_mime_default.m_handler;
  }
}

String const& 
http_site::name() const
{
  return m_name;
}

String const& 
http_site::server() const
{
  return m_server;
}

void 
http_site::set_server(String const& s)
{
  m_server = s;
}

bool 
http_site::autostart() const
{
  return m_autostart;
}

void 
http_site::set_autostart(bool b)
{
  m_autostart = b;
}

mime_resolver* 
http_site::resolver() const
{
  return m_resolver;
}

void
http_site::set_resolver(mime_resolver* resolver)
{
  delete m_resolver;
  m_resolver = resolver;
}

bool
http_site::running() const
{
  return m_running;
}

void 
http_site::start()
{
  CLASS_STATE(ss_stopped);
  set_state(ss_starting);

  aliaslist::iterator it, ie;
  it = m_aliasses.begin();
  ie = m_aliasses.end();
  for(; it != ie; ++it)
  {
    httpd.start_listener(it->m_port, this, it->m_name);
  }

  set_state(ss_running);
}

void 
http_site::stop()
{
  CLASS_STATE(ss_running);
  set_state(ss_stopping);

  aliaslist::iterator it, ie;
  it = m_aliasses.begin();
  ie = m_aliasses.end();
  for(; it != ie; ++it)
  {
    httpd.stop_listener(it->m_port, this, it->m_name);
  }

  set_state(ss_stopped);
}

void 
http_site::add_alias(String const& name, int port)
{
  CLASS_STATE(ss_stopped);
  alias a;
  a.m_name = name;
  a.m_port = port;
  m_aliasses.push_back(a);
}

http_site::aliaslist const& 
http_site::aliasses() const
{
  return m_aliasses;
}

void 
http_site::set_mime_type(String const& extension, 
                         String const& type, 
                         mime_handler*  handler,
                         bool is_default)
{
  if(is_default)
  {
    m_mime_default.m_extension = extension;
    m_mime_default.m_type      = type;
    if(handler)
    {
      delete m_mime_default.m_handler;
      m_mime_default.m_handler = handler;
    }
  }
  else
  {
    m_mime_types[extension] = mime_type(extension, type, handler);
  }
}

http_site::mime_type const*
http_site::mime_type_from_type(String const& type)
{
  mime_map::const_iterator it, ie;
  it = m_mime_types.begin();
  ie = m_mime_types.end();
  for(; it != ie; ++it)
  {
    if(it->second.m_type == type)
    {
      return &it->second;
    }
  }
  if(m_mime_default.m_type == type)
  {
    return &m_mime_default;
  }
  return 0;
}

http_site::mime_type const* 
http_site::mime_type_from_ext(String const& ext)
{
  mime_map::const_iterator it, ie;
  it = m_mime_types.find(ext);
  if(it == m_mime_types.end())
  {
    return 0;
  }
  return &it->second;
}

void 
http_site::set_errorpage(int error, String const& path)
{
  m_errordocs[error] = path;
}

mime_handler* 
http_site::resolve(http_context& context, String const& uri) const
{
  // Check resolver
  if(!m_resolver)
  {
    return 0;
  }

  // Resolve through resolver
  if(mime_handler* handler = m_resolver->resolve(context, uri))
  {
    return handler;
  }

  // Determine some path properties
  String::size_type ln = uri.length();
  String::size_type es = uri.find_last_of(".");
  String::size_type ss = uri.find_last_of("/");

  // Determine the required path separator
  String separator;
  if(ss != String::npos && ss != ln - 1)
  {
    separator = "/";
  }

  // Apply default documents
  stringlist::const_iterator it = default_docs.begin();
  stringlist::const_iterator ie = default_docs.end();
  for(; it != ie; ++it)
  {
    String newuri = uri + separator + *it;
    if(mime_handler* handler = m_resolver->resolve(context, newuri))
    {
      return handler;
    }
  }

  // TODO Directory browsing

  // Not found
  return 0;
}

bool 
http_site::handle_request(http_context& context)
{
  // Replace the server header
  if(!m_server.empty())
  {
    context.response.headers["Server"] = m_server;
  }

  // Determine request uri
  String uri = context.request.headers["HTTP_URI"];

  // Resolve the path
  mime_handler* handler = resolve(context, uri);
  if(handler == 0)
  {
    context.response.send_error(404);
    return false;
  }

  // Determine default content-type
  String content_type = m_mime_types[context.resolved_ext]->m_type;
  if(content_type.empty())
  {
    content_type = m_mime_default.m_type;
  }
  context.response.headers["Content-Type"] = content_type;

  // Pessimistic view: http_response failed
  bool result = false;

  // Delegate the http_request to the handler
  try
  {
    handler->handle(context);
  }
  catch(std::exception const& e)
  {
    httpd.log(e.what());
  }
  catch(...)
  {
    httpd.log("Unknown exception");
  }

  // If the http_request failed, send a http_response
  if(result == false && !context.response.headers.finalized())
  {
    // Send internal server error
    context.response.send_error(500);
  }

  // Done
  return result;
}
