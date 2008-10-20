#include "precomp.h"
#include "threadpool.h"
#include "thread.h"

#define WAIT_OBJECT_1 (WAIT_OBJECT_0+1)
#define WAIT_OBJECT_2 (WAIT_OBJECT_0+2)
#define WAIT_OBJECT_3 (WAIT_OBJECT_0+3)
#define WAIT_OBJECT_4 (WAIT_OBJECT_0+4)
#define WAIT_OBJECT_5 (WAIT_OBJECT_0+5)

//////////////////////////////////////////////////////////////////////////
//
// threadpool::task implementation

threadpool::task::task() :
m_queue (0),
m_state (ts_none)
{
}

threadpool::task::~task()
{
}

threadpool::taskstates 
threadpool::task::state() const
{
  return m_state;
}

void 
threadpool::task::destroy()
{
  switch(m_state)
  {
  case ts_queued:
    // TODO: unqueue
    // -= Fallthrough =-

  case ts_none:
  case ts_error:
  case ts_timeout:
  case ts_complete:
    delete this;
    return;

  case ts_running:
    throw exception("cannot destroy a running task");

  default:
    throw exception("unexpected: invalid state in threadpool::task::destroy");
  }
}

void 
threadpool::task::set_state(taskstates state)
{
  m_state = state;
}

//////////////////////////////////////////////////////////////////////////
//
// threadpool implementation

threadpool::threadpool(size_t min, size_t max) :
m_task_available  (true),   // Autoreset
m_pool_terminate  (false),  // No autoreset
m_threads_min (min + 1),
m_threads_max (max + 1)
{
}

threadpool::~threadpool()
{
  stop();
}

void 
threadpool::set_threadcount(size_t min, size_t max)
{
  if(m_threads.size())
  {
    throw exception("Cannot set threadcount on running threadpool");
  }
  if(min < 1 || max < min)
  {
    throw exception("Invalid threadcount specified");
  }
  m_threads_min = min + 1;
  m_threads_max = max + 1;
}

void 
threadpool::start()
{
  for(size_t n = 0; n < m_threads_min; ++n)
  {
    CloseHandle(create_thread(this, &threadpool::threadproc));
  }
  queue(new timeout_task(*this, false));
}

void
threadpool::stop()
{
  m_pool_terminate.set();
  while(m_threads.size() > 0)
  {
    Sleep(100);
  }
}

void 
threadpool::queue(task* t, bool destroy, timeout_t timeout)
{
  // Create the queue item
  queue_record qr;
  qr.m_task = t;
  qr.m_destroy = destroy;
  qr.m_timeout = timeout;

  // Append to back of queue
  {
    critical_section::lock csl(m_cs_queue);
    m_queue.push_back(qr);
    qr.m_queued = GetTickCount();
  }

  // Make sure the task gets picked up
  // Event is autoreset, so this releases 1 thread
  m_task_available.set();
}

void 
threadpool::timeout(task_queue& tasks)
{
  // Enumerate timed-out tasks
  task_queue::iterator it = tasks.begin();
  task_queue::iterator ie = tasks.end();
  for(; it != ie; ++it)
  {
    // Notify task of timeout
    try
    {
      it->m_task->timeout();
    }
    catch(...)
    {
    }

    // Delete if required
    if(it->m_destroy)
    {
      try
      {
        delete it->m_task;
      }
      catch(...)
      {
      }
    }
  }
}

void 
threadpool::threadproc()
{
  // Create informational structure
  thread_record this_thread;
  this_thread.m_handle = GetCurrentThread();

  // Begin scope for the lock
  {
    // Add ourselves to list of threads
    critical_section::lock csl(m_cs_threads);
    m_threads.push_front(&this_thread);
  }

  // Run thread loop
  for(;;)
  {
    // Create array of handles
    HANDLE handles[2] = 
    {
      m_task_available,
      m_pool_terminate
    };

    // Wait until we're signalled
    DWORD waitres = WaitForMultipleObjects(
              2, handles, FALSE, INFINITE);

    // A task is available
    if(waitres == WAIT_OBJECT_0)
    {
      // Run while there are tasks available
      for(;;)
      {
        // Retrieve task
        queue_record qr;
        {
          critical_section::lock csl(m_cs_queue);
          if(m_queue.size())
          {
            qr = m_queue.front();
            m_queue.pop_front();
          }
          else
          {
            break;
          }
        }

        // Handle task
        try
        {
          qr.m_task->set_state(ts_running);
          qr.m_task->perform();
          qr.m_task->set_state(ts_complete);
        }
        catch(...)
        {
          try
          {
            qr.m_task->set_state(ts_error);
            qr.m_task->error();
          }
          catch(...)
          {
          }
        }

        // Delete task
        if(qr.m_destroy)
        {
          try
          {
            qr.m_task->destroy();
          }
          catch(...)
          {
          }
        }

        // Check whether we're stopping
        if(WaitForSingleObject(m_pool_terminate, 0) == WAIT_OBJECT_0)
        {
          break;
        }

      } // for(;;)

      // Next iteration
      continue;

    } // if(waitres == WAIT_OBJECT_0)
    

    // We're terminating
    if(waitres == WAIT_OBJECT_1)
    {
      // Exit main threadloop
      break;
    }

    // An unexpected return code from WaitForSingleObject
    OutputDebugString("Unexpected failure in thread\n");
    return;

  } // for(;;)


  // Lock the thread list
  critical_section::lock csl(m_cs_threads);

  // Remove from thread list
  thread_list::iterator it;
  if((it = std::find(m_threads.begin(), m_threads.end(), 
    &this_thread)) != m_threads.end())
  {
    m_threads.erase(it);
  }
}



threadpool::timeout_task::timeout_task(threadpool& pool, bool monitor) :
m_pool    (pool),
m_monitor (monitor)
{
}

void 
threadpool::timeout_task::perform()
{
  for(;;)
  {
    // Wait until we're signalled or timeout
    DWORD waitres = WaitForSingleObject(m_pool.m_pool_terminate, 5000);

    // Check whether to abort
    if(waitres == WAIT_OBJECT_0)
    {
      return;
    }

    threadpool::task_queue tq;
    threadpool::task_queue::iterator it;

    // Remove tasks that have timed out from the queue
    {
      critical_section::lock csl(m_pool.m_cs_queue);
      it = m_pool.m_queue.begin();
      {
        while(it != m_pool.m_queue.end())
        {
          if(it->m_timeout != 0 && GetTickCount() - it->m_queued > it->m_timeout)
          {
            tq.push_back(*it);
            it = m_pool.m_queue.erase(it);
          }
          else
          {
            ++it;
          }
        }
      }
    }

    // Delegate to threadpool
    m_pool.timeout(tq);
  }
}

void 
threadpool::timeout_task::timeout()
{
}

void 
threadpool::timeout_task::error()
{
}

