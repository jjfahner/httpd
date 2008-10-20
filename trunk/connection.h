#ifndef CONNECTION_H
#define CONNECTION_H

#include <sstream>

#include "listener.h"
#include "threadpool.h"

class connection : public socket, public threadpool::task
{
public:

  //
  // Construct from listener
  //
  connection(listener*);

  //
  // Destruction
  //
  virtual ~connection();

  //
  // Retrieve listener
  //
  listener* get_listener() const;

  //
  // Retrieve a line from the buffer
  //
  bool read_line(String& line, bool peek = false);

  //
  // Retrieve a line from the buffer
  //
  bool read_line(String& line, int size);

  //
  // Retrieve a specific number of bytes
  //
  bool read_bytes(char* buf, int size);

  //
  // Log a string
  //
  void log(String const&);

private:

  //
  // Main loop
  //
  void handle_request();

  //
  // Task handler
  //
  virtual void perform();

  //
  // Timeout handler
  //
  void timeout();

  //
  // Resize the read buffer
  //
  void resize_buffer(int newlen);

  //
  // Append available data to buffer
  //
  bool read_avail();

  //
  // Members
  //
  listener*         m_listener;
  char*             m_buffer;
  int               m_buflen;
  int               m_bufuse;
  int               m_bufpos;
  std::stringstream m_log;

  //
  // Not allowed
  //
  connection(connection const&);
  connection const& operator = (connection const&);

};


#endif  // #ifndef CONNECTION_H
