#ifndef HTTPD_STRING_H
#define HTTPD_STRING_H

//
// Implement string as wrapper around std::string
//
class String : public std::string
{
public:

  //
  // Base class
  //
  typedef std::string Base;

  //
  // Construction from pointer
  //
  String(char const* str) : Base(str)
  {
  }

  //
  // Construction from wide char
  //
  String(wchar_t const* str)
  {
    // TODO, probably best in source file
  }

  //
  // Copy construction
  //
  String(Base const& base) : Base(base)
  {
  }

};

#endif // HTTPD_STRING_H
