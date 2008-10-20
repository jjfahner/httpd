#ifndef FSRESOLVER_H
#define FSRESOLVER_H

#include "resolver.h"

//
// Resolves requests within a filesystem
//
class filesystem_resolver : public mime_resolver
{
public:
  
  //
  // Construction
  //
   filesystem_resolver(String const& root);
  ~filesystem_resolver();

  //
  // Retrieve the root
  //
  String const& root() const;

  //
  // Resolve a http_request
  //
  mime_handler* resolve(http_context& context, String const& uri);

private:

  //
  // Resolve any \..\ refs
  //
  bool resolve_updirs(String path);

  // Filesystem root
  String m_root;

};

inline String const& 
filesystem_resolver::root() const
{
  return m_root;
}

#endif  // #ifndef FSRESOLVER_H
