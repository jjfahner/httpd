#ifndef REQUEST_H
#define REQUEST_H

#include "connection.h"
#include "header_map.h"

enum request_methods
{
  rm_unknown,
  rm_get,
  rm_post,
  rm_connect,
  rm_custom,
  rm_delete,
  rm_head,
  rm_none,
  rm_options,
  rm_put,
  rm_trace
};

class http_request 
{
public:

  //
  // Construction
  //
   http_request(connection&);
  ~http_request();

  //
  // Parse http_request
  //
  bool parse_request();

  //
  // HTTP http_request method
  //
  request_methods method() const;

  //
  // Requested host name/port
  //
  String const&  host() const;
  int                 port() const;

  //
  // Headers
  //
  header_map const& headers;

  //
  // Request variables
  //
  header_map const& getvars;

  //
  // Post variables
  //
  header_map const& postvars;

  //
  // Request body
  //
  String body() const;

private:

  //
  // Parse the header block
  //
  bool parse_headers();

  //
  // Parse an individual header line
  //
  bool parse_line(String const&);

  //
  // Parse the get params
  //
  bool parse_url_params(String const&, header_map&);

  //
  // Parse POST form data
  //
  bool parse_postdata();

  //
  // Members
  //
  connection&     m_con;
  request_methods m_method;
  header_map      m_headers;
  header_map      m_getvars;
  header_map      m_postvars;
  String     m_body;

};


#endif  // #ifndef REQUEST_H
