#include "precomp.h"

//
// The overlap between socket() and class socket is solved by
// first including the socket headers, then creating a wrapper
// that unambiguously resolves to ::socket(), then including
// the socket header.
//

// winsock headers
#pragma warning(disable:4127)
#pragma warning(disable:4706)
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

// wrapper for socket()
SOCKET create_socket(int af, int type, int protocol)
{
  return socket(af, type, protocol);
}

// Local headers
#include "socket.h"
#include "thread.h"

/////////////////////////////////////////////////////////////////////////////////////
// Winsock initializer

class socket::wsainit
{
  static long* getcounter()
  {
    static long counter = 0;
    return &counter;
  }
public:
  wsainit()
  {
    if(InterlockedIncrement(getcounter()) == 1)
    {
      WSADATA wsadata;
      if(WSAStartup(MAKEWORD(2,2), &wsadata) != 0)
      {
        throw std::exception("Winsock initialization failed");
      }
    }
  }
  ~wsainit()
  {
    if(InterlockedDecrement(getcounter()) == 0)
    {
      WSACleanup();
    }
  }
};

/////////////////////////////////////////////////////////////////////////////////////
// Socket autohandle

struct sockethandle
{
  SOCKET m_handle;
  sockethandle(SOCKET s = INVALID_SOCKET) : m_handle (s) {}
  ~sockethandle() { if(m_handle != INVALID_SOCKET) closesocket(m_handle); }
  operator SOCKET () const { return m_handle; }
  void swap(SOCKET& s) { SOCKET t = s; s = m_handle; m_handle = t; }
  sockethandle const& operator = (SOCKET s) { m_handle = s; return *this; }
};


/////////////////////////////////////////////////////////////////////////////////////
// Implementation

socket::socket() :
m_wsainit (new wsainit())
{
  init();
}

socket::socket(String const& hostname, int port, sync_modes sm) :
m_wsainit (new wsainit())
{
  init();
  connect(hostname, port, sm);
}

socket::socket(int port, int backlog, sync_modes sm) :
m_wsainit (new wsainit())
{
  init();
  listen(port, backlog, sm);
}

socket::socket(socket& from, sync_modes sm) :
m_wsainit (new wsainit())
{
  init();
  accept(from, sm);
}

socket::~socket()
{
  reset(false);
}

void 
socket::init()
{
  // Socket state
  set_state(ss_closed);

  // Initialize members
  m_socket  = INVALID_SOCKET;
  m_event   = 0;
  m_async   = false;
  m_port    = 0;

  m_peer_addr = "";
  m_peer_name = "";
  m_peer_port = 0;
}

void 
socket::reset(bool do_init)
{
  if(state() != ss_closed)
  {
    make_sync();
    close();
  }

  if(m_event)
  {
    WSACloseEvent(m_event);
  }

  if(do_init)
  {
    init();
  }
}

int 
socket::port() const
{
  return m_port;
}

String 
socket::peer_addr() const
{
  // Resolve once
  if(m_peer_addr.empty())
  {
    // Get peer info
    sockaddr_in sa;
    int len = sizeof(sa);
    if(getpeername(m_socket, (sockaddr*)&sa, &len) == SOCKET_ERROR)
    {
      SOCKET_EXCEPTION("Failed to retrieve peer name");
    }

    // Set address
    m_peer_addr = inet_ntoa(sa.sin_addr);
  }

  // Done
  return m_peer_addr;
}

String 
socket::peer_name() const
{
  // Resolve once
  if(m_peer_name.empty())
  {
    // Get peer info
    sockaddr_in sa;
    int len = sizeof(sa);
    if(getpeername(m_socket, (sockaddr*)&sa, &len) == SOCKET_ERROR)
    {
      SOCKET_EXCEPTION("Failed to retrieve peer name");
    }

    // Get address info
    char host[100];
    if(getnameinfo((sockaddr*)&sa, len, host, 99, 0, 0, 0) == SOCKET_ERROR)
    {
      // TODO nadenken hierover...
      //SOCKET_EXCEPTION("Failed to retrieve name information");
      m_peer_name = inet_ntoa(sa.sin_addr);
    }
    else
    {
      // Store host
      m_peer_name = host;
    }
  }

  // Done
  return m_peer_name;
}

