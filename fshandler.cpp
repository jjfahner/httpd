#include "precomp.h"
#include "fshandler.h"
#include "context.h"
#include "httpdate.h"

struct AutoHandle
{
  HANDLE m_handle;
  AutoHandle(HANDLE handle = INVALID_HANDLE_VALUE) : m_handle (handle)
  {
  }
  ~AutoHandle()
  {
    if(m_handle != INVALID_HANDLE_VALUE)
    {
      CloseHandle(m_handle);
    }
  }
  operator HANDLE () const
  {
    return m_handle;
  }
};

filesystem_handler::filesystem_handler(String const& path, WIN32_FIND_DATA const& wfd) :
m_path  (path)
{
  memcpy(&m_wfd, &wfd, sizeof(m_wfd));
}

bool 
filesystem_handler::handle(http_context& context)
{
  if(context.request.method() == rm_get)
  {
    return handler(context);
  }
  context.response.send_error(501);
  return true;
}

bool 
filesystem_handler::handler(http_context& context)
{
  bool res = false;
  try {
    res = handler_impl(context);
  }
  catch(...) {
  }

  delete this;
  
  return res;
}

bool 
filesystem_handler::handler_impl(http_context& context)
{
  // Check for large files
  if(m_wfd.nFileSizeHigh > 0)
  {
    context.response.send_error(500);
    return true;
  }

  // Set content-location
  if(context.request.headers["HTTP_URI"] != context.resolved_uri)
  {
    context.response.headers["Content-Location"] = context.resolved_uri;
  }

  // Determine last modified date
  httpdate omd(m_wfd.ftLastWriteTime);

  // Handle If-Modified-Since
  if(context.request.headers.contains("If-Modified-Since"))
  {
    httpdate imd;
    if(imd.set(context.request.headers["If-Modified-Since"]) && imd == omd)
    {
      // Not modified
      context.response.send_not_modified();
      return true;
    }
  }

  // Set last modified date
  context.response.headers["Last-Modified"] = omd.str();

  // Send expiry of 1 day to lessen load
  // TODO make configurable
  context.response.headers["Expires"] = httpdate().add_seconds(86400);

  // Enable caching
  // TODO if there's any security, this is wrong !
  context.response.headers["Cache-Control"] = "public";

  // Open file
  AutoHandle hFile(::CreateFile(m_path.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0));
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
    return true;
  }

  // Determine file size
  DWORD size = m_wfd.nFileSizeLow;

  static const size_t bufsize = 4096;

  // Write file in chunks
  char  buff[bufsize];
  DWORD read, done;
  while(size)
  {
    // Determine read size
    read  = size < bufsize ? size : bufsize;
    size -= read;

    // Read chunk
    if(!ReadFile(hFile, buff, read, &done, 0) || read != done)
    {
      context.response.send_error(500);
      return true;
    }

    // Write chunk
    context.response.send(buff, read);
  }

  // Close http_response
  context.response.finish();
  return true;
}
