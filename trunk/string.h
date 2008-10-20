#ifndef HTTPD_STRING_H
#define HTTPD_STRING_H

//
// Implement string as wrapper around String
//
class String : public std::string
{
public:

  //
  // Base class
  //
  typedef std::string Base;

  //
  // Empty construction
  //
  String()
  {
  }

  //
  // Construction from pointer
  //
  String(char const* str) : Base(str)
  {
  }

  //
  // Construction from wide char
  //
  String(wchar_t const* str);

  //
  // Construction from iterator range
  //
  String(Base::const_iterator const& it, Base::const_iterator const& ie) : Base(it, ie)
  {
  }

  //
  // Copy construction
  //
  String(Base const& base) : Base(base)
  {
  }

  //
  // Allocate a system string
  //
  wchar_t* AllocSysString() const;

};

#endif // HTTPD_STRING_H
