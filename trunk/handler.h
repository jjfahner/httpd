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
  virtual bool handle_get(http_context&);
  virtual bool handle_post(http_context&);
  virtual bool handle_connect(http_context&);
  virtual bool handle_custom(http_context&);
  virtual bool handle_delete(http_context&);
  virtual bool handle_head(http_context&);
  virtual bool handle_none(http_context&);
  virtual bool handle_options(http_context&);
  virtual bool handle_put(http_context&);
  virtual bool handle_trace(http_context&);

private:

  //
  // Generic not-implemented handler
  //
  bool handle_not_implemented(http_context&);

};


#endif  // #ifndef HANDLER_H
