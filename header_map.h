#ifndef HEADER_MAP_H
#define HEADER_MAP_H

#include "stringmap.h"

/////////////////////////////////////////////////////////////////////////////////////
//
// header_map declaration

class header_map : public stringmap
{
public:

  //
  // Local types
  //
  typedef stringmap base_type;
  typedef header_map this_type;
  typedef std::list<key_type> stringlist;
  typedef stringlist::const_iterator const_inorder_iterator;

  //
  // Construction
  //
  header_map();

  //
  // Finalize the map so no alterations can be made
  //
  void finalize();

  //
  // Is the map finalized
  //
  bool finalized() const;

  //
  // Clear the map
  //
  void clear();

  //
  // Remove a pair
  //
  void remove(key_type const& key);

  //
  // Set a value from key, value
  //
  void set(key_type const& key, value_type const& value);

  //
  // Set a value from "name: value"
  //
  void set(std::string const& line);

  //
  // Append a value to key
  //
  void append(key_type const& key, value_type const& value);

  //
  // Append a value from "name: value"
  //
  void append(std::string const& line);

  //
  // Build a string containing all pairs
  //
  std::string str(std::string const& separator = "\r\n") const;

  //
  // Indexing operator
  //
  class const_index_proxy;
  const_index_proxy operator [] (key_type const& key) const;
  class index_proxy;
  index_proxy operator [] (key_type const& key);

  //
  // Enumeration
  //
  const_inorder_iterator inorder_begin() const;
  const_inorder_iterator inorder_end() const;

private:

  //
  // Operations that aren't allowed
  //
  header_map(header_map const&);
  header_map const& operator = (header_map const&);

  //
  // Verify that the map hasn't been finalized
  //
  void check_not_final();

  //
  // Member data
  //
  bool        m_final;
  stringlist  m_order;

};  // class header_map...


/////////////////////////////////////////////////////////////////////////////////////
//
// Const index proxy 
//

class header_map::const_index_proxy : public header_map::base_type::const_index_proxy
{
public:
  const_index_proxy(header_map const& map, key_type const& key) :
  base_type::const_index_proxy(map, key)
  {
  }
  const_index_proxy(const_index_proxy const& rhs) :
  base_type::const_index_proxy(rhs)
  {
  };
};

inline bool operator == (header_map::const_index_proxy const& lhs, std::string const& rhs)
{ return _stricmp(lhs.as_str().c_str(), rhs.c_str()) == 0; }
inline bool operator == (header_map::const_index_proxy const& lhs, char const* rhs)
{ return _stricmp(lhs.as_str().c_str(), rhs) == 0; }

inline bool operator != (header_map::const_index_proxy const& lhs, std::string const& rhs)
{ return _stricmp(lhs.as_str().c_str(), rhs.c_str()) != 0; }
inline bool operator != (header_map::const_index_proxy const& lhs, char const* rhs)
{ return _stricmp(lhs.as_str().c_str(), rhs) != 0; }

inline bool operator == (std::string const& lhs, header_map::const_index_proxy const& rhs)
{ return _stricmp(lhs.c_str(), rhs.as_str().c_str()) == 0; }
inline bool operator == (char const* lhs, header_map::const_index_proxy const& rhs)
{ return _stricmp(lhs, rhs.as_str().c_str()) == 0; }

inline bool operator != (std::string const& lhs, header_map::const_index_proxy const& rhs)
{ return _stricmp(lhs.c_str(), rhs.as_str().c_str()) != 0; }
inline bool operator != (char const* lhs, header_map::const_index_proxy const& rhs)
{ return _stricmp(lhs, rhs.as_str().c_str()) != 0; }

/////////////////////////////////////////////////////////////////////////////////////
//
// Index proxy 
//
class header_map::index_proxy : public header_map::base_type::index_proxy
{
public:
  index_proxy(header_map& map, key_type const& key) :
  base_type::index_proxy(map, key)
  {
  }
  index_proxy(index_proxy const& rhs) :
  base_type::index_proxy(rhs)
  {
  }
  value_type const& operator = (value_type const& value)
  {
    static_cast<header_map&>(ncmap()).set(m_key, value);
    return value;
  }
  char const* operator = (char const* value)
  {
    *this = value_type(value);
    return value;
  }
  int operator = (int value)
  {
    char buff[15];
    _itoa_s(value, buff, 10);
    *this = value_type(buff);
    return value;
  }
  bool operator = (bool value)
  {
    *this = value ? 1 : 0;
    return value;
  }
};

