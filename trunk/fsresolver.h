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
   filesystem_resolver(std::string const& root);
  ~filesystem_resolver();

  //
  // Resolve a http_request
  //
  mime_handler* resolve(http_context& context, std::string const& uri);

private:

  //
  // Resolve any \..\ refs
  //
  bool resolve_updirs(std::string path);

  // Filesystem root
  std::string m_root;

};

#endif  // #ifndef FSRESOLVER_H
