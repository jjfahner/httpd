#ifndef EXTMAP_H
#define EXTMAP_H

template <typename Key, typename Value, typename Pred = std::less<Key>, typename Alloc = std::allocator<std::pair<const Key, Value> > >
class extmap : public std::map<Key, Value, Pred, Alloc>
{
public:

  //
  // Types
  //
  typedef std::map<Key, Value, Pred, Alloc> base_type;
  typedef extmap<Key, Value, Pred, Alloc>   this_type;
  typedef Key     key_type;
  typedef Value   value_type;

  //
  // Does the map contain a key
  //
  bool contains(key_type const& key) const
  {
    return find(key) != end();
  }

  //
  // Does the map contain a key,value
  //
  bool contains(key_type const& key, value_type const& value) const
  {
    const_iterator it = find(key);
    return it != end() && it->second == value;
  }

  //
  // Remove key from map
  //
  void remove(key_type const& key)
  {
    erase(find(key));
  }

  //
  // Remove all pairs with value
  //
  void remove_value(value_type const& value)
  {
    iterator it, ie;
    for(it = begin(), ie = end(); it != ie;)
    {
      if(it->second == value)
      {
        erase(it++);
      }
      else
      {
        ++it;
      }
    }
  }

  //
  // Index operator for const
  //
  value_type const& operator [] (key_type const& key) const
  {
    const_iterator it;
    if((it = find(key)) == end())
    {
      return dummy_value();
    }
    return it->second;
  }

  //
  // Index proxy for non-const
  //
  class index_proxy
  {
    this_type&  m_map;
    key_type    m_key;
  protected:
    value_type const& get() const
    {
      const_iterator it;
      if((it = m_map.find(m_key)) == m_map.end())
      {
        return dummy_value();
      }
      return it->second;
    }
  public:
    index_proxy(this_type& map, key_type const& key) :
      m_map (map), m_key (key) 
    {
    }
    
    value_type const& operator * () const   { return  get(); } 
    operator value_type const& () const     { return  get(); } 
    value_type const* operator &  () const  { return &get(); } 
    value_type const* operator -> () const  { return &get(); } 

    value_type const& operator = (value_type const& rhs) 
    {
      static_cast<base_type&>(m_map)[m_key] = rhs;
      return rhs;
    }

  private:
    index_proxy const& operator = (index_proxy const&);
  };

  //
  // Index operator for non-const
  //
  index_proxy operator [] (key_type const& key)
  {
    return index_proxy(*this, key);
  }

private:

  //
  // Returned when referring to non-existing elements
  //
  static value_type const& dummy_value()
  {
    static value_type vt;
    return vt;
  }

};

//
// Binary operator generator
//
#define EXTMAP_BINOP(res,op)  \
template <typename Key, typename Value, typename Pred, typename Alloc>  \
inline res operator op (typename extmap<Key, Value, Pred, Alloc>::index_proxy const& lhs, Value const& rhs)  \
{ \
  return *lhs op rhs; \
} \
template <typename Key, typename Value, typename Pred, typename Alloc>  \
inline res operator op (Value const& lhs, typename extmap<Key, Value, Pred, Alloc>::index_proxy const& rhs)  \
{ \
  return lhs op *rhs; \
}

// Comparison ops
EXTMAP_BINOP(bool,==)
EXTMAP_BINOP(bool,!=)
EXTMAP_BINOP(bool,<=)
EXTMAP_BINOP(bool,>=)
EXTMAP_BINOP(bool,<)
EXTMAP_BINOP(bool,>)

// Logical ops
EXTMAP_BINOP(bool,&&)
EXTMAP_BINOP(bool,||)

// Mathematical ops
EXTMAP_BINOP(Value,+)
EXTMAP_BINOP(Value,-)
EXTMAP_BINOP(Value,*)
EXTMAP_BINOP(Value,/)
EXTMAP_BINOP(Value,%)

// Bitwise ops
EXTMAP_BINOP(Value,&)
EXTMAP_BINOP(Value,|)
EXTMAP_BINOP(Value,^)
EXTMAP_BINOP(Value,<<)
EXTMAP_BINOP(Value,>>)

#endif  // #ifndef EXTMAP_H
