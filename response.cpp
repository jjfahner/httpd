#include "precomp.h"
#include "response.h"
#include "connection.h"
#include "httpd.h"

const String server_string = "httpd/1.0(Win32)";

http_response::http_response(connection& con) :
m_con     (con),
m_ver     (httpver_1_1),
m_state   (rs_initial),
m_start   (GetTickCount()),
m_buffered(true)
{
  // Default http_response status
  m_status = "200 OK";

  // Date
  if(!headers.contains("Date"))
  {
    headers["Date"] = httpdate().str();
  }

  // Empty server string
  if(!headers.contains("Server"))
  {
    headers["Server"] = server_string;
  }
}

http_response::~http_response()
{
  char buf[50];
  sprintf_s(buf, "Request processed in %d ms", GetTickCount() - m_start);
  httpd.log(buf);
}

http_versions 
http_response::version() const
{
  return m_ver;
}

void 
http_response::set_version(http_versions ver)
{
  // State must be initial
  if(m_state != rs_initial)
  {
    throw std::exception("Invalid state");
  }

  // Store version
  m_ver = ver;
}

String const& 
http_response::status() const
{
  return m_status;
}

void 
http_response::set_status(String const& s)
{
  // May not be empty
  if(s.empty())
  {
    throw std::exception("Status header may not be empty");
  }

  // State must be initial
  if(m_state != rs_initial)
  {
    throw std::exception("Invalid state");
  }

  m_status = s;
}

void 
http_response::send_headers()
{
  // Check current state
  if(m_state != rs_initial)
  {
    throw std::exception("Cannot send headers: invalid state");
  }

  // Caching is off by default
  if(!headers.contains("Cache-control"))
  {
    headers["Cache-Control"] = "no-cache";
  }

  // Determine content-type
  String ct = headers["Content-Type"];
  if(ct.empty())
  {
    ct = "application/octet-stream";
  }

  // For text, append charset
  if(ct.substr(0, 5) == "text/" && 
     ct.find(';') == String::npos)
  {
    ct += "; charset=utf-8";
  }

  // Update content-type
  headers["Content-Type"] = ct;

  // Set transfer encoding
  if(version() == httpver_1_0)
  {
    if(headers.contains("Transfer-Encoding"))
    {
      headers.remove("Transfer-Encoding");
    }
  }
  else
  {
    if(!headers.contains("Transfer-Encoding"))
    {
      headers["Transfer-Encoding"] = "chunked";
    }
    else if(headers["Transfer-Encoding"] != "chunked")
    {
      headers["Connection"] = "close";
    }
  }

  // Send the headers now
  impl_send_headers();
}

void 
http_response::impl_send_headers()
{
  String hdrs;

  // Status line
  if(version() == httpver_1_0)
  {
    hdrs += "HTTP/1.0 ";
  }
  else
  {
    hdrs += "HTTP/1.1 ";
  }

  // Status
  hdrs += m_status;

  // Log the response
  m_con.log(hdrs);

  // Headers
  hdrs += "\r\n";
  hdrs += headers.str();    // Headers
  hdrs += "\r\n";

  // Write headers to client
  m_con.send(hdrs.c_str(), hdrs.length());

  // Set state
  headers.finalize();
  m_state = rs_headers;
}

void 
http_response::send(String const& str)
{
  send(str.c_str(), str.length());
}

void 
http_response::send(char const* data, int size)
{
  // Check state
  if(m_state == rs_finished)
  {
    throw std::exception("Invalid state");
  }

  // Determine size if unknown
  if(size == 0)
  {
    size = strlen(data);
  }

  // Add to buffer
  m_buffer.append(data, size);

  // Flush when not buffered or over limit
  if(!m_buffered || m_buffer.size() > 4096)
  {
    flush();
  }
}

void 
http_response::flush()
{
  impl_flush(false);
}

void 
http_response::impl_flush(bool final)
{
  // Request done
  if(m_state == rs_finished)
  {
    return;
  }

  // Headers not sent
  if(m_state == rs_initial)
  {
    send_headers();
  }

  // Handle regular data transfer
  if(version() == httpver_1_0 || headers["Transfer-Encoding"] != "chunked")
  {
    buffer::block const* block;
    while(block = m_buffer.get_block())
    {
      m_con.send(block->m_data, block->m_size);
      m_buffer.forget_block();
    }
    return;
  }
  
  // Setup buffer
  static const size_t bufsize = buffer::blocksize + 8;
  char buf[bufsize];

  // Send blocks
  buffer::block const* block;
  while(block = m_buffer.get_block())
  {
    char* ptr = buf;

    // Write chunk header
    sprintf_s(buf, "%x\r\n", block->m_size);
    ptr += strlen(buf);

    // Write chunk data
    memcpy(ptr, block->m_data, block->m_size);
    ptr += block->m_size;

    // Write chunk trailer
    memcpy(ptr, "\r\n", 2);
    ptr += 2;

    // Send chunk
    m_con.send(buf, ptr - buf);

    // Next chunk
    m_buffer.forget_block();
  }

  // Send final chunk
  if(final)
  {
    m_con.send("0\r\n\r\n");
  }
}

