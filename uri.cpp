#include "precomp.h"
#include "uri.h"

bool
uri::parse(String const& src)
{
  clear();

  // Find protocol prefix
  String::size_type pos1 = 0, pos2;
  if((pos1 = src.find(':')) != String::npos)
  {
    if(src[pos1 + 1] == '/' && src[pos1 + 2] == '/')
    {
      m_prot = src.substr(0, pos1);
      pos1 += 3;
    }
    else
    {
      pos1 = 0;
    }
  }

  // Find port separator
  if((pos2 = src.find(':', pos1)) != String::npos)
  {
    // Ensure that the port sep lies before any path seps
    if(src.find('/', pos1) > pos2)
    {
      // Extract host
      m_host = src.substr(pos1, pos2 - pos1);

      // Find port end
      pos1 = pos2 + 1;
      pos2 = src.find_first_not_of("0123456789", pos1);
      //String::size_type len = pos2 == String::npos ? pos2 : pos2 - pos1;

      // Extract port
      m_port = atoi(src.substr(pos1, pos2 - pos1).c_str());

      // Correct pos
      pos1 = pos2;
    }
  }
  else
  {
    // Find end of hostname
    pos2 = src.find('/', pos1);
    m_host = src.substr(pos1, pos2 - pos1);
  }

  return true;
}

String 
uri::str() const
{
  String res;
  if(!m_prot.empty())
  {
    res = m_prot + "://";
  }  
  if(!m_host.empty())
  {
    res += m_host;
    if(m_port)
    {
      res += ":";

      char szPort[10];
      _itoa_s(m_port, szPort, 10);
      res += szPort;
    }
  }
  res += m_path;
  return res;
}
