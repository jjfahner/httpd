#ifndef ENCODING_H
#define ENCODING_H

//
// Base64 encode a string
//
std::string base64_encode(std::string const& input);

//
// Decode base64 encoded string
//
std::string base64_decode(std::string const& input);

//
// Perform url encoding on string
//
std::string url_encode(std::string input);

//
// Perform url decoding on string
//
std::string url_decode(std::string input);


#endif  // #ifndef ENCODING_H
