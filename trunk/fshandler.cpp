#include "precomp.h"
#include "fshandler.h"
#include "context.h"
#include "httpdate.h"

filesystem_handler::filesystem_handler(String const& path, WIN32_FIND_DATA const& wfd) :
m_path  (path)
{
  memcpy(&m_wfd, &wfd, sizeof(m_wfd));
}

bool 
filesystem_handler::handler_impl(http_context& context)
{
  // Set content-location
  if(context.request.headers["HTTP_URI"] != context.resolved_uri)
  {
    context.response.headers["Content-Location"] = context.resolved_uri;
  }

  // Determine last modified date
  httpdate omd(m_wfd.ftLastWriteTime);

  // Handle If-Modified-Since
  httpdate imd;
  if(imd.set(context.request.headers["If-Modified-Since"]) && imd == omd)
  {
    // Not modified
    context.response.send_not_modified();
    return true;
  }

  // Set last modified date
  context.response.headers["Last-Modified"] = omd.str();

  // Enable caching
  // TODO if there's any security, this is wrong !
  context.response.headers["Cache-Control"] = "public";

  // Open file
  HANDLE hFile = ::CreateFile(m_path.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
  if(hFile == INVALID_HANDLE_VALUE)
  {
    int error;
    switch(GetLastError())
    {
    case ERROR_ACCESS_DENIED:   error = 401; break;
    case ERROR_NOT_FOUND:       error = 404; break;
    default:                    error = 500; break;
    }
    context.response.send_error(error);
    return false;
  }

  // Determine file size
  DWORD size = GetFileSize(hFile, 0);

  // Write file in chunks
  char  buff[4096];
  DWORD read, done, sent = 0;
  while(size)
  {
    // Determine read size
    read  = size < 4096 ? size : 4096;
    size -= read;

    // Read chunk
    if(!ReadFile(hFile, buff, read, &done, 0) || read != done)
    {
      // TODO handle read error correctly (if at all possible)
      CloseHandle(hFile);
      return false;
    }

    // Write chunk
    context.response.send(buff, read);

    // Update sent data
    sent += read;
  }

  // Close http_response
  context.response.finish();

  // Close the file
  CloseHandle(hFile);

  // Succeeded
  return true;
}
