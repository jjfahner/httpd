#include "precomp.h"
#include "fsresolver.h"
#include "fshandler.h"
#include "context.h"
#include "site.h"

filesystem_resolver::filesystem_resolver(http_site* site, String const& root) :
m_site (site),
m_root (root)
{
}

filesystem_resolver::~filesystem_resolver()
{
}

mime_handler* 
filesystem_resolver::resolve(http_context& context, String const& uri)
{
  // Fetch request path
  String path = uri;
  
  // Replace slashes
  std::replace(path.begin(), path.end(), '/', '\\');

  // Remove updir references from path
  if(!resolve_updirs(path))
  {
    return false;
  }

  // Prepend root
  path = m_root + path;

  // Determine file information
  WIN32_FIND_DATA wfd;
  HANDLE hFind = FindFirstFile(path.c_str(), &wfd);
  if(hFind == INVALID_HANDLE_VALUE)
  {
    return 0;
  }
  FindClose(hFind);


  // Extract extension
  String ext;
  String::size_type es = uri.find_last_of(".");
  String::size_type ss = uri.find_last_of("/");
  if(es != String::npos && (ss == String::npos || ss < es))
  {
    ext = uri.substr(es + 1);
  }

  // Store uri and extension
  context.resolved_uri = uri;
  context.resolved_ext = ext;

  // Retrieve specialized handler
  if(http_site::mime_type const* mt = m_site->mime_type_from_ext(ext))
  {
    if(mt->m_handler)
    {
      return mt->m_handler;
    }
  }

  // Return new handler
  return new filesystem_handler(path, wfd);
}

bool
filesystem_resolver::resolve_updirs(String path)
{
  path;
  return true;

// String::size_type pos1;
// if((pos1 = path.find('\\') == String::npos)
// {
// }


}






















