int         
socket::peer_port() const
{
  // Resolve once
  if(m_peer_port == 0)
  {
    // Get peer info
    sockaddr_in sa;
    int len = sizeof(sa);
    if(getpeername(m_socket, (sockaddr*)&sa, &len) == SOCKET_ERROR)
    {
      SOCKET_EXCEPTION("Failed to retrieve peer name");
    }

    // Set port
    m_peer_port = ntohs(sa.sin_port);
  }

  // Done  
  return m_peer_port;
}

socket::sync_modes 
socket::sync_mode() const
{
  return m_async ? sm_async : sm_sync;
}

void 
socket::connect(String const& hostname, int port, sync_modes sm)
{
  CLASS_STATE(ss_closed);

  // Set new sync mode
  if(sm != sm_default)
  {
    m_async = sm == sm_async ? true : false;
  }

  // Translate port to string
  char szPort[15];
  _itoa_s(port, szPort, 10);

  // Prepare hint address info
  addrinfo ai_hint;
  memset(&ai_hint, 0, sizeof(ai_hint));
  ai_hint.ai_family   = AF_INET;
  ai_hint.ai_socktype = SOCK_STREAM;
  ai_hint.ai_protocol = IPPROTO_TCP;

  // Resolve address info
  addrinfo* ai;
  if(getaddrinfo(hostname.c_str(), szPort, &ai_hint, &ai) != 0)
  {
    SOCKET_EXCEPTION("failed to resolve hostname");
  }

  // Create socket
  m_socket = create_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if(m_socket == INVALID_SOCKET)
  {
    set_state(ss_closed);
    SOCKET_EXCEPTION("socket creation failed");
  }

  // Set up for async
  if(m_async)
  {
    make_async();
  }

  // Establish connection
  int res = ::connect(m_socket, ai->ai_addr, sizeof(sockaddr));

  // Close address info
  freeaddrinfo(ai);

  // Test result
  if(res == SOCKET_ERROR)
  {
    // WSAEWOULDBLOCK is not a failure
    if(WSAGetLastError() == WSAEWOULDBLOCK && m_async)
    {
      NOT_IMPLEMENTED;
    }
    else
    {
      SOCKET_EXCEPTION("Failed to connect");
    }
  }
  else
  {
    // Connected succesfully
    set_state(ss_connected);
    m_port = port;
  }
}

