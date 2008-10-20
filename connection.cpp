#include "precomp.h"
#include "connection.h"
#include "listener.h"
#include "httpd.h"
#include "context.h"
#include "uri.h"

connection::connection(listener* l) :
m_listener(l),
m_buffer  (0),
m_buflen  (0),
m_bufuse  (0),
m_bufpos  (0)
{
  // Use a reasonable default buffer size
  resize_buffer(1024);

  // Accept the incoming connection
  accept(*m_listener, sm_sync);

  // Initialize log stream
  m_log << "Incoming connection on port " << m_listener->port() << " from " << peer_addr() << ":" << peer_port();
  log(m_log.str());
}

connection::~connection()
{
  // Free buffer memory
  resize_buffer(0);

  // Write connection log
  log("Disconnect");
// m_log << "Disconnected";
// httpd.log(m_log.str());
}

void 
connection::log(std::string const& line)
{
  httpd.log(line);
  //m_log << line << "\n\t";
}

listener* 
connection::get_listener() const
{
  return m_listener;
}

void 
connection::perform()
{
  try
  {
    handle_request();
  }
  catch(std::exception const& e)
  {
    log(e.what());
  }
  catch(...)
  {
    log("Unexpected exception");
  }
}

void 
connection::handle_request()
{
  static const int def_keepalive = 3000;
  static const int timeout       = 3000;
  static const int max_requests  = 10;
  int keepalive = def_keepalive;
  int requests  = 0;

  // Outer loop represents keep-alive
  for(;;)
  {
    // Handle existing data or wait for more
    if(m_bufuse == m_bufpos)
    {
      if(avail(keepalive) == 0)
      {
        close();
        return;
      }
    }

    // Create http_request context
    http_request   request (*this);
    http_response  response(*this);

    // Parse http_request headers
    if(!request.parse_request())
    {
      // Bad http_request
      response.send_error(400);
      return;
    }

    // Set http version
    if(request.headers["HTTP_VERSION"] == "HTTP/1.0")
    {
      response.set_version(httpver_1_0);
    }
    else if(request.headers["HTTP_VERSION"] == "HTTP/1.1")
    {
      response.set_version(httpver_1_1);
    }
    else
    {
      // Invalid version
      response.send_error(505);
      return;
    }
    
    // Find website
    http_site* site;
    if((site = m_listener->get_site(request.headers["HTTP_HOST"])) == 0 &&
       (site = m_listener->get_site("*")) == 0)
    {
      response.send_error(503);
      return;
    }

    // Check maximum number of requests for 1.1
    if(response.version() == httpver_1_1)
    {
      if(++requests == max_requests)
      {
        response.headers["Connection"] = "close";
      }
      else if(request.headers["Connection"] == "keep-alive")
      {
        response.headers["Connection"] = "keep-alive";
      }
    }

    // Create context
    http_context context(*site, request, response);

    // Let http_site handle request
    try
    {
      site->handle_request(context);
    }
    catch(std::exception const& e)
    {
      log(e.what());
      close();
      return;
    }
    catch(...)
    {
      log("Unknown exception");
      close();
      return;
    }

    // HTTP/1.0 requires us to close the connection
    if(response.version() == httpver_1_0)
    {
      close();
      return;    
    }

    // Determine keepalive
    if(response.headers["Connection"] == "close")
    {
      keepalive = 0;
    }
    else if(request.headers["Connection"] == "keep-alive")
    {
      if(request.headers.contains("Keep-alive"))
      {
        keepalive = request.headers["Keep-alive"];
      }
      else
      {
        keepalive = def_keepalive;
      }
    }
    else
    {
      keepalive = def_keepalive;
    }

    // Close the connection if keepalive is expired
    if(keepalive <= 0)
    {
      close();
      return;
    }
  }
}

void 
connection::timeout()
{
  // Create a simple response
  http_response response(*this);

  // Send a 'service not available' message
  response.headers["Connection"] = "close";
  response.send_error(503);

  // Close socket
  close();
}

