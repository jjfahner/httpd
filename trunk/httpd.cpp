#include "precomp.h"
#include "httpd.h"
#include "httpdate.h"
#include "connection.h"
#include "fsresolver.h"

/*static*/ http_daemon& 
http_daemon::instance()
{
  static http_daemon _instance;
  return _instance;
}

http_daemon::http_daemon() :
class_state<httpd_states>(ds_stopped),
threadpool            (5, 5),
m_connections         (0),
log_request_headers   (false),
log_response_headers  (false),
m_hlogfile            (0)
{
}

http_daemon::~http_daemon()
{
  try
  {
    stop();

    while(m_listeners.size())
    {
      delete m_listeners.begin()->second;
      m_listeners.erase(m_listeners.begin());
    }

    while(m_sites.size())
    {
      delete m_sites.begin()->second;
      m_sites.erase(m_sites.begin());
    }
  }
  catch(std::exception const& e)
  {
    log(e.what());
  }
  catch(...)
  {
    log("Unexpected exception");
  }

  term_logfile();
}

void 
http_daemon::init_logfile(std::string filename)
{
  // Close current file
  term_logfile();

  // Determine temp path for logging
  if(filename == "")
  {
    char buff[MAX_PATH];
    GetTempPath(MAX_PATH, buff);
    filename = buff;
    filename += "httpd.log";
  }

  // Create log file
  m_hlogfile = ::CreateFile(filename.c_str(), GENERIC_WRITE, 
                      FILE_SHARE_READ, 0, OPEN_ALWAYS, 0, 0);

  // Check handle
  if(m_hlogfile == INVALID_HANDLE_VALUE)
  {
    m_hlogfile = 0;
    return;
  }

  // Store name
  m_logfile = filename;

  // Move to append
  SetFilePointer(m_hlogfile, 0, 0, SEEK_END);

  // Write initial entry
  log("====================================================================");
  log("httpd started");
}

void 
http_daemon::term_logfile()
{
  if(m_hlogfile == 0)
  {
    return;
  }

  log("httpd stopped");

  CloseHandle(m_hlogfile);
  m_hlogfile = 0;
}

void 
http_daemon::log(std::string line)
{
  CLASS_STATE(0);
  DWORD w;

  if(m_hlogfile == 0)
  {
    return;
  }

  // Prepend time/date
  line = httpdate().str() + ": " + line + "\r\n";

  // Write to logfile
  WriteFile(m_hlogfile, line.c_str(), line.length(), &w, 0);
  FlushFileBuffers(m_hlogfile);
}

void 
http_daemon::add_site(http_site* s)
{
  CLASS_STATE(ds_stopped);

  if(m_sites.find(s->name()) != m_sites.end())
  {
    throw std::exception("http_site name is not unique");
  }
  m_sites[s->name()] = s;
}

http_site* 
http_daemon::get_site(std::string const& name, int /*port*/) const
{
  site_map::const_iterator it;
  if((it = m_sites.find(name)) == m_sites.end())
  {
    return 0;
  }
  return it->second;
}

void 
http_daemon::start()
{
  CLASS_STATE(ds_stopped);

  // Start the threadpool
  threadpool::start();

  // Begin startup
  set_state(ds_starting);
  log("Initializing...");

  // Enumerate sites
  site_map::iterator it, ie;
  it = m_sites.begin();
  ie = m_sites.end();
  for(; it != ie; ++it)
  {
    // Start if autostart only
    if(it->second->autostart())
    {
      it->second->start();
    }
  }

  // Started
  set_state(ds_running);
  log("Running");
}

void 
http_daemon::stop()
{
  CLASS_STATE(ds_running|ds_stopped);
  if(state() == ds_stopped)
  {
    return;
  }

  set_state(ds_stopping);
  log("Stopping...");

  // Enumerate sites
  site_map::iterator it, ie;
  it = m_sites.begin();
  ie = m_sites.end();
  for(; it != ie; ++it)
  {
    // Stop running http_site
    if(it->second->running())
    {
      it->second->stop();
    }
  }

  // Stop thread pool
  threadpool::stop();

  // Stopped
  set_state(ds_stopped);
  log("Stopped");
}

void 
http_daemon::start_site(std::string const& name)
{
  CLASS_STATE(ds_running);

  site_map::iterator it;
  if((it = m_sites.find(name)) == m_sites.end())
  {
    // TODO throw
  }
  else
  {
    // Start the http_site
    if(!it->second->running())
    {
      it->second->start();
    }
  }
}

void 
http_daemon::stop_site(std::string const& name)
{
  CLASS_STATE(ds_running);

  site_map::iterator it;
  if((it = m_sites.find(name)) == m_sites.end())
  {
    // TODO throw
  }
  else
  {
    // Stop the http_site
    if(it->second->running())
    {
      it->second->stop();
    }
  }
}

void 
http_daemon::start_listener(int port, http_site* s, std::string const& name)
{
  CLASS_STATE(ds_starting|ds_running);

  listener_map::iterator it;
  if((it = m_listeners.find(port)) == m_listeners.end())
  {
    m_listeners[port] = new listener(port);
    it = m_listeners.find(port);
  }
  it->second->attach(s, name);
}

void 
http_daemon::stop_listener(int port, http_site* s, std::string const& name)
{
  CLASS_STATE(ds_running|ds_stopping);

  listener_map::iterator it;
  if((it = m_listeners.find(port)) != m_listeners.end())
  {
    if(it->second->detach(s, name) == 0)
    {
      delete it->second;
      m_listeners.erase(it);
    }
  }
}

int 
http_daemon::get_allocated_port() const
{
  listener_map::const_iterator it = m_listeners.find(0);
  if(it == m_listeners.end())
  {
    return 0;
  }
  return it->second->port();
}
