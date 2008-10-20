#ifndef HTTPDATE_H
#define HTTPDATE_H

class httpdate
{
public:

  //
  // Construction
  //
  httpdate();
  httpdate(String const& src);
  httpdate(FILETIME const& ft);
  httpdate(SYSTEMTIME const& st);

  //
  // Set time
  //
  bool set(String const&);
  bool set(SYSTEMTIME const& st);
  void set(FILETIME const& ft);

  //
  // Set to current date
  //
  void set_now();

  //
  // Get string representation
  //
  String str() const;

  //
  // Implicit conversion to string
  //
  operator String () const;

  //
  // Add seconds to date
  //
  httpdate& add_seconds(int seconds);

  //
  // Retrieve the time as FILETIME thingy in different thingy
  //
  LARGE_INTEGER const& time() const;

private:

  //
  // Members
  //
  LARGE_INTEGER m_time;
};

inline LARGE_INTEGER const& 
httpdate::time() const
{
  return m_time;
}

inline 
httpdate::operator String () const
{
  return str();
}

inline bool 
operator < (httpdate const& lhs, httpdate const& rhs)
{
  return lhs.time().QuadPart < rhs.time().QuadPart;
}

inline bool 
operator == (httpdate const& lhs, httpdate const& rhs)
{
  return !(lhs < rhs || rhs < lhs);
}

inline bool 
operator != (httpdate const& lhs, httpdate const& rhs)
{
  return (lhs < rhs) || (rhs < lhs);
}

inline bool 
operator <= (httpdate const& lhs, httpdate const& rhs)
{
  return lhs < rhs || !(rhs < lhs);
}

inline bool 
operator > (httpdate const& lhs, httpdate const& rhs)
{
  return rhs < lhs;
}

inline bool 
operator >= (httpdate const& lhs, httpdate const& rhs)
{
  return rhs < lhs || !(lhs < rhs);
}

#endif  // #ifndef HTTPDATE_H
