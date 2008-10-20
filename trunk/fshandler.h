#ifndef FSHANDLER_H
#define FSHANDLER_H

#include "handler.h"

class filesystem_handler : public mime_handler
{
public:

  //
  // Construction
  //
  filesystem_handler(std::string const& path, WIN32_FIND_DATA const& wfd);

  //
  // GET handler
  //
  bool handle_get(http_context& context)
  {
    return handler_stub(context);
  }

  //
  // POST handler
  //
  bool handle_post(http_context& context)
  {
    return handler_stub(context);
  }
  
private:

  //
  // Handler stub
  //
  bool handler_stub(http_context& context)
  {
    try
    {
      bool res = handler_impl(context);
      delete this;
      return res;
    }
    catch(...)
    {
      delete this;
      return false;
    }
  }

  //
  // Implements handle_get & handle_post
  //
  bool handler_impl(http_context&);


  //
  // Member data
  //
  std::string     m_path;
  WIN32_FIND_DATA m_wfd;

};


#endif  // #ifndef FSHANDLER_H
