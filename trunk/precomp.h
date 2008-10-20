#ifndef PRECOMP_H
#define PRECOMP_H

#pragma once

#pragma warning(disable:4702) // unreachable code

// Windows headers/libs
#define _WIN32_WINNT 0x0500
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// System headers
#include <process.h>

// STL headers
#include <iostream>
#include <string>
#include <list>
#include <map>
#include <vector>
#include <deque>
#include <algorithm>
#include <sstream>

//
// Include project string type
//
#include "string.h"

/////////////////////////////////////////////////////////////////////////////////////
// Not implemented exception

// Feature not implemented
class notimpl_exception : public std::exception
{
  static inline std::string format_error(char const* file, int line)
  {
    char buff[1000];
    sprintf_s(buff, "%s(%d) : Feature not implemented", file, line);
    return buff;
  }
public:
  notimpl_exception(char const* file, int line) : std::exception(format_error(file, line).c_str()) 
  {
  }
};

// Use NOT_IMPLEMENTED to throw exception
#define NOT_IMPLEMENTED \
  throw notimpl_exception(__FILE__,__LINE__)


#endif  // #ifndef PRECOMP_H
