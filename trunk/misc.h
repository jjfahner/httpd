#ifndef MISC_H
#define MISC_H

// Case-insensitive comparator for std::string
struct httpd_nocase_cmp 
{
  bool operator () (std::string const& lhs, std::string const& rhs) const
  { 
    return _stricmp(lhs.c_str(), rhs.c_str()) < 0; 
  }
};

#endif  // #ifndef MISC_H
