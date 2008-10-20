#ifndef STRINGMAP_H
#define STRINGMAP_H

#include "misc.h"
#include <algorithm>

/////////////////////////////////////////////////////////////////////////////////////
//
// stringmap declaration

class stringmap : protected std::map<String, String, httpd_nocase_cmp >
{
public:

  //
  // Local types
  //
  typedef std::map<String, String, httpd_nocase_cmp > base_type;
  typedef stringmap this_type;
  typedef String key_type;
  typedef String value_type;

  //
  // Members used directly from base.
  //
  using base_type::const_iterator;
  using base_type::const_reverse_iterator;
  using base_type::clear;
  using base_type::empty;
  using base_type::size;

public:

  //
  // Determine the existence of a key
  //
  bool contains(key_type const& key) const;

  //
  // Remove a pair
  //
  void remove(key_type const& key);

  //
  // Set a value from key, value
  //
  void set(key_type const& key, value_type const& value);

  //
  // Match key value ignoring case
  //
  bool match_nocase(key_type const& key, value_type const& value) const;

  //
  // Indexing
  //
  class const_index_proxy;
  class index_proxy;
  const_index_proxy operator [] (key_type const& key) const;
  index_proxy operator [] (key_type const& key);

  //
  // Iteration
  //
  const_iterator begin() const;
  const_iterator end() const;
  const_reverse_iterator rbegin() const;
  const_reverse_iterator rend() const;

};  // class stringmap...


/////////////////////////////////////////////////////////////////////////////////////
//
// stringmap proxy declarations

class stringmap::const_index_proxy
{
public:
  // Construction
  const_index_proxy(this_type const& map, key_type const& key);
  const_index_proxy(const_index_proxy const& rhs);

  // Explicit type conversions
  value_type const& as_str() const;
  int as_int() const;
  bool as_bln() const;

  // Implicit type conversions
  operator value_type const& () const;
  operator int () const;
  operator bool () const;

  // Subvalue indexing features
  size_t count() const;
  value_type operator [] (size_t index) const;

  // Comparisons
  bool operator == (value_type const& rhs) const;
  bool operator == (char const* rhs) const;
  bool operator == (int rhs) const;
  bool operator == (bool rhs) const;
  bool operator != (value_type const& rhs) const;
  bool operator != (char const* rhs) const;
  bool operator != (int rhs) const;
  bool operator != (bool rhs) const;

  // Used by index_proxy
protected:
  this_type const&  m_map;
  key_type          m_key;

  // Not allowed
private:
  const_index_proxy const& operator = (const_index_proxy const&);
};

class stringmap::index_proxy : public stringmap::const_index_proxy
{
protected:
  inline this_type& ncmap();
public:
  // Construction
  index_proxy(this_type& map, key_type const& key);
  index_proxy(index_proxy const& rhs);

  // Assignment
  value_type const& operator = (value_type const& value);
  char const* operator = (char const* value);
  int operator = (int value);
  bool operator = (bool value);

  // Subvalue addition
  value_type const& operator += (value_type const& value);

private:
  index_proxy const& operator = (index_proxy const&);
};


/////////////////////////////////////////////////////////////////////////////////////
//
// stringmap implementation

inline bool 
stringmap::contains(key_type const& key) const
{
  return find(key) != end();
}

inline void 
stringmap::remove(key_type const& key)
{
  base_type::iterator it = find(key);
  if(it != end())
  {
    erase(it);
  }
}

inline void 
stringmap::set(key_type const& key, value_type const& value)
{
  if(value.empty())
  {
    remove(key);
  }
  else
  {
    base_type::iterator it = find(key);
    if(it == base_type::end())
    {
      insert(std::make_pair(key, value));
    }
    else
    {
      it->second = value;
    }
  }
}

inline bool 
stringmap::match_nocase(key_type const& key, value_type const& value) const
{
  const_iterator it = find(key);
  if(it == end())
  {
    return value.empty();
  }
  else
  {
    return _stricmp(it->second.c_str(), value.c_str()) == 0;
  }
}

inline stringmap::const_index_proxy 
stringmap::operator [] (key_type const& key) const
{
  return const_index_proxy(*this, key);
}

inline stringmap::index_proxy 
stringmap::operator [] (key_type const& key)
{
  return index_proxy(*this, key);
}

inline stringmap::const_iterator 
stringmap::begin() const
{
  return base_type::begin();
}

inline stringmap::const_iterator 
stringmap::end() const
{
  return base_type::end();
}

inline stringmap::const_reverse_iterator
stringmap::rbegin() const
{
  return base_type::rbegin();
}

inline stringmap::const_reverse_iterator
stringmap::rend() const
{
  return base_type::rend();
}

/////////////////////////////////////////////////////////////////////////////////////
//
// const_index_proxy implementation

