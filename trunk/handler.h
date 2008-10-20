#ifndef HANDLER_H
#define HANDLER_H

class http_context;

//
// A mime-handler handles a single http_request for a mime type
//
class mime_handler 
{
public:

  //
  // Destruction
  //
  virtual ~mime_handler(){}

  //
  // Handlers for the different http_request types
  //
  virtual bool handle(http_context&) = 0;

};


#endif  // #ifndef HANDLER_H
