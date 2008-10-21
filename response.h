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


template <int blocksize_t = 1024>
class buffer_t
{
public:

  enum { blocksize = blocksize_t };

  struct block
  {
    block() : m_size (0) {}
    char   m_data[blocksize];
    size_t m_size;
  };

  typedef std::list<block> BlockList;

public:

  size_t size() const
  {
    return m_list.size() == 0 ? 0 : 
      (m_list.size() - 1 * blocksize) + m_list.back().m_size;
  }

  void append(char const* data, size_t size)
  {
    while(size)
    {
      // Ensure there's a block with available space
      if(m_list.size() == 0 || 
         m_list.back().m_size == blocksize)
      {
        m_list.push_back(block());
      }

      // Determine copy size
      size_t copy = blocksize - m_list.back().m_size;
      if(size < copy) copy = size;

      // Copy data into block
      memcpy(m_list.back().m_data, data, copy);
      m_list.back().m_size += copy;

      // Adjust sizes
      size -= copy;
      data += copy;
    }
  }

  block const* get_block() const
  {
    return m_list.size() ? &m_list.front() : 0;
  }

  void forget_block()
  {
    m_list.pop_front();
  }

private:

  BlockList m_list;

};

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
  // Flush implementation
  //
  void impl_flush(bool final);

  //
  // Create error document
  //
  void create_error_doc(String& msg);

  //
  // Implementation
  //
  void impl_send_headers();
  
  typedef buffer_t<> buffer;

  //
  // Member data
  //
  connection&     m_con;
  http_versions   m_ver;
  String          m_status;
  buffer          m_buffer;
  response_states m_state;
  DWORD           m_start;
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
