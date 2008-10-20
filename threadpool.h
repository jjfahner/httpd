#ifndef THREADPOOL_H
#define THREADPOOL_H

#include "sync.h"
#include <deque>
#include <list>

class threadpool 
{
public:

  typedef unsigned long timeout_t;

  //
  // The states a task passes through
  //
  enum taskstates 
  {
    ts_none,
    ts_queued,
    ts_running,
    ts_error,
    ts_timeout,
    ts_complete,
  };

  //
  // A task is the executable unit for the threadpool
  // Defined in this file following class threadpool.
  //
  class task;

  //
  // Exception classes
  //
  class exception : public std::exception 
  {
  public:
    exception(String const& what) : std::exception(what.c_str()) {}
  };


public:

  //
  // Construction
  //
   threadpool(size_t min = 10, size_t max = 10);
  ~threadpool();

  //
  // Set thread count. Only valid when stopped.
  //
  void set_threadcount(size_t min, size_t max);

  //
  // Queue a task. If destroy is set, the task will be destroyed
  // after it has been completed (either succesfully or in error).
  // 
  //
  void queue(task* t, bool destroy = true, timeout_t timeout = 0);

  //
  // Block queue. When the queue is blocked, attempts to queue a
  // task will cause an exception to be thrown. 
  //
  void block();
  void unblock();

  //
  // Queue state
  //
  void start();
  void stop();

  
private:

  //
  // Queue entry
  //
  struct queue_record
  {
    task*         m_task;
    bool          m_destroy;
    timeout_t     m_timeout;
    timeout_t     m_queued;
  };

  //
  // Task queue
  //
  typedef std::deque<queue_record> task_queue;
  task_queue        m_queue;
  critical_section  m_cs_queue;

  //
  // Timeout handler class
  //
  class timeout_task;

  //
  // One or more tasks have timed out  
  //
  void timeout(task_queue& tasks);

  //
  // Thread instance
  //
  struct thread_record
  {
    HANDLE  m_handle;
  };

  // Thread list
  typedef std::list<thread_record*> thread_list;

  // The threads
  thread_list       m_threads;
  critical_section  m_cs_threads;

  // Allowed number of threads
  size_t m_threads_min;
  size_t m_threads_max;

  // The event that is pulsed when a task is available
  event m_task_available;
  event m_pool_terminate;

  // Thread proc
  void threadproc();
};

//
// A task is the executable unit for the threadpool
//
class threadpool::task
{
protected:

  //
  // Construction
  //
  task();

public:

  //
  // Task information
  //
  taskstates state() const;

  //
  // Destroy the task. This will fail while the task is
  // running.
  //
  void destroy();

protected:

  // 
  // Destruction is virtual and protected. This means
  // that the only way to destroy a task is through
  // the task::destroy() method or from a derived type.
  //
  virtual ~task();

private:

  //
  // Override to perform task
  //
  virtual void perform() = 0;

  //
  // Override to handle timeout
  //
  virtual void timeout() {}

  //
  // Override to handle error
  //
  virtual void error() {}

  //
  // Used by threadpool to set status
  //
  friend class threadpool;
  void set_state(taskstates state);

  //
  // Member data
  //
private:
  threadpool* m_queue;
  taskstates  m_state;

};

//
// Timeout task. This is the thread that notifies timeouts
//
class threadpool::timeout_task : public threadpool::task
{
public:

  // Construction
  timeout_task(threadpool& pool, bool monitor = false);

  //
  // Timeout implementation
  //
  virtual void perform();

  //
  // Timeout handler
  //
  virtual void timeout();

  //
  // Error handler
  //
  virtual void error();

private:

  //
  // Not allowed
  //
  timeout_task const& operator = (timeout_task const&);

  //
  // Members
  //
  threadpool&   m_pool;
  bool          m_monitor;

};

#endif
