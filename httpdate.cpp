#include "precomp.h"
#include "httpdate.h"

// #define _PRONTODLL
// #include "../Include/Basisklassen/BasisklassenImportExport.h"
// #include "../Include/warnings.h"
// #include "../Include/Basisklassen/String.h"
// #include "../Include/Basisklassen/Moment.h"

//////////////////////////////////////////////////////////////////////////

inline 
const char* weekday(int day)
{
  switch(day)
  {
  case 0: return "Sun";
  case 1: return "Mon";
  case 2: return "Tue";
  case 3: return "Wed";
  case 4: return "Thu";
  case 5: return "Fri";
  case 6: return "Sat";
  }
  throw std::exception("weekday(): invalid day");
}

inline int weekday(char const* day)
{
  if(!_stricmp(day, "sun")) return 0;
  if(!_stricmp(day, "mon")) return 1;
  if(!_stricmp(day, "tue")) return 2;
  if(!_stricmp(day, "wed")) return 3;
  if(!_stricmp(day, "thu")) return 4;
  if(!_stricmp(day, "fri")) return 5;
  if(!_stricmp(day, "sat")) return 6;
  throw std::exception("weekday(): invalid day");
}

inline const char* month_abr(int mon)
{
  switch(mon)
  {
  case 1:  return "Jan";
  case 2:  return "Feb";
  case 3:  return "Mar";
  case 4:  return "Apr";
  case 5:  return "May";
  case 6:  return "Jun";
  case 7:  return "Jul";
  case 8:  return "Aug";
  case 9:  return "Sep";
  case 10: return "Oct";
  case 11: return "Nov";
  case 12: return "Dec";
  }
  throw std::exception("month_abr(): invalid month");
}

inline int month_abr(char const* mon)
{
  if(!_stricmp(mon, "jan")) return 1;
  if(!_stricmp(mon, "feb")) return 2;
  if(!_stricmp(mon, "mar")) return 3;
  if(!_stricmp(mon, "apr")) return 4;
  if(!_stricmp(mon, "may")) return 5;
  if(!_stricmp(mon, "jun")) return 6;
  if(!_stricmp(mon, "jul")) return 7;
  if(!_stricmp(mon, "aug")) return 8;
  if(!_stricmp(mon, "sep")) return 9;
  if(!_stricmp(mon, "oct")) return 10;
  if(!_stricmp(mon, "nov")) return 11;
  if(!_stricmp(mon, "dec")) return 12;
  throw std::exception("month_abr(): invalid month");
}

//////////////////////////////////////////////////////////////////////////

httpdate::httpdate()
{
  set_now();
}

httpdate::httpdate(std::string const& src)
{
  if(!set(src))
  {
    throw std::runtime_error("Invalid date string");
  }
}

httpdate::httpdate(FILETIME const& ft)
{
  set(ft);
}

httpdate::httpdate(SYSTEMTIME const& st)
{
  if(!set(st))
  {
    throw std::runtime_error("Invalid SYSTEMTIME");
  }
}

bool
httpdate::set(std::string const& src)
{
  char wkday[4];
  char mon[4];
  char tz[4];

  SYSTEMTIME st;
  memset(&st, 0, sizeof(st));

  // Parse http date
  int args = sscanf_s(
    src.c_str(),
    "%3c, %02d %3c %4d %2d:%2d:%2d %3c", 
    wkday,
    &st.wDay,
    mon,
    &st.wYear,
    &st.wHour,
    &st.wMinute,
    &st.wSecond,
    tz);

  if(args != 8)
  {
    return false;
  }

  wkday[3]  = 0;
  mon[3]    = 0;
  tz[3]     = 0;

  if(strcmp(tz, "GMT"))
  {
    return false;
  }

  st.wDayOfWeek = (WORD) weekday(wkday);
  st.wMonth     = (WORD) month_abr(mon);

  return set(st);
}

void 
httpdate::set(FILETIME const& ft)
{
  m_time.LowPart  = ft.dwLowDateTime;
  m_time.HighPart = ft.dwHighDateTime;
  m_time.QuadPart -= m_time.QuadPart % 10000000LL;
}

bool
httpdate::set(SYSTEMTIME const& st)
{
  FILETIME ft;
  if(!SystemTimeToFileTime(&st, &ft))
  {
    return false;
  }
  set(ft);
  return true;
}

void 
httpdate::set_now()
{
  FILETIME ft;
  GetSystemTimeAsFileTime(&ft);
  set(ft);
}

std::string 
httpdate::str() const
{
  SYSTEMTIME st;
  FILETIME ft;
  ft.dwLowDateTime  = m_time.LowPart;
  ft.dwHighDateTime = m_time.HighPart;

  FileTimeToSystemTime(&ft, &st);

  char date[50];
  sprintf_s(date, "%s, %02d %s %d %02d:%02d:%02d GMT", 
    weekday(st.wDayOfWeek), 
    st.wDay, 
    month_abr(st.wMonth), 
    st.wYear,
    st.wHour,
    st.wMinute,
    st.wSecond
    );

  return date;
}

httpdate&
httpdate::add_seconds(int seconds)
{
  // Dit moet echt gewoon zo! (vraag maar aan Wouter)
  m_time.QuadPart += seconds * 10000000LL;
  return *this;
}
