#ifndef SOCKET_H
#define SOCKET_H

#include "class_state.h"
#include <memory>

enum socket_states 
{ 
  ss_closed     = 0x01,   // No open socket (default state)
  ss_rclosed    = 0x02,   // Read end is closed
  ss_sclosed    = 0x04,   // Write end is closed
  ss_listening  = 0x08,   // Waiting for connections
  ss_connecting = 0x10,   // Attempting to establish a connection
  ss_connected  = 0x20,   // Connection established
};

class socket : public class_state<socket_states>
{
public:

  enum close_types 
  { 
    cs_recv,  // Close receiving end of socket
    cs_send,  // Close sending end of socket
    cs_both   // Close entire socket
  };

  enum sync_modes
  {
    sm_default,             // Use default (current) settings
    sm_sync,                // Synchronous
    sm_async                // Asynchronous
  };

  // Base exception
  class exception : public std::exception 
  {
  public:
    exception(std::string const& what) : std::exception(what.c_str()) {}
  };

  // Exception from network layer
  class socket_exception : public exception
  {
    static std::string format_error(std::string const& what, int error);
  public:
    socket_exception(std::string const& what, int error);
  };

  //
  // Construction
  //
  socket();
  virtual ~socket();

  //
  // Construct a connected socket
  //
  socket(std::string const& hostname, int port = 80, sync_modes = sm_default);

  //
  // Construct a listening socket
  //
  socket(int port, int backlog, sync_modes = sm_default);

  //
  // Construct a connecting socket (this is *not* copy-construction)
  //
  socket(socket& from, sync_modes = sm_default);

  //
  // Sync/async mode
  //
  sync_modes sync_mode() const;
  void make_sync();
  void make_async();

  //
  // Local port
  //
  int port() const;

  //
  // Remote ip/port/hostname
  //
  std::string peer_addr() const;
  std::string peer_name() const;
  int         peer_port() const;

  //
  // Connect to a remote socket
  //
  void connect(std::string const& hostname, int port = 80, sync_modes = sm_default);
  
  //
  // Listen for incoming connections
  //
  void listen(int port = 80, int backlog = 1, sync_modes = sm_default);

  //
  // Accept an incoming connection
  //
  void accept(socket const& s, sync_modes = sm_default);

  //
  // Close the connection
  //
  void close(close_types cs = cs_both);

  //
  // Send data
  //
  void send(std::string const&, int timeout = 0);
  void send(void* pbuf, int size, int timeout = 0);
  void send(char const* pbuf, int size = 0, int timeout = 0);

  //
  // Available data on socket
  //
  int avail(int timeout = 0) const;

  //
  // Receive data
  //
  int recv(void* pbuf, int size);

  //
  // Called when the connection is established
  //
  virtual void on_connect() {}

  //
  // Called when the connection is closed
  //
  virtual void on_disconnect() {}

  //
  // Called when a connection http_request arrives
  //
  virtual void on_accept() {}

private:

  //
  // Internal handlers
  //
  void impl_on_connect();
  void impl_on_disconnect();
  void impl_on_accept();

  //
  // Implementation details
  //
  void init();
  void reset(bool do_init = true);

  //
  // Listening thread
  //
  void threadproc();

  //
  // Member data
  //
  UINT_PTR        m_socket;
  HANDLE          m_event;
  HANDLE          m_tevent;
  bool            m_async;
  int             m_port;

  mutable std::string m_peer_addr;
  mutable std::string m_peer_name;
  mutable int         m_peer_port;

  class wsainit;
  std::auto_ptr<wsainit> m_wsainit;
};


#define SOCKET_EXCEPTION(what)  \
  throw socket_exception(what, WSAGetLastError())

#endif  // #ifndef SOCKET_H
