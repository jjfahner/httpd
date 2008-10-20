#ifndef RESPONSE_H
#define RESPONSE_H

#include "header_map.h"
#include "httpdate.h"

class connection;
class http_request;

enum response_states
{
  rs_initial  = 1,
  rs_headers  = 2,
  rs_finished = 3
};

enum http_versions
{
  httpver_1_0 = 0,
  httpver_1_1 = 1
};

#define RESPONSE_BUFSIZE (4096)

class http_response 
{
public:
  
  //
  // Construction
  //
  http_response(connection&);
  ~http_response();

  //
  // Version
  //
  http_versions version() const;
  void set_version(http_versions);

  //
  // Status
  //
  String const& status() const;
  void set_status(String const&);

  //
  // Headers
  //
  header_map headers;

  //
  // Send a generic error
  //
  void send_error(int error);

  //
  // Unauthorized
  //
  void send_unauthorized();

  //
  // Specific responses  
  //
  void send_not_modified();

  //
  // Write headers to client
  //
  void send_headers();

  //
  // Write data to client
  //
  void send(String const&);
  void send(char const* data, int size = 0);

  //
  // Flush output buffer
  //
  void flush();

  //
  // Finish http_response
  //
  void finish();

private:

  //
  // Create error document
  //
  void create_error_doc(String& msg);

  //
  // Implementation
  //
  void impl_send_headers();

  //
  // Member data
  //
  connection&         m_con;
  http_versions       m_ver;
  String         m_status;
  char                m_buffer[RESPONSE_BUFSIZE];
  char*               m_bufptr;
  response_states     m_state;
  DWORD               m_start;
};

//
// Operators for writing data
//
inline http_response& operator << (http_response& os, String const& s)
{
  os.send(s.c_str(), s.length());
  return os;
}

inline http_response& operator << (http_response& os, char const* s)
{
  os.send(s, 0);
  return os;
}

#endif  // #ifndef RESPONSE_H