inline stringmap::const_index_proxy::const_index_proxy(
  this_type const& map, key_type const& key) :
m_map (map),
m_key (key)
{
}

inline stringmap::const_index_proxy::const_index_proxy(
  const_index_proxy const& rhs) :
m_map (rhs.m_map),
m_key (rhs.m_key)
{
}

inline stringmap::value_type const& 
stringmap::const_index_proxy::as_str() const
{
  const_iterator it = m_map.find(m_key);
  if(it == m_map.end())
  {
    static value_type dummy;
    return dummy;
  }
  return it->second;
}

inline int 
stringmap::const_index_proxy::as_int() const
{
  value_type const& lhs = *this;
  if(lhs.empty())
  {
    return 0;
  }
  return atoi(lhs.c_str());
}

inline bool 
stringmap::const_index_proxy::as_bln() const
{
  return ((int)*this) != 0;
}

inline 
stringmap::const_index_proxy::operator stringmap::value_type const& () const
{
  return as_str();
}

inline 
stringmap::const_index_proxy::operator int () const
{
  return as_int();
}

inline 
stringmap::const_index_proxy::operator bool () const
{
  return as_bln();
}

inline size_t 
stringmap::const_index_proxy::count() const
{
  value_type const& lhs = *this;  
  return lhs.empty() ? 0 : std::count(lhs.begin(), lhs.end(), ',') + 1;
}

template <typename FwdIterator, typename T>
inline FwdIterator find_nth(FwdIterator first, 
                            FwdIterator last, 
                            size_t num, T value)
{
  size_t n = 0;
  while(n < num)
  {
    first = std::find(first, last, value);
    if(first == last)
    {
      return last;
    }
    if(++n == num)
    {
      return first;
    }
    ++first;
  }
  return first;
}

inline stringmap::value_type 
stringmap::const_index_proxy::operator [] (size_t index) const
{
  value_type const& lhs = *this;
  value_type::const_iterator it = find_nth(lhs.begin(), lhs.end(), index, ',');
  value_type::const_iterator ie = lhs.end();
  if(it != ie)
  {
    if(it != lhs.begin()) ++it;
    ie = std::find(it, lhs.end(), ',');
  }
  return value_type(it, ie);
}

inline bool 
stringmap::const_index_proxy::operator == (value_type const& rhs) const
{
  value_type const& lhs = *this;
  return lhs == rhs;
}
inline bool 
stringmap::const_index_proxy::operator == (char const* rhs) const
{
  value_type const& lhs = *this;
  return lhs == value_type(rhs);
}
inline bool 
stringmap::const_index_proxy::operator == (int rhs) const
{
  return int(*this) == rhs;
}
inline bool 
stringmap::const_index_proxy::operator == (bool rhs) const
{
  return bool(*this) == rhs;
}
inline bool 
stringmap::const_index_proxy::operator != (value_type const& rhs) const
{
  return !(*this == rhs);
}
inline bool 
stringmap::const_index_proxy::operator != (char const* rhs) const
{
  return !(*this == rhs);
}
inline bool 
stringmap::const_index_proxy::operator != (int rhs) const
{
  return !(*this == rhs);
}
inline bool 
stringmap::const_index_proxy::operator != (bool rhs) const
{
  return !(*this == rhs);
}

inline String 
operator + (String const& lhs, stringmap::const_index_proxy const& rhs)
{
  return lhs + rhs.as_str();
}

/////////////////////////////////////////////////////////////////////////////////////
//
// index_proxy implementation

inline stringmap& 
stringmap::index_proxy::ncmap()
{
  return const_cast<this_type&>(m_map);
}

inline stringmap::index_proxy::index_proxy(this_type& map, key_type const& key) :
const_index_proxy(map, key)
{
}

inline stringmap::index_proxy::index_proxy(index_proxy const& rhs) :
const_index_proxy(rhs.m_map, rhs.m_key)
{
}

inline stringmap::value_type const& 
stringmap::index_proxy::operator = (value_type const& value)
{
  ncmap().set(m_key, value);
  return value;
}

inline char const* 
stringmap::index_proxy::operator = (char const* value)
{
  *this = value_type(value);
  return value;
}

inline int 
stringmap::index_proxy::operator = (int value)
{
  char buff[15];
  _itoa_s(value, buff, 10);
  *this = value_type(buff);
  return value;
}

inline bool 
stringmap::index_proxy::operator = (bool value)
{
  *this = value ? 1 : 0;
  return value;
}

inline stringmap::value_type const& 
stringmap::index_proxy::operator += (value_type const& value)
{
  if(!value.empty())
  {
    value_type v = *this;
    if(v.empty())
    {
      v = value;
    }
    else
    {
      v += ",";
      v += value;
    }
    *this = v;
  }
  return *this;
}

#endif  // #ifndef STRINGMAP_H
