#include "precomp.h"
#include "handler.h"
#include "context.h"

bool 
mime_handler::handle_not_implemented(http_context& context)
{
  context.response.set_status("501 Not Implemented");
  context.response.finish();
  return true;
}

bool 
mime_handler::handle_get(http_context& context)
{
  return handle_not_implemented(context);
}

bool 
mime_handler::handle_post(http_context& context)
{
  return handle_not_implemented(context);
}

bool 
mime_handler::handle_connect(http_context& context)
{
  return handle_not_implemented(context);
}

bool 
mime_handler::handle_custom(http_context& context)
{
  return handle_not_implemented(context);
}

bool 
mime_handler::handle_delete(http_context& context)
{
  return handle_not_implemented(context);
}

bool 
mime_handler::handle_head(http_context& context)
{
  return handle_not_implemented(context);
}

bool 
mime_handler::handle_none(http_context& context)
{
  return handle_not_implemented(context);
}

bool 
mime_handler::handle_options(http_context& context)
{
  return handle_not_implemented(context);
}

bool 
mime_handler::handle_put(http_context& context)
{
  return handle_not_implemented(context);
}

bool 
mime_handler::handle_trace(http_context& context)
{
  return handle_not_implemented(context);
}