inline bool operator == (header_map::index_proxy const& lhs, std::string const& rhs)
{ return _stricmp(lhs.as_str().c_str(), rhs.c_str()) == 0; }
inline bool operator == (header_map::index_proxy const& lhs, char const* rhs)
{ return _stricmp(lhs.as_str().c_str(), rhs) == 0; }

inline bool operator != (header_map::index_proxy const& lhs, std::string const& rhs)
{ return _stricmp(lhs.as_str().c_str(), rhs.c_str()) != 0; }
inline bool operator != (header_map::index_proxy const& lhs, char const* rhs)
{ return _stricmp(lhs.as_str().c_str(), rhs) != 0; }

inline bool operator == (std::string const& lhs, header_map::index_proxy const& rhs)
{ return _stricmp(lhs.c_str(), rhs.as_str().c_str()) == 0; }
inline bool operator == (char const* lhs, header_map::index_proxy const& rhs)
{ return _stricmp(lhs, rhs.as_str().c_str()) == 0; }

inline bool operator != (std::string const& lhs, header_map::index_proxy const& rhs)
{ return _stricmp(lhs.c_str(), rhs.as_str().c_str()) != 0; }
inline bool operator != (char const* lhs, header_map::index_proxy const& rhs)
{ return _stricmp(lhs, rhs.as_str().c_str()) != 0; }

/////////////////////////////////////////////////////////////////////////////////////
//
// header_map implementation

inline 
header_map::header_map() :
m_final (false)
{
}

inline void 
header_map::finalize()
{
  m_final = true;
}

inline bool 
header_map::finalized() const
{
  return m_final;
}

inline void 
header_map::check_not_final()
{
  if(m_final)
  {
    throw std::exception("Attempt to modify a finalized header_map");
  }
}

inline void 
header_map::clear()
{
  check_not_final();
  base_type::clear();
  m_order.clear();
}

inline void 
header_map::remove(key_type const& key)
{
  check_not_final();
  base_type::remove(key);
  m_order.remove(key);
}

inline void 
header_map::append(key_type const& key, value_type const& value)
{
  check_not_final();

  base_type::iterator it = base_type::find(key);
  if(it == base_type::end())
  {
    base_type::set(key, value);
    m_order.push_back(key);
  }
  else
  {
    it->second += ",";
    it->second += value;
  }
}

inline void 
header_map::append(std::string const& line)
{
  std::string::size_type pos = line.find(':');
  if(pos == 0 || pos == std::string::npos)
  {
    throw std::exception("Invalid header line");
  }
  append(line.substr(0, pos), line.substr(
    line.find_first_not_of(' ', pos + 1)));
}

inline void 
header_map::set(key_type const& key, value_type const& value)
{
  check_not_final();

  base_type::iterator it = base_type::find(key);
  if(it == base_type::end())
  {
    base_type::set(key, value);
    m_order.push_back(key);
  }
  else
  {
    it->second = value;
  }
}

inline void 
header_map::set(std::string const& line)
{
  std::string::size_type pos = line.find(':');
  if(pos == 0 || pos == std::string::npos)
  {
    throw std::exception("Invalid header line");
  }
  set(line.substr(0, pos), line.substr(
    line.find_first_not_of(' ', pos + 1)));
}

inline header_map::value_type 
header_map::str(std::string const& separator) const
{ 
  value_type headers;
  header_map::const_inorder_iterator it, ie;
  it = inorder_begin();
  ie = inorder_end();
  for(; it != ie; ++it)
  {
    headers += *it;
    headers += ": ";
    headers += (*this)[*it];
    headers += separator;
  }
  return headers;
}

inline header_map::const_index_proxy 
header_map::operator [] (key_type const& key) const
{
  return const_index_proxy(*this, key);
}

inline header_map::index_proxy 
header_map::operator [] (key_type const& key)
{
  return index_proxy(*this, key);
}

inline header_map::const_inorder_iterator 
header_map::inorder_begin() const
{
  return m_order.begin();
}

inline header_map::const_inorder_iterator 
header_map::inorder_end() const
{
  return m_order.end();
}

#endif  // #ifndef HEADERS_H
