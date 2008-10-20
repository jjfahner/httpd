#include "precomp.h"
#include "listener.h"
#include "connection.h"
#include "httpd.h"

listener::listener(int port)
{
  socket::listen(port, 1, sm_async);

  char buf[30];
  sprintf_s(buf, "Listening on port %d", socket::port());
  httpd.log(buf);
}

listener::~listener()
{
  socket::close();
}

int 
listener::port() const
{
  return socket::port();
}

http_site*
listener::get_site(String const& name)
{
  site_map::iterator it;
  if((it = m_sites.find(name)) == m_sites.end())
  {
    return 0;
  }
  return it->second;
}

void 
listener::attach(http_site* s, String const& name)
{
  if(m_sites.find(name) != m_sites.end())
  {
    throw std::exception("Name not unique");
  }
  m_sites[name] = s;
}

long 
listener::detach(http_site*, String const& name)
{
  site_map::iterator it;
  if((it = m_sites.find(name)) == m_sites.end())
  {
    throw std::exception("Site doesn't exist");
  }
  m_sites.erase(it);
  return m_sites.size();
}

void 
listener::on_accept()
{
  httpd.queue(new connection(this), true, 100);
}
