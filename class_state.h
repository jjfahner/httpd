#ifndef CLASS_STATE_H
#define CLASS_STATE_H

#include "sync.h"

template <typename T>
class class_state
{
public:

  T state() const
  {
    return m_state;
  }

protected:

  class_state(T t = T()) :
  m_state(t)
  {
  }

  void set_state(T new_state)
  {
    m_state = new_state;
  }

  void check_state(unsigned states, char const* err) const
  {
    if(states && !(m_state & states))
    {
      throw std::exception(err);
    }
  }

  critical_section m_state_cs;

private:

  T m_state;

};

#define CLASS_STATE(expected_state)         \
  critical_section::lock _csl(m_state_cs);  \
  check_state(expected_state, "class_state: invalid state in " __FUNCTION__)
  
#endif  // #ifndef CLASS_STATE_H