void 
http_response::finish()
{
  // Flush output
  try
  {
    impl_flush(true);
  }
  catch(...)
  {
  }

  // Last state
  m_state = rs_finished;
}

void 
http_response::send_error(int error)
{
  // Can only be done if headers not sent
  if(m_state != rs_initial)
  {
    throw std::exception("Cannot send error response: invalid state");
  }

  // Set headers
  headers.clear();
  headers["Date"]               = httpdate().str();
  headers["Server"]             = server_string;
  headers["Content-Type"]       = "text/html; charset=utf-8";
  headers["Cache-Control"]      = "no-cache";
  headers["Transfer-Encoding"]  = "chunked";

  // Set correct status code
  switch(error)
  {
  case 400: m_status = "400 Bad Request"; break;
  case 401: m_status = "401 Unauthorized"; break;
  case 402: m_status = "402 Payment Required"; break;
  case 403: m_status = "403 Forbidden"; break;
  case 404: m_status = "404 Not Found"; break;
  case 405: m_status = "405 Method Not Allowed"; break;
  case 406: m_status = "406 Not Acceptable"; break;
  case 407: m_status = "407 Proxy Authentication Required"; break;
  case 408: m_status = "408 Request Timeout"; break;
  case 409: m_status = "409 Conflict"; break;
  case 410: m_status = "410 Gone"; break;
  case 411: m_status = "411 Length Required"; break;
  case 412: m_status = "412 Precondition Failed"; break;
  case 413: m_status = "413 Request Entity Too Large"; break;
  case 414: m_status = "414 Request-URI Too Long"; break;
  case 415: m_status = "415 Unsupported Media Type"; break;
  case 416: m_status = "416 Requested Range Not Satisfiable"; break;
  case 417: m_status = "417 Expectation Failed"; break;
  case 500: m_status = "500 Internal Server Error"; break;
  case 501: m_status = "501 Not Implemented"; break;
  case 502: m_status = "502 Bad Gateway"; break;
  case 503: m_status = "503 Service Unavailable"; break;
  case 504: m_status = "504 Gateway Time-out"; break;
  case 505: m_status = "505 HTTP Version not supported"; break;
  default: throw std::exception("Unknown error code");
  }

  // Headers
  impl_send_headers();

  // Body
  String msg;
  create_error_doc(msg);
  send(msg.c_str());

  // Finish http_request
  finish();
}

void 
http_response::send_unauthorized()
{
  // Can only be done if headers not sent
  if(m_state != rs_initial)
  {
    throw std::exception("Cannot send error response: invalid state");
  }

  // Set headers
  headers["Date"]               = httpdate().str();
  headers["Server"]             = server_string;
  headers["Content-Type"]       = "text/html; charset=utf-8";
  headers["Cache-Control"]      = "no-cache";
  headers["Transfer-Encoding"]  = "chunked";

  // Status
  m_status = "401 Unauthorized";

  // Headers
  impl_send_headers();

  // Body
  String msg;
  create_error_doc(msg);
  send(msg.c_str());

  // Finish http_request
  finish();
}

void 
http_response::send_not_modified()
{
  // Must be initial
  if(m_state != rs_initial)
  {
    throw std::exception("Invalid state");
  }

  // Remove some headers
  headers.remove("Content-Type");
  headers.remove("Cache-Control");
  headers.remove("Transfer-Encoding");

  // Default http_response status
  m_status = "304 Not Modified";

  // Date
  headers["Date"] = httpdate().str();

  // Server name/version
  headers["Server"] = server_string;

  // Send headers
  impl_send_headers();

  // Set finalized
  m_state = rs_finished;
}

void 
http_response::create_error_doc(String& msg)
{
  std::stringstream reply;

  // Header
  reply << "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">"
    << "<html>"
    << "<title>" << m_status << "</title>"
    << "</head>";

  // Body
  reply << "<body>"
    << "<h1>" << m_status << "</h1>"
    << "An error occurred while processing your request:<br><br>"
    << "<table>"
    << "<tr><td>HTTP error:&nbsp;</td><td>" << m_status << "</td></tr>"
    << "<tr><td>Server name:&nbsp;</td><td>" << headers["Server"].as_str() << "</td></tr>"
    << "<tr><td>Server time:&nbsp;</td><td>" << httpdate().str() << "</td></tr>";

  // Client info
  reply << "<tr><td>Client IP:&nbsp;</td><td>";
  if(m_con.peer_name() == m_con.peer_addr())
  {
    reply << m_con.peer_addr() << ":" << m_con.peer_port();
  }
  else
  {
    reply << m_con.peer_name() << "(" << m_con.peer_addr() << ":" << m_con.peer_port() << ")";
  }
  reply << "</td></tr>";

  // Finish body
  reply << "</table><br>"
    << "Please contact the administrator if this error persists.<br>"
    << "</body></html>";

  // Done
  msg = reply.str();
}