void 
socket::listen(int port, int backlog, sync_modes sm)
{
  CLASS_STATE(ss_closed);

  // Set new sync mode
  if(sm != sm_default)
  {
    m_async = sm == sm_async ? true : false;
  }

  // Create a socket
  m_socket = create_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if(m_socket == 0)
  {
    SOCKET_EXCEPTION("Failed to create socket");
  }

  // Create address info
  sockaddr_in sa;
  memset(&sa, 0, sizeof(sa));
  sa.sin_family = AF_INET;
  sa.sin_port   = htons((u_short)port);

  // Bind the socket to a port
  if(::bind(m_socket, (sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR)
  {
    SOCKET_EXCEPTION("Failed to bind socket to port");
  }

  // Find port number
  if(port == 0)
  {
    sockaddr_in sa;
    int size = sizeof(sa);
    ::getsockname(m_socket, (sockaddr*)&sa, &size);
    port = ntohs(sa.sin_port);
  }

  // Set up for async
  if(m_async)
  {
    make_async();
  }

  // Listen for incoming connection requests
  if(::listen(m_socket, backlog) == SOCKET_ERROR)
  {
    if(WSAGetLastError() == WSAEWOULDBLOCK && m_async)
    {
      NOT_IMPLEMENTED;
    }
    else
    {
      SOCKET_EXCEPTION("Failed to listen");
    }
  }

  // Set new state
  set_state(ss_listening);
  m_port = port;
}

void 
socket::accept(socket const& s, sync_modes sm)
{
  CLASS_STATE(ss_closed);

  // Set new sync mode
  if(sm != sm_default)
  {
    m_async = (sm == sm_async);
  }

  // Accept the connection
  if((m_socket = ::accept(s.m_socket, 0, 0)) == SOCKET_ERROR)
  {
    SOCKET_EXCEPTION("Failed to accept connection");
  }

  // Make sync/async
  if(m_async)
  {
    make_async();
  }
  else
  {
    make_sync();
  }

  // Set new state
  set_state(ss_connected);
}

void 
socket::make_sync()
{
  m_async = false;
  
  if(WSAEventSelect(m_socket, 0, 0) == SOCKET_ERROR)
  {
    SOCKET_EXCEPTION("Failed to unset socket event");
  }
  unsigned long ul = 0;
  if(ioctlsocket(m_socket, FIONBIO, &ul) == SOCKET_ERROR)
  {
    SOCKET_EXCEPTION("Failed to change socket to blocking mode");
  }

  if(m_event)
  {
    CloseHandle(m_event);
    m_event = 0;
  }
}

void 
socket::make_async()
{
  // Set flag
  m_async = true;

  // Create an event
  m_event = WSACreateEvent();
  if(m_event == 0)
  {
    SOCKET_EXCEPTION("Failed to create an event");
  }

  // Make the socket asynchronous
  long flags = FD_READ|FD_WRITE|FD_ACCEPT|FD_CONNECT|FD_CLOSE;
  if(WSAEventSelect(m_socket, m_event, flags) == SOCKET_ERROR)
  {
    SOCKET_EXCEPTION("Failed to set event object");
  }

  // Create a thread that listens for socket events
  m_tevent = ::CreateEvent(0, TRUE, FALSE, 0);
  create_thread(this, &socket::threadproc);

  // Wait until the thread is created
  if(::WaitForSingleObject(m_tevent, 10000) != WAIT_OBJECT_0)
  {
    SOCKET_EXCEPTION("Timeout while waiting for thread creation");
  }

  // Close the event
  ::CloseHandle(m_tevent);
  m_tevent = 0;
}

void 
socket::close(close_types cs)
{
  CLASS_STATE(ss_connected|ss_rclosed|ss_sclosed|ss_listening|ss_closed);
  
  // Determine next state and shutdown flag
  socket_states ns;
  int sd = 0;
  switch(cs)
  {
  case cs_recv: 
    ns = ss_rclosed; 
    sd = SD_RECEIVE;
    break;
  case cs_send: 
    ns = ss_sclosed;
    sd = SD_SEND;
    break;
  case cs_both:
    ns = ss_closed;  
    sd = SD_BOTH;
    break;
  default:
    throw std::exception("Unexpected enum value");
  }

  // Next state is current state
  if(ns == state())
  {
    return;
  }

  // Shutdown the socket
  if(::shutdown(m_socket, sd) == SOCKET_ERROR)
  {
    if(WSAGetLastError() == WSAEWOULDBLOCK && m_async)
    {
      NOT_IMPLEMENTED;
    }
    else if(WSAGetLastError() == WSAENOTCONN)
    {
      m_socket = 0;
      set_state(ss_closed);
      return;
    }
    else
    {
      SOCKET_EXCEPTION("Failed to shutdown socket");
    }
  }

  // The socket should be closed
  if(ns == ss_closed)
  {
    // Remove socket handle from this
    SOCKET s = m_socket;
    m_socket = 0;

    // Close the socket
    if(::closesocket(s) == SOCKET_ERROR)
    {
      if(WSAGetLastError() == WSAEWOULDBLOCK && m_async)
      {
        NOT_IMPLEMENTED;
      }
      else
      {
        SOCKET_EXCEPTION("Failed to close socket");
      }
    }
  }

  // Set next state
  set_state(ns);
}

int 
socket::avail(int timeout) const
{
  CLASS_STATE(ss_connected|ss_sclosed);

  if(timeout > 0)
  {
    timeval tv;
    tv.tv_sec  = 0;
    tv.tv_usec = timeout * 1000;

    fd_set rds;
    FD_ZERO(&rds);
    FD_SET(m_socket, &rds);

    int res;
    if((res = ::select(0, &rds, 0, 0, &tv)) == SOCKET_ERROR)
    {
      SOCKET_EXCEPTION("Failed to check socket for readability");
    }

    if(res == 0)
    {
      return 0;
    }
  }

  unsigned long s;
  if(::ioctlsocket(m_socket, FIONREAD, &s) == SOCKET_ERROR)
  {
    SOCKET_EXCEPTION("Failed to determine available bytes");
  }
  return s;
}

void 
socket::send(String const& buf, int timeout)
{
  send((void*)buf.c_str(), buf.length(), timeout);
}

void 
socket::send(char const* pbuf, int size, int timeout)
{
  CLASS_STATE(ss_connected|ss_rclosed);

  if(size == 0)
  {
    size = (int) strlen(pbuf);
  }
  send((void*)pbuf, size, timeout);
}

void 
socket::send(void* pbuf, int size, int /*timeout*/)
{
  CLASS_STATE(ss_connected|ss_rclosed);

  int s;
  if((s = ::send(m_socket, (char const*)pbuf, size, 0)) == SOCKET_ERROR)
  {
    if(WSAGetLastError() == WSAEWOULDBLOCK && m_async)
    {
      NOT_IMPLEMENTED;
    }
    else
    {      
      SOCKET_EXCEPTION("Failed to write to socket");
    }
  }
  else if(s != size)
  {
    SOCKET_EXCEPTION("Failed to write requested size");
  }
}

int 
socket::recv(void* pbuf, int size)
{
  CLASS_STATE(ss_connected|ss_sclosed);

  int s;
  if((s = ::recv(m_socket, (char*)pbuf, size, 0)) == SOCKET_ERROR)
  {
    if(WSAGetLastError() == WSAEWOULDBLOCK && m_async)
    {
      NOT_IMPLEMENTED;
    }
    else
    {
      SOCKET_EXCEPTION("Failed to read from socket");
    }
  }
  if(s != size)
  {
    SOCKET_EXCEPTION("Failed to read requested size");
  }
  return s;
}

#define WSA_EVENT(evt,handler)  \
if((wne.lNetworkEvents & evt) == evt) \
{ \
  if(wne.iErrorCode[evt##_BIT] != 0)  \
  { \
    NOT_IMPLEMENTED;  \
  } \
  handler();  \
}

void 
socket::threadproc()
{
  // Signal main thread
  ::SetEvent(m_tevent);

  // Main thread loop
  for(;;)
  {
    // Wait for events on socket
    if(WaitForSingleObject(m_event, INFINITE) != WAIT_OBJECT_0)
    {
      NOT_IMPLEMENTED;
    }

    // If m_socket is zero, the socket was closed locally
    if(m_socket == 0)
    {
      return;
    }

    // Enumerate events
    WSANETWORKEVENTS wne;
    if(WSAEnumNetworkEvents(m_socket, m_event, &wne) == SOCKET_ERROR)
    {
      switch(WSAGetLastError())
      {
      case WSAENOTSOCK:
        m_socket = 0;
        set_state(ss_closed);
        return;
      default:
        NOT_IMPLEMENTED;
      }
    }

    // Dispatch events
    WSA_EVENT(FD_CONNECT, on_connect);
    WSA_EVENT(FD_CLOSE,   on_disconnect);
    WSA_EVENT(FD_ACCEPT,  on_accept);

    // Not implemented
    //WSA_EVENT(FD_READ,    on_read);
    //WSA_EVENT(FD_WRITE,   on_write);
    //WSA_EVENT(FD_OOB,     on_oob);
    //WSA_EVENT(FD_QOS,     on_qos);

    // Handle close
    if(wne.lNetworkEvents & FD_CLOSE)
    {
      if(closesocket(m_socket) == SOCKET_ERROR)
      {
        NOT_IMPLEMENTED;
      }
      m_socket = 0;
      set_state(ss_closed);
      return;
    }

  } // for(;;)
}

void 
socket::impl_on_connect()
{
  on_connect();
}

void 
socket::impl_on_disconnect()
{
  on_disconnect();
  m_socket = 0;
  set_state(ss_closed);
}

void 
socket::impl_on_accept()
{
  on_accept();
}

/////////////////////////////////////////////////////////////////////////////////////
// Exceptions

socket::socket_exception::socket_exception(String const& what, int error) :
exception(format_error(what, error))
{
}

/*static*/ String 
socket::socket_exception::format_error(String const& what, int error)
{
  char buff[1000];
  sprintf_s(buff, "Socket error: %s (%d)", what.c_str(), error);
  return buff;
}

