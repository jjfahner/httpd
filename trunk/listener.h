#ifndef LISTENER_H
#define LISTENER_H

#include "socket.h"
#include "misc.h"

#include <map>

class http_site;

class listener : public socket
{
public:

  //
  // Mapping between name and http_site
  //
  typedef std::map<std::string, http_site*, httpd_nocase_cmp > site_map;

  //
  // Construction
  //
  listener(int port);
  virtual ~listener();

  //
  // Port number
  //
  int port() const;

  //
  // Start/stop listener
  //
  void start(); // TODO
  void stop();

  //
  // Retrieve a http_site by name
  //
  http_site* get_site(std::string const& name);

  //
  // Add/remove sites
  //
  void attach(http_site* s, std::string const& name);
  long detach(http_site* s, std::string const& name);

private:

  //
  // Listen for incoming requests
  //
  virtual void on_accept();

  //
  // Members
  //
  site_map  m_sites;

};

#endif  // #ifndef LISTENER_H
