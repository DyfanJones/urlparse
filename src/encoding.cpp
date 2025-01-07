// This encoder has been modified from the excellent urltools R package
// https://github.com/Ironholds/urltools/blob/master/src/encoding.cpp

#include "encoding.h"
#include <Rcpp.h>

using namespace Rcpp;

// Precomputed hex conversion lookup table
const char hex_chars[] = "0123456789ABCDEF";

// Precomputed unreserved characters lookup table
const std::bitset<256> unreserved_chars_map = []
{
  std::bitset<256> bitset;
  std::string unreserved_chars = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ._~-";
  for (char ch : unreserved_chars)
  {
    bitset.set(static_cast<unsigned char>(ch));
  }
  return bitset;
}();

std::string internal_url_encode(const std::string &url, const std::string &safe)
{
  // Create a bitset for additional safe characters
  std::bitset<256> safe_chars_map = unreserved_chars_map;
  for (char ch : safe)
  {
    safe_chars_map.set(static_cast<unsigned char>(ch));
  }

  // Create an output string with preallocated memory
  std::string output;
  output.reserve(url.size() * 3); // Assume worst case where all characters are encoded

  // For each character
  for (char ch : url)
  {
    // If it's in the list of unreserved ones, just pass it through
    if (safe_chars_map.test(static_cast<unsigned char>(ch)))
    {
      output.push_back(ch);
    }
    else
    {
      // Otherwise, append in an encoded form
      output.push_back('%');
      output.append(to_hex(ch));
    }
  }

  // Return
  return output;
}

std::string internal_url_unencode(const std::string &url)
{
  std::string unescaped;
  unescaped.reserve(url.size()); // Reserve memory to avoid reallocations
  size_t length = url.length();

  for (size_t i = 0; i < length; ++i)
  {
    if (url[i] == '%' && i + 2 < length)
    {
      int high = from_hex(url[i + 1]);
      int low = from_hex(url[i + 2]);
      if (high != -1 && low != -1)
      {
        unescaped.push_back(static_cast<char>((high << 4) | low));
        i += 2;
      }
      else
      {
        unescaped.push_back('%'); // Invalid percent encoding, keep as is
      }
    }
    else
    {
      unescaped.push_back(url[i]); // Keep other characters as is
    }
  }
  return unescaped;
}

//' @title Escape characters for use in URLs.
//'
//' @description This function encodes a character vector for use in URLs, escaping all special characters
//' except for those specified in the `safe` parameter.
//'
//' @param urls A character vector to be encoded/decoded.
//' @param safe A character vector of extra characters that should not be encoded.
//'
//' @return A character vector with the encoded URLs.
//'
//' @examples
//' library(urlparse)
//'
//' # Example 1:
//' url_encoder("foo = bar + 5")
//'
//' # Example 2:
//' # prevent special characters being encoded:
//' url <- "https://example.com/path?query= 1+2"
//' url_encoder(url, ":/?=")
//'
//' # Example 3:
//' url_decoder(url_encoder("foo = bar + 5"))
//'
//' @name encoding
//' @export
//' @useDynLib urlparse _urlparse_url_encoder
//' @importFrom Rcpp evalCpp
// [[Rcpp::export]]
CharacterVector url_encoder(CharacterVector urls, CharacterVector safe = "")
{
  // Measure size, create output object and holding objects
  int input_size = urls.size();
  CharacterVector output(input_size);
  std::string safe_pattern = Rcpp::as<std::string>(safe);

  // For each string
  for (int i = 0; i < input_size; ++i)
  {
    // Check for user interrupts
    if ((i % 10000) == 0)
    {
      Rcpp::checkUserInterrupt();
    }

    if (urls[i] == NA_STRING)
    {
      output[i] = NA_STRING;
    }
    else
    {
      std::string holding = Rcpp::as<std::string>(urls[i]);
      output[i] = internal_url_encode(holding, safe_pattern);
    }
  }

  // Return
  return output;
}

//' @rdname encoding
//' @export
//' @useDynLib urlparse _urlparse_url_decoder
//' @importFrom Rcpp evalCpp
// [[Rcpp::export]]
CharacterVector url_decoder(CharacterVector urls)
{
  int input_size = urls.size();
  CharacterVector output(input_size);

  for (int i = 0; i < input_size; ++i)
  {
    if (urls[i] == NA_STRING)
    {
      output[i] = NA_STRING;
    }
    else
    {
      output[i] = internal_url_unencode(Rcpp::as<std::string>(urls[i]));
    }
    if ((i % 10000) == 0)
    {
      Rcpp::checkUserInterrupt();
    }
  }

  return output;
}
