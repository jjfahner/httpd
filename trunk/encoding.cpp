#include "precomp.h"
#include "encoding.h"

//////////////////////////////////////////////////////////////////////////
//
// Base64 encoding
//

String 
base64_encode(String const& input)
{
  // Encoding table
  static char const* enctable = 
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef"
    "ghijklmnopqrstuvwxyz0123456789+/";

  // Output string
  String dst;

  // Extract data and length
  char const*  src = input.c_str();
  unsigned int len = input.length();

  // Encode string
  while(len)
  {
    unsigned num = 0;
    unsigned enc = 0;
    switch(len)
    {
    default: enc |= (src[2]);      ++num;
    case 2:  enc |= (src[1]<<8);   ++num;
    case 1:  enc |= (src[0]<<16);  ++num;
      dst += enctable[(enc>>0x12) & 0x3F];
      dst += enctable[(enc>>0x0C) & 0x3F];
      dst += enctable[(enc>>0x06) & 0x3F];
      dst += enctable[(enc>>0x00) & 0x3F];    
      src += num;
      len -= num; 
      break;
    }
  }

  // Done
  return dst;
}

//////////////////////////////////////////////////////////////////////////
//
// Base64 decoding
//

String 
base64_decode(String const& input)
{
  static char const dectable[] = 
  { 
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0, 62,  0,  0,  0, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61,  0,  0,
    0,  0,  0,  0,  0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
   15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,  0,  0,  0,  0,  0,  0, 26, 27, 28,
   29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
   49, 50, 51,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  };

  String dst;

  // Reserve space for output
  dst.reserve(input.length() / 4 * 3 + 1);

  // Get source
  char const* src = input.c_str();

  // Decode the string
  while(*src)
  {
    unsigned dec = 0;
    dec |= dectable[src[0]]<<0x12;
    dec |= dectable[src[1]]<<0x0C;
    dec |= dectable[src[2]]<<0x06;
    dec |= dectable[src[3]]<<0x00;
    dst += (char)((dec>>0x10) & 0xFF);
    dst += (char)((dec>>0x08) & 0xFF);
    dst += (char)((dec>>0x00) & 0xFF);
    src += 4;
  }

  // Done
  return dst;
}

//////////////////////////////////////////////////////////////////////////
//
// Helpers for url encoding/decoding
//

inline unsigned int hex2num(char ch)
{
  if(ch >= '0' && ch <= '9') return ch - '0';
  if(ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
  if(ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
  throw std::exception("invalid hexadecimal literal");
}

inline char 
hex_decode(char const* hex)
{
  return (char)((hex2num(hex[0]) << 4) + hex2num(hex[1]));
}

inline void 
hex_encode(char ch, char* sz)
{
  static const char hex[] = "0123456789ABCDEF";
  sz[0] = hex[ch / 16];
  sz[1] = hex[ch % 16]; 
}

// Predicate to determine whether a char must be url-
// encoded (ie. whether is is not alphanumeric or space)
struct pred_must_url_encode { 
  bool operator () (char ch) { 
    return !(isalnum(ch) || ch == ' '); 
  } 
};


//////////////////////////////////////////////////////////////////////////
//
// Perform url encoding. This replaces any invalid character with
// an escape character.
//

String 
url_encode(String in)
{
  // Replace non-alpanumerics with hex code
  String::const_iterator io = in.begin();
  String::const_iterator it = io;
  String::const_iterator ie = in.end();
  char sz[4] = { '%', 0, 0, 0 };
  for(;; it += 3)
  {
    // Find character to encode
    it = std::find_if(it, ie, pred_must_url_encode());
    if(it == ie)
    {
      break;
    }

    // Hex-encode
    hex_encode(*it, sz + 1);

    // Replace with encoded version
    in.replace(it - io, 1, sz, 3);
  }

  // Convert ' ' to '+'
  std::replace(in.begin(), in.end(), ' ', '+');

  // Done
  return in;
}


//////////////////////////////////////////////////////////////////////////
//
// Decode a url, removing all escape characters
//

String 
url_decode(String in)
{
  // Convert '+' to ' '
  std::replace(in.begin(), in.end(), '+', ' ');

  // Replace hex with char
  String::size_type pos = 0;
  for(;; ++pos)
  {
    // Find hex offset
    if((pos = in.find('%', pos)) == String::npos)
    {
      return in;
    }

    // Replace with char
    in.replace(pos, 3, 1, hex_decode(&in[pos + 1]));
  }
}

