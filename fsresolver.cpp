#include "precomp.h"
#include "fsresolver.h"
#include "fshandler.h"
#include "context.h"

filesystem_resolver::filesystem_resolver(String const& root) :
m_root (root)
{
}

filesystem_resolver::~filesystem_resolver()
{
}

mime_handler* 
filesystem_resolver::resolve(http_context&, String const& uri)
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





















































