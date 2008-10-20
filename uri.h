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
  uri(std::string const& src);

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
  std::string const& protocol() const;
  std::string const& host() const;
  int                port() const;
  std::string const& path() const;

  //
  // Try to set from string
  //
  bool parse(std::string const&);

  //
  // Set from string or throw exception
  //
  void set(std::string const&);

  //
  // Retrieve as string
  //
  std::string str() const;

  // Behave as string
  operator std::string() const;

private:

  std::string   m_prot;
  std::string   m_host;
  int           m_port;
  std::string   m_path;

};

class uri::invalid_uri : public std::exception
{
  static std::string format_error(std::string const& src)
  {
    std::string text("Invalid uri: ");
    text += src;
    return text;
  }
public:
  invalid_uri(std::string const& src) : 
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
uri::uri(std::string const& str)
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

inline std::string const& 
uri::protocol() const
{
  return m_prot;
}

inline std::string const& 
uri::host() const
{
  return m_host;
}

inline int
uri::port() const
{
  return m_port;
}

inline std::string const& 
uri::path() const
{
  return m_path;
}

inline void
uri::set(std::string const& src)
{
  if(!parse(src))
  {
    throw invalid_uri(src);
  }
}

inline 
uri::operator std::string() const
{
  return str();
}

#endif  // #ifndef URI_H
