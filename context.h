#ifndef CONTEXT_H
#define CONTEXT_H

#include "site.h"
#include "request.h"
#include "response.h"
#include "any.h"

//
// Context contains all objects that are part of a single http_request
//
class http_context 
{
public:

  //
  // Construction
  //
  http_context(http_site const&, http_request const&, http_response&);

  //
  // Website
  //
  http_site const& site;

  //
  // Request
  //
  http_request const& request;

  //
  // Response
  //
  http_response& response;

  //
  // The uri which was resolved
  //
  std::string resolved_uri;
  std::string resolved_ext;

  //
  // Username
  //
  std::string username;

  //
  // User data. May be used freely
  //
  any userdata;

private:

  //
  // Not allowed
  //
  http_context const& operator = (http_context const&);

};


inline 
http_context::http_context(http_site const& _site, http_request const& _request, http_response& _response) :
site    (_site),
request (_request),
response(_response)
{
}


#endif  // #ifndef CONTEXT_H
