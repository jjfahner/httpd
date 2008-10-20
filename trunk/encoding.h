#ifndef ENCODING_H
#define ENCODING_H

//
// Base64 encode a string
//
String base64_encode(String const& input);

//
// Decode base64 encoded string
//
String base64_decode(String const& input);

//
// Perform url encoding on string
//
String url_encode(String input);

//
// Perform url decoding on string
//
String url_decode(String input);


#endif  // #ifndef ENCODING_H
