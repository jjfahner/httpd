#ifndef RESOLVER_H
#define RESOLVER_H

class http_context;
class mime_handler;

//
// The mime_resolver class is responsible for 
// creating a mime_handler based on a http_site and http_request
//
class mime_resolver
{
public:
  virtual ~mime_resolver(){}
  virtual mime_handler* resolve(http_context& context, String const& uri) = 0;
};


#endif  // #ifndef RESOLVER_H
