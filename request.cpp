#include "precomp.h"
#include "request.h"
#include "httpd.h"
#include "misc.h"
#include "encoding.h"

http_request::http_request(connection& con) :
m_con     (con),
m_method  (rm_unknown),
headers   (m_headers),
getvars   (m_getvars),
postvars  (m_postvars)
{
}

http_request::~http_request()
{
}

request_methods 
http_request::method() const
{
  return m_method;
}

std::string 
http_request::body() const
{
  return m_body;
}

bool 
http_request::parse_request()
{
  std::string::size_type pos;

  // Parse headers
  if(!parse_headers())
  {
    return false;
  }

  // Write headers
  if(httpd.log_request_headers)
  {
    header_map::const_iterator it, ie;
    it = headers.begin();
    ie = headers.end();
    for(; it != ie; ++it)
    {
      httpd.log(it->first + ":" + it->second);
    }
  }

  // Set http_request type
  #define METHOD(str,val) \
  if(cmd == str) m_method = val; else
  std::string cmd = headers["HTTP_COMMAND"];
  METHOD("GET",      rm_get)
  METHOD("POST",     rm_post)
  METHOD("CONNECT",  rm_connect)
  METHOD("CUSTOM",   rm_custom)
  METHOD("DELETE",   rm_delete)
  METHOD("HEAD",     rm_head)
  METHOD("NONE",     rm_none)
  METHOD("OPTIONS",  rm_options)
  METHOD("PUT",      rm_put)
  METHOD("TRACE",    rm_trace) return false;
  #undef METHOD

  // Determine host/port
  std::string hp = headers["Host"];
  if(hp.empty())
  {
    return false;
  }
  if((pos = hp.find(':')) == std::string::npos)
  {
    m_headers["HTTP_HOST"] = hp;
    m_headers["HTTP_PORT"] = 80;
  }
  else
  {
    m_headers["HTTP_HOST"] = hp.substr(0, pos);
    m_headers["HTTP_PORT"] = hp.substr(pos + 1);
  }

  // See if there's post data
  if(headers["HTTP_COMMAND"] == "POST")
  {
    if(!parse_postdata())
    {
      return false;
    }
  }

  // Decode username/password if available
  if(headers.contains("Authorization"))
  {
    std::string auth;
    std::string type;
    std::string::size_type from;

    // Extract header values
    auth = headers["Authorization"];
    from = auth.find(' ');
    type = auth.substr(0, from);
    auth = auth.substr(from + 1);

    // Handle basic authentication
    if(_stricmp(type.c_str(), "basic") == 0)
    {
      // Decode username/password
      auth = base64_decode(auth);
      from = auth.find(':');
      m_headers["HTTP_USERNAME"] = auth.substr(0, from);
      m_headers["HTTP_PASSWORD"] = auth.substr(from + 1);
    }
  }

  // Success
  return true;
}

bool
http_request::parse_headers()
{
  bool firstline = true;
  std::string line;
  for(;;)
  {
    // Read a line
    if(!m_con.read_line(line))
    {
      return false;
    }

    // Last line
    if(line.empty())
    {
      // rfc2616-4.1: In the interest of robustness, servers SHOULD ignore 
      // any empty line(s) received where a Request-Line is expected
      if(!firstline)
      {
        return true;
      }
    }
    else
    {
      // Add request line to log
      if(firstline)
      {
        firstline = false;
        m_con.log(line);
      }

      // Parse the line
      parse_line(line);
    }
  }
}

bool
http_request::parse_line(std::string const& line)
{
  #define VALID_POS(arg) if(std::string::npos == (arg)) return false;

  std::string::size_type pos1, pos2;

  // First line is the http request
  if(m_headers.empty())
  {
    // Find first separator
    VALID_POS(pos1 = line.find(' '));

    // Extract command
    std::string cmd = line.substr(0, pos1);
    VALID_POS(pos1 = line.find_first_not_of(' ', pos1));

    // Add to headers
    m_headers["HTTP_COMMAND"] = cmd;

    // Find next separator
    VALID_POS(pos2 = line.find(' ', pos1));

    // Extract query
    std::string uri = line.substr(pos1, pos2 - pos1);

    // Extract query params
    if((pos1 = uri.find('?')) != std::string::npos)
    {
      if(!parse_url_params(uri.substr(pos1 + 1), m_getvars))
      {
        // TODO it might make sense to just store the entire thing
        // and ignore any parsing errors that occurred. Who cares ?
        // JF 05022008 It turns out that this is indeed probably the 
        // best thing to do.
        m_getvars[uri.substr(pos1 + 1)] = "";
      }
      uri = uri.substr(0, pos1);
    }

    // Replace backslashes if present (would be an error)
    std::replace(uri.begin(), uri.end(), '\\', '/');

    // TODO a partial url decoding is required (rfc2616:3.2.3)

    // Store uri
    m_headers["HTTP_URI"] = uri;

    // Determine version
    VALID_POS(pos1 = line.find_first_not_of(' ', pos2));
    m_headers["HTTP_VERSION"] = line.substr(pos1);
  }
  else
  {
    // Add to regular headers
    m_headers.append(line);
  }
  return true;
}

bool 
http_request::parse_url_params(std::string const& params, header_map& map)
{
  std::string::size_type pos1 = 0;
  std::string::size_type pos2 = 0;
  std::string::size_type pos3;
  std::string param;

  // Enumerate params
  for(; pos2 != std::string::npos; )
  {
    // Find next param
    if((pos2 = params.find('&', pos1)) == std::string::npos)
    {
      param = params.substr(pos1);
    }
    else
    {
      param = params.substr(pos1, pos2 - pos1);
      pos1 = pos2 + 1;
    }

    // Find name/value separator
    if((pos3 = param.find('=')) == std::string::npos)
    {
      return false;
    }

    // Decode and add to map
    map[url_decode(param.substr(0, pos3))] = 
        url_decode(param.substr(pos3 + 1)) ;
  }

  // Done
  return true;
}

bool
http_request::parse_postdata() 
{
  // Check length
  if(!headers.contains("Content-Length"))
  {
    return false;
  }
  int len = headers["Content-Length"];

  // Alloc buffer
  char* buf = new char[len + 1];
  
  // Read the line
  if(!m_con.read_bytes(buf, len))
  {
    delete [] buf;
    return false;
  }

  // Null-terminate
  buf[len] = 0;
  std::string data = buf;
  delete [] buf;

  // Hack: Mozilla appears to add trailing empty lines,
  // so we remove any empty lines following the POST
  std::string temp;
  while(m_con.read_line(temp, true))  // peek
  {
    if(!temp.empty())
    {
      break;
    }
    m_con.read_line(temp);  // remove
  }

  // Expect form content-type
  if(headers["Content-Type"] == "application/x-www-form-urlencoded")
  {
    // Retrieve form data
    return parse_url_params(data, m_postvars);
  }
  else
  {
    m_body = data;
    return true;
  }
}