void 
connection::resize_buffer(int newlen)
{
  // No change
  if(newlen == m_buflen)
  {
    return;
  }

  // Resize to zero
  if(newlen == 0)
  {
    delete [] m_buffer;
    m_buffer = 0;
    m_buflen = 0;
    m_bufuse = 0;
    m_bufpos = 0;
    return;
  }

  // Resize to smaller size
  if(newlen < m_buflen)
  {
    NOT_IMPLEMENTED;
  }

  // Allocate new buffer
  char* buffer = new char[newlen];

  // Move pointer back to start if empty,
  // otherwise copy remainder into buffer
  if(m_bufpos == m_bufuse)
  {
    m_bufpos = 0;
    m_bufuse = 0;
  }
  else
  {
    int used_len = m_bufuse - m_bufpos;
    memcpy(buffer, m_buffer + m_bufpos, used_len);
    m_bufuse = used_len;
    m_bufpos = 0;
  }

  // Delete old buffer
  delete [] m_buffer;

  // Assign new values
  m_buffer = buffer;
  m_buflen = newlen;
}

bool
connection::read_avail()
{
  // See if there's data available
  int len = avail(5000);  // TODO make setting
  if(len == 0) 
  {
    return false;
  }

  // Move back to start if possible
  if(m_bufpos == m_bufuse)
  {
    m_bufpos = 0;
    m_bufuse = 0;
  }

  // See if there's enough space
  if(len <= m_buflen - m_bufuse)
  {
    // See if the used bits needs to be moved
    if(len > m_buflen - m_bufpos - m_bufuse)
    {
      memmove(m_buffer + m_bufpos, m_buffer, m_bufuse);
      m_bufpos = 0;
    }
  }
  else
  {
    // Resize to fit
    resize_buffer(m_bufuse + len);
  }

  // Read data into buffer
  m_bufuse += recv(m_buffer + m_bufpos, len);

  // Done
  return true;
}

bool 
connection::read_line(std::string& line, bool peek)
{
  // Check if we have data
  if(m_bufuse == m_bufpos)
  {
    // If peeking, don't wait
    if(peek)
    {
      return false;
    }

    // Wait for data
    if(!read_avail())
    {
      return false;
    }
  }

  // Find the line ending
  char const* s = m_buffer + m_bufpos;
  char const* e = m_buffer + m_bufuse;
  char const* p = std::find(s, e, '\r');
  if(p == e)
  {
    // No line feed in current buffer
    // TODO try another read ?
    return false;
  }
  else
  {
    // Copy string into line
    line.assign(s, p - s);

    // Advance position
    if(!peek)
    {
      m_bufpos += p - s + 2;
    }

    // Succeeded
    return true;
  }
}

bool 
connection::read_line(std::string& line, int size)
{
  // Ensure we have the data. Basically, this loops,
  // reading data every iteration, using a timeout.
  // The assumption is that if data keeps coming in,
  // no matter how slow, we'll eventually reach size
  while(m_bufuse - m_bufpos < size)
  {
    // Wait for more data
    if(!read_avail())
    {
      return false;
    }
  }

  // Temporary null-termination
  char old = *(m_buffer + m_bufpos + size);
  *(m_buffer + m_bufpos + size) = 0;

  // Copy to string
  line = m_buffer + m_bufpos;
  
  // Restore string
  *(m_buffer + m_bufpos + size) = old;

  // Adjust size
  m_bufpos += size;

  // Done
  return true;
}

bool 
connection::read_bytes(char* buf, int size)
{
  // Ensure we have the data. Basically, this loops,
  // reading data every iteration, using a timeout.
  // The assumption is that if data keeps coming in,
  // no matter how slow, we'll eventually reach size
  while(m_bufuse - m_bufpos < size)
  {
    // Wait for more data
    if(!read_avail())
    {
      return false;
    }
  }

  // warning C4996: 'std::copy': Function call with parameters that may be unsafe
  #pragma warning(disable:4996) 

  // Copy the bytes
  std::copy(m_buffer + m_bufpos, m_buffer + m_bufpos + size, buf);
  m_bufpos += size;

  // Done
  return true;
}












