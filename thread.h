#ifndef THREAD_H
#define THREAD_H

template <typename T>
class thread_t
{
public:

  typedef void (T::*F)();

  HANDLE start(T* t, F f)
  {
    m_inst = t;
    m_proc = f;
    return (HANDLE)_beginthreadex(0, 0, threadproc, this, 0, 0);
  }

private:

  static unsigned __stdcall threadproc(void* pvoid)
  {
    T* inst = ((thread_t*)pvoid)->m_inst;
    F  proc = ((thread_t*)pvoid)->m_proc;    
    delete ((thread_t*)pvoid);

    try
    {
      (inst->*proc)();
      return EXIT_SUCCESS;
    }
    catch(...)
    {
      return EXIT_FAILURE;
    }
  }

  T* m_inst;
  F  m_proc;

};


template <typename T>
HANDLE create_thread(T* t, void (T::*p)())
{
  thread_t<T>* inst = new thread_t<T>;
  return inst->start(t, p);
}

#endif  // #ifndef THREAD_H
