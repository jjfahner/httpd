#ifndef MISC_H
#define MISC_H

// Case-insensitive comparator for String
struct httpd_nocase_cmp 
{
  bool operator () (String const& lhs, String const& rhs) const
  { 
    return _stricmp(lhs.c_str(), rhs.c_str()) < 0; 
  }
};

#endif  // #ifndef MISC_H
