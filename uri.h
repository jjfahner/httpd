#ifndef URI_H
#define URI_H

//
// Object representation of Uniform Resource Identifier
//
class uri
{
public:

  // Exception
  class invalid_uri;
  
  //
  // Construction
  //
  uri();
  uri(String const& src);

  //
  // Clear the uri
  //
  void clear();

  //
  // Is the uri empty
  //
  bool empty() const;

  //
  // Is the uri valid
  // 
  bool valid() const;

  // 
  // Individual components
  //
  String const& protocol() const;
  String const& host() const;
  int                port() const;
  String const& path() const;

  //
  // Try to set from string
  //
  bool parse(String const&);

  //
  // Set from string or throw exception
  //
  void set(String const&);

  //
  // Retrieve as string
  //
  String str() const;

  // Behave as string
  operator String() const;

private:

  String   m_prot;
  String   m_host;
  int           m_port;
  String   m_path;

};

class uri::invalid_uri : public std::exception
{
  static String format_error(String const& src)
  {
    String text("Invalid uri: ");
    text += src;
    return text;
  }
public:
  invalid_uri(String const& src) : 
    std::exception(format_error(src).c_str()) 
  {
  }
};

inline
uri::uri()
{
  clear();
}

inline
uri::uri(String const& str)
{
  set(str);
}

inline void 
uri::clear()
{
  m_prot.clear();
  m_host.clear();
  m_port = 0;
  m_path.empty();
}

inline bool
uri::empty() const
{
  return  m_prot.empty() &&
          m_host.empty() &&
          m_port == 0    &&
          m_path.empty() ;
}

inline bool
uri::valid() const
{
  return !empty();
}

inline String const& 
uri::protocol() const
{
  return m_prot;
}

inline String const& 
uri::host() const
{
  return m_host;
}

inline int
uri::port() const
{
  return m_port;
}

inline String const& 
uri::path() const
{
  return m_path;
}

inline void
uri::set(String const& src)
{
  if(!parse(src))
  {
    throw invalid_uri(src);
  }
}

inline 
uri::operator String() const
{
  return str();
}

#endif  // #ifndef URI_H
