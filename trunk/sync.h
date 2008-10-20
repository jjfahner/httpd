#ifndef SYNC_H
#define SYNC_H

//
// critical_section provides a synchronization mechanism.
// Usage is exclusively through the lock member class.
//
class critical_section
{
public:

  //
  // Auto-lock class
  //
  class lock
  {
    critical_section const& m_cs;
  public:
    lock(critical_section const& cs) : m_cs (cs) 
    {
      m_cs.acquire();
    }
    ~lock() 
    {
      m_cs.release();
    }
  private:
    lock(lock const&);
    lock const& operator = (lock const&);
  };

  //
  // Construction
  //
   critical_section();
  ~critical_section();

private:

  //
  // Lock implementation
  //
  void acquire() const;
  void release() const;

  //
  // The critical section
  //
  CRITICAL_SECTION m_cs;

};

inline 
critical_section::critical_section()
{
  InitializeCriticalSection(&m_cs);
}

inline 
critical_section::~critical_section()
{
  DeleteCriticalSection(&m_cs);
}

inline void 
critical_section::acquire() const
{
  EnterCriticalSection(&const_cast<CRITICAL_SECTION&>(m_cs));
}

inline void 
critical_section::release() const
{
  LeaveCriticalSection(&const_cast<CRITICAL_SECTION&>(m_cs));
}

/////////////////////////////////////////////////////////////////////////////////////
// 
// Event class

class event 
{
  HANDLE  m_handle;
public:
  
  event(bool autoreset)
  {
    m_handle = CreateEvent(0, autoreset ? FALSE : TRUE, FALSE, 0);
  }

  ~event()
  {
    CloseHandle(m_handle);
  }

  void pulse()
  {
    PulseEvent(m_handle);
  }

  void set()
  {
    SetEvent(m_handle);
  }

  void reset()
  {
    ResetEvent(m_handle);
  }

  operator HANDLE() const
  {
    return m_handle;
  }

  event& operator = (event const& rhs)
  {
    if(this != &rhs)
    {
      CloseHandle(m_handle);
      DuplicateHandle(GetCurrentProcess(), rhs.m_handle, 
                      GetCurrentProcess(), &m_handle, 
                      0, FALSE, DUPLICATE_SAME_ACCESS);
    }
    return *this;
  }

};





#endif  // #ifndef SYNC_H
