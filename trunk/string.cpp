#include "precomp.h"
#include "string.h"
#include <atlconv.h>

String::String(wchar_t const* str)
{
  USES_CONVERSION;
  *this = W2T(str);
}

wchar_t*
String::AllocSysString() const
{
  USES_CONVERSION;
  return SysAllocString(T2W(this->c_str()));
}
