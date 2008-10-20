#ifndef HTTPD_H
#define HTTPD_H

#include <windows.h>

#include "threadpool.h"
#include "listener.h"
#include "site.h"
#include "sync.h"

#include "class_state.h"

//
// Server states
//
enum httpd_states 
{
  ds_stopped  = 1,
  ds_starting = 2,
  ds_running  = 4,
  ds_stopping = 8
};

//
// Server implementation
//
class http_daemon : 
  public class_state<httpd_states>,
  public threadpool
{
public:

  typedef std::map<String, http_site*, httpd_nocase_cmp > site_map;
  typedef site_map::const_iterator site_iterator;

  //
  // Singleton instance
  //
  static http_daemon& instance();

  //
  // Open the log file
  //
  void init_logfile(String filename = "");

  //
  // Add a line to the log
  //
  void log(String line);

  //
  // Start/stop the server
  //
  void start();
  void stop();

  //
  // Start/stop a http_site by name
  //
  void start_site(String const& name);
  void stop_site(String const& name);

  //
  // Add a http_site
  //
  void add_site(http_site*);

  //
  // Retrieve a http_site
  //
  http_site* get_site(String const& name, int port) const;

  //
  // Site enumerators
  //
  site_iterator site_begin() const;
  site_iterator site_end() const;

  //
  // Header logging
  //
  bool log_request_headers;
  bool log_response_headers;

  //
  // Retrieve port for listener
  //
  int get_allocated_port() const;

private:

  //
  // Construction
  //
  http_daemon();
  ~http_daemon();

  //
  // Close logfile
  //
  void term_logfile();

  //
  // Listening ports
  //
  friend void http_site::start();
  friend void http_site::stop();
  void start_listener(int port, http_site*, String const& name);
  void stop_listener(int port, http_site*,  String const& name);

  //
  // Current number of connections
  //
  int m_connections;

  //
  // Site list
  //
  site_map m_sites;

  //
  // Listeners
  //
  typedef std::map<int, listener*> listener_map;
  listener_map m_listeners;

  //
  // Log file
  //
  String m_logfile;
  HANDLE      m_hlogfile;
};

//
// Instance of the server
//
#define httpd (http_daemon::instance())

//
// Inline implementations
//
inline http_daemon::site_iterator
http_daemon::site_begin() const
{
  return m_sites.begin();
}

inline http_daemon::site_iterator 
http_daemon::site_end() const
{
  return m_sites.end();
}

#endif  // #ifndef HTTPD_H
