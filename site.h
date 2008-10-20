#ifndef SITE_H
#define SITE_H

#include "class_state.h"
#include "extmap.h"
#include "misc.h"
#include <list>

class http_context;
class mime_handler;
class mime_resolver;
class xmlconfig;

enum site_states 
{
  ss_stopped  = 1,
  ss_starting = 2,
  ss_running  = 4,
  ss_stopping = 8
};

class http_site : public class_state<site_states>
{
public:

  //
  // Site registration entry
  //
  struct alias 
  {
    String m_name;
    int         m_port;
  };

  //
  // Mime type
  //
  struct mime_type
  {
    mime_type(String const& extension  = "", 
              String const& type       = "", 
              mime_resolver* resolver       = 0, 
              mime_handler* handler         = 0) 
              : 
              m_extension (extension), 
              m_type      (type), 
              m_resolver  (resolver),
              m_handler   (handler) { }

    String     m_extension;
    String     m_type;
    mime_resolver*  m_resolver;
    mime_handler*   m_handler;
  };

  //
  // Alias list
  //
  typedef std::list<alias> aliaslist;

  //
  // String list
  //
  typedef std::list<String> stringlist;

  //
  // String map
  //
  typedef extmap<String, mime_type, httpd_nocase_cmp > mime_map;

  //
  // List from errornumber to page
  //
  typedef std::map<int, String> errordoc_map;

  //
  // Construction
  //
   http_site(String const& name, bool autostart = true);
  ~http_site();

  //
  // Site name
  //
  String const& name() const;

  //
  // Default documents
  //
  stringlist default_docs;

  //
  // Current state
  //
  bool running() const;

  //
  // Start/stop the http_site
  //
  void start();
  void stop();

  //
  // Start this http_site when the server starts
  //
  bool autostart() const;
  void set_autostart(bool);

  //
  // Add an alias for the http_site
  //
  void add_alias(String const& name, int port);

  //
  // Retrieve alias list
  //
  aliaslist const& aliasses() const;

  //
  // Set a mime type
  //
  void set_mime_type(
    String const& extension, 
    String const& type, 
    mime_resolver* = 0, 
    mime_handler*  = 0, 
    bool is_default = false);

  //
  // Retrieve a mime type or null if it doesn't exist
  //
  mime_type const* get_mime_type(String const& type);

  //
  // Set the default mime resolver. Returns the current one
  //
  void set_default_resolver(mime_resolver* resolver);

  //
  // Handle a http_request
  //
  bool handle_request(http_context&);

  //
  // Server string - override on http_site level
  //
  String const& server() const;
  void set_server(String const&);

  //
  // Error documents
  //
  void set_errorpage(int error, String const& path);

private:

  //
  // Resolve a filename
  //
  mime_handler* resolve(http_context&, String const&) const;

  //
  // Members
  //
  String       m_name;
  String       m_server;
  aliaslist         m_aliasses;
  errordoc_map      m_errordocs;
  bool              m_autostart;
  bool              m_running;
  mime_type         m_mime_default;
  mime_map          m_mime_types;
};


inline String 
file_extension(String const& file)
{
  String::size_type pos = file.find_last_of('.');
  if(pos == String::npos || pos < file.find_last_of('/'))
  {
    return "";
  }
  return file.substr(pos + 1);
}


#endif // #ifndef SITE_H
