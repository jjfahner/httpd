#ifndef FSHANDLER_H
#define FSHANDLER_H

#include "handler.h"

class filesystem_handler : public mime_handler
{
public:

  //
  // Construction
  //
  filesystem_handler(String const& path, WIN32_FIND_DATA const& wfd);

  //
  // Handler
  //
  bool handle(http_context& context);
  
private:

  //
  // Handler stub
  //
  bool handler(http_context& context);

  //
  // Implements handle_get & handle_post
  //
  bool handler_impl(http_context&);


  //
  // Member data
  //
  String     m_path;
  WIN32_FIND_DATA m_wfd;

};


#endif  // #ifndef FSHANDLER_H
