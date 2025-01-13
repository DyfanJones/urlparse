#include <Rcpp.h>
#include "encoding.h"

#include <string>
#include <vector>
#include <algorithm>
#include <sstream>

using namespace Rcpp;

/**
 * @title Parses a query string into a list of key-value pairs.
 *
 * @param query The query string to parse. It may start with a '?'.
 *
 * @return
 * A list where each element is a key-value pair from the query string.
 * If the query string is empty, an empty list is returned.
 *
 */
List parse_query_string(std::string query)
{
  // Remove leading '?' if present
  if (!query.empty() && query[0] == '?')
  {
    query.erase(0, 1);
  }

  // Return an empty list if the query is empty
  if (query.empty())
  {
    return List::create();
  }

  std::vector<std::pair<std::string, std::string>> result_vector;
  size_t start = 0, end = 0;
  std::string key, value;

  while (start < query.length())
  {
    end = query.find('=', start);
    if (end == std::string::npos || query[end] == '&')
    {
      // Handle case where there is no '=' or it's part of an empty key-value pair
      key = query.substr(start, query.find('&', start) - start);
      value = "";
      start = query.find('&', start);
      if (start == std::string::npos)
      {
        start = query.length();
      }
      else
      {
        ++start;
      }
    }
    else
    {
      key = query.substr(start, end - start);
      start = end + 1;
      end = query.find('&', start);
      if (end == std::string::npos)
      {
        value = query.substr(start);
        start = query.length();
      }
      else
      {
        value = query.substr(start, end - start);
        start = end + 1;
      }
    }
    result_vector.push_back({internal_url_unencode(key), internal_url_unencode(value)});
  }

  List result(result_vector.size());
  CharacterVector names(result_vector.size());
  for (size_t i = 0; i < result_vector.size(); ++i)
  {
    names[i] = result_vector[i].first;
    result[i] = result_vector[i].second;
  }
  result.attr("names") = names;

  return result;
}

/** @title Encodes a list of key-value pairs into a query string.
 *
 * @param params A list where each element is a key-value pair to be encoded.
 *
 * @return A query string representing the key-value pairs.
 *
 */
std::string build_query_string(List params)
{
  if (params.size() == 0)
  {
    return "";
  }
  std::string query;
  std::vector<std::string> nv = params.names();
  for (int i = 0; i < params.size(); ++i)
  {
    std::string key = internal_url_encode(nv[i], "");
    std::string value = internal_url_encode(params[i], "");
    if (!query.empty())
    {
      query += "&";
    }
    query += key + "=" + value;
  }
  return query;
}

class URL
{
public:
  std::string scheme;
  std::string user;
  std::string password;
  std::string host;
  std::string port;
  std::string raw_path;
  std::string path;
  std::string raw_query;
  std::string fragment;

  // Method to recreate the URL string from components
  std::string toString() const
  {
    if (scheme.empty() && host.empty())
    {
      return "";
    }

    std::ostringstream url;
    if (!scheme.empty())
    {
      url << scheme << "://";
    }
    if (!user.empty() || !password.empty())
    {
      url << user;
      if (!password.empty())
      {
        url << ":" << password;
      }
      url << "@";
    }
    url << host;
    if (!port.empty())
    {
      url << ":" << port;
    }
    if (!raw_path.empty())
    {
      // Ensure the raw_path starts with a '/'
      if (raw_path[0] != '/')
      {
        url << "/";
      }
      url << raw_path;
    }
    else
    {
      url << path;
    }
    if (!raw_query.empty())
    {
      url << "?" << raw_query;
    }
    if (!fragment.empty())
    {
      url << "#" << fragment;
    }
    return url.str();
  }
};

class URLParser
{
public:
  static URL parse(const std::string &url)
  {
    URL result;
    std::string::const_iterator it = url.begin();
    std::string::const_iterator end = url.end();

    // Parse scheme
    static const std::string scheme_delim = "://";
    auto scheme_end = std::search(it, end, scheme_delim.begin(), scheme_delim.end());
    if (scheme_end != end)
    {
      result.scheme = std::string(it, scheme_end);
      it = scheme_end + scheme_delim.size(); // Skip "://"
    }

    // Parse host (including port if present)
    auto host_end = std::find_if(it, end, [](char ch)
                                 { return ch == '/' || ch == '?' || ch == '#'; });
    std::string host_port = std::string(it, host_end);

    // Use rfind to locate the last occurrence of ':' in host_port
    // Use find to locate '@' in host_port
    auto colon_pos = host_port.rfind(':');
    auto at_pos = host_port.find('@');

    if (colon_pos != std::string::npos)
    {
      if (at_pos != std::string::npos)
      {
        if (colon_pos > at_pos)
        {
          // contains: <user>:<password>@<host>:<port>
          result.host = host_port.substr(0, colon_pos);
          result.port = host_port.substr(colon_pos + 1);

          // split user, password and host
          auto user_col_pos = result.host.find(':');
          if (user_col_pos != std::string::npos)
          {
            result.user = result.host.substr(0, user_col_pos);
            result.password = result.host.substr(user_col_pos + 1, at_pos - user_col_pos - 1);
            result.host = result.host.substr(at_pos + 1);
          }
          else
          {
            // assume user when ":" can't be found
            result.user = result.host.substr(0, at_pos);
            result.host = result.host.substr(at_pos + 1);
          }
        }
        else
        {
          // contains: <user>:<pass>@<host>
          result.user = host_port.substr(0, colon_pos);
          result.password = host_port.substr(colon_pos + 1, at_pos - colon_pos - 1);
          result.host = host_port.substr(at_pos + 1);
        }
      }
      else
      {
        // contains: <host>:<port>
        result.host = host_port.substr(0, colon_pos);
        result.port = host_port.substr(colon_pos + 1);
      }
    }
    else if (at_pos != std::string::npos)
    {
      // contains: <user>@<host>
      result.user = host_port.substr(0, at_pos);
      result.host = host_port.substr(at_pos + 1);
    }
    else
    {
      // contains: <host>
      result.host = host_port;
    }

    it = host_end;

    // Parse path
    if (it != end && *it == '/')
    {
      auto path_end = std::find_if(it, end, [](char ch)
                                   { return ch == '?' || ch == '#'; });
      result.path = std::string(it, path_end);
      it = path_end;
    }

    // Parse raw_query
    if (it != end && *it == '?')
    {
      auto query_end = std::find(it, end, '#');
      result.raw_query = std::string(it + 1, query_end); // Skip '?'
      it = query_end;
    }

    // Parse fragment
    if (it != end && *it == '#')
    {
      result.fragment = std::string(it + 1, end); // Skip '#'
    }

    return result;
  }
};

//' Parses a URL string into its components.
//'
//' @param url The URL string to parse.
//'
//' @return
//' A list containing the components of the URL: scheme, user, password, host, path, raw_path, query, raw_query, and fragment.
//'
//' @examples
//' library(urlparse)
//' url_parse("https://host.com/path?query#fragment")
//'
//' @export
//' @useDynLib urlparse _urlparse_url_parse
//' @importFrom Rcpp evalCpp
// [[Rcpp::export]]
Rcpp::List url_parse(const std::string &url)
{
  URL parsed_url = URLParser::parse(url);
  std::string raw_path = parsed_url.path;
  std::string path = internal_url_unencode(raw_path);
  std::string escaped_path = internal_url_encode(raw_path, "$&+,/;:=@");

  if (escaped_path == raw_path)
  {
    raw_path = "";
  }

  Rcpp::List query = parse_query_string(parsed_url.raw_query);
  std::string raw_query = build_query_string(query);

  Rcpp::List result = Rcpp::List::create(
      Rcpp::Named("scheme") = parsed_url.scheme,
      Rcpp::Named("user") = parsed_url.user,
      Rcpp::Named("password") = parsed_url.password,
      Rcpp::Named("host") = parsed_url.host,
      Rcpp::Named("port") = parsed_url.port,
      Rcpp::Named("path") = path,
      Rcpp::Named("raw_path") = raw_path,
      Rcpp::Named("query") = query,
      Rcpp::Named("raw_query") = raw_query,
      Rcpp::Named("fragment") = parsed_url.fragment);

  return result;
}

//' @title Parses a vector URLs into a dataframe.
//' @description Parses a vector of URLs into their respective components.
//'              It returns a data.frame where each row represents a URL,
//'              and each column represents a specific component of the URL
//'              such as the scheme, user, password, host, port, path, raw path,
//'              raw query, and fragment.
//' @param url A vector of strings, where each string is a URL to be parsed.
//' @return A data frame with the following columns:
//'         - href: The original URL.
//'         - scheme: The scheme component of the URL (e.g., "http", "https").
//'         - user: The user component of the URL.
//'         - password: The password component of the URL.
//'         - host: The host component of the URL.
//'         - port: The port component of the URL.
//'         - path: The decoded path component of the URL.
//'         - raw_path: The raw path component of the URL.
//'         - raw_query: The raw query component of the URL.
//'         - fragment: The fragment component of the URL.
//' @examples
//' library(urlparse)
//' urls <- c("https://user:password@www.example.com:8080/path/to/resource?query=example#fragment",
//'           "http://www.test.com")
//' url_parse_v2(urls)
//'
//' @export
//' @useDynLib urlparse _urlparse_url_parse_v2
//' @importFrom Rcpp evalCpp
// [[Rcpp::export]]
Rcpp::List url_parse_v2(std::vector<std::string> url)
{
  int n = url.size();

  Rcpp::CharacterVector href(n);
  Rcpp::CharacterVector scheme(n);
  Rcpp::CharacterVector user(n);
  Rcpp::CharacterVector password(n);
  Rcpp::CharacterVector host(n);
  Rcpp::CharacterVector port(n);
  Rcpp::CharacterVector path(n);
  Rcpp::CharacterVector raw_path(n);
  Rcpp::CharacterVector raw_query(n);
  Rcpp::CharacterVector fragment(n);
  Rcpp::IntegerVector row_name(n);

  for (int i = 0; i < n; ++i)
  {
    URL parsed_url = URLParser::parse(url[i]);
    std::string raw_pathi = parsed_url.path;
    std::string pathi = internal_url_unencode(raw_pathi);
    std::string escaped_path = internal_url_encode(raw_pathi, "$&+,/;:=@");

    if (escaped_path == raw_pathi)
    {
      raw_pathi = "";
    }
    href[i] = url[i];
    scheme[i] = parsed_url.scheme;
    user[i] = parsed_url.user;
    password[i] = parsed_url.password;
    host[i] = parsed_url.host;
    port[i] = parsed_url.port;
    path[i] = pathi;
    raw_path[i] = raw_pathi;
    raw_query[i] = build_query_string(parse_query_string(parsed_url.raw_query));
    fragment[i] = parsed_url.fragment;
    row_name[i] = i + 1; // R row names are 1-based
  }
  Rcpp::List result = Rcpp::List::create(
      Rcpp::Named("href") = href,
      Rcpp::Named("scheme") = scheme,
      Rcpp::Named("user") = user,
      Rcpp::Named("password") = password,
      Rcpp::Named("host") = host,
      Rcpp::Named("port") = port,
      Rcpp::Named("path") = path,
      Rcpp::Named("raw_path") = raw_path,
      Rcpp::Named("raw_query") = raw_query,
      Rcpp::Named("fragment") = fragment);

  // as data.frame is expensive - create from the list
  result.attr("row.names") = row_name;
  result.attr("class") = "data.frame";

  return result;
}

//' @title Builds a URL string from its components.
//'
//' @param url_components A list containing the components of the URL: scheme, host, port, path, query, and fragment.
//' - **scheme** A character string for the new scheme (e.g., "http" or "https") or `NULL` to keep it unchanged.
//' - **host** A character string for the new host or `NULL` to keep it unchanged.
//' - **port** A character string for the new port or `NULL` to keep it unchanged.
//' - **path** A character string for the new path or `NULL` to keep it unchanged.
//' - **query** A list or character of new query parameters or `NULL` to keep it unchanged.
//' - **fragment** A character string for the new fragment or `NULL` to keep it unchanged.
//'
//' @return A URL string constructed from the provided components
//'
//' @examples
//' library(urlparse)
//' url_build(list(
//'   scheme = "https",
//'   user = "",
//'   password = "",
//'   host = "host.com",
//'   port = 8000,
//'   path = "/path",
//'   query = "query",
//'   fragment = "fragment"
//' ))
//'
//' @export
//' @useDynLib urlparse _urlparse_url_build
//' @importFrom Rcpp evalCpp
// [[Rcpp::export]]
std::string url_build(const Rcpp::List &url_components)
{
  URL url;
  if (url_components.containsElementNamed("scheme"))
  {
    url.scheme = Rcpp::as<std::string>(url_components["scheme"]);
  }
  if (url_components.containsElementNamed("user"))
  {
    url.user = Rcpp::as<std::string>(url_components["user"]);
  }
  if (url_components.containsElementNamed("password"))
  {
    url.password = Rcpp::as<std::string>(url_components["password"]);
  }
  if (url_components.containsElementNamed("host"))
  {
    url.host = Rcpp::as<std::string>(url_components["host"]);
  }

  if (url_components.containsElementNamed("port"))
  {
    // Check if port is an integer, double, or string and convert to string
    if (TYPEOF(url_components["port"]) == INTSXP)
    {
      url.port = std::to_string(Rcpp::as<int>(url_components["port"]));
    }
    else if (TYPEOF(url_components["port"]) == REALSXP)
    {
      url.port = std::to_string(Rcpp::as<int>(url_components["port"]));
    }
    else if (TYPEOF(url_components["port"]) == STRSXP)
    {
      url.port = Rcpp::as<std::string>(url_components["port"]);
    }
    else
    {
      Rcpp::stop("Port must be an integer, double, or string.");
    }
  }
  if (url_components.containsElementNamed("path"))
  {
    url.path = Rcpp::as<std::string>(url_components["path"]);
    url.raw_path = url.path;
  }
  if (url_components.containsElementNamed("raw_path"))
  {
    url.raw_path = Rcpp::as<std::string>(url_components["raw_path"]);
  }
  if (url_components.containsElementNamed("raw_query"))
  {
    url.raw_query = Rcpp::as<std::string>(url_components["raw_query"]);
  }
  else if (url_components.containsElementNamed("query"))
  {
    // Check if query is a string or list and build query string
    if (TYPEOF(url_components["query"]) == STRSXP)
    {
      url.raw_query = Rcpp::as<std::string>(url_components["query"]);
    }
    else if (TYPEOF(url_components["query"]) == VECSXP)
    {
      url.raw_query = build_query_string(Rcpp::as<Rcpp::List>(url_components["query"]));
    }
    else
    {
      Rcpp::stop("Query must be a string or a list.");
    }
  }
  if (url_components.containsElementNamed("fragment"))
  {
    url.fragment = Rcpp::as<std::string>(url_components["fragment"]);
  }
  return url.toString();
}

//' @title Modifies a URL string by updating its components.
//'
//' @description
//' This function modifies a URL string by updating its components such as scheme, user, password, host, port, query, raw query, and fragment.
//' If any of these components are not provided (i.e., `NULL`), the existing components of the URL are retained.
//'
//' @param url A character string representing the original URL.
//' @param scheme A character string for the new scheme (e.g., "http" or "https") or `NULL` to keep it unchanged.
//' @param user A character string for the username or `NULL` to keep it unchanged.
//' @param password A character string for the new password or `NULL` to keep it unchanged.
//' @param host A character string for the new host or `NULL` to keep it unchanged.
//' @param port A character string for the new port or `NULL` to keep it unchanged.
//' @param path A character string for the new path or `NULL` to keep it unchanged.
//' @param query A list or character of new query parameters or `NULL` to keep it unchanged.
//' @param fragment A character string for the new fragment or `NULL` to keep it unchanged.
//'
//' @return
//' A character string representing the modified URL.
//'
//' @examples
//' library(urlparse)
//'
//' # Example 1: Modify the scheme and host of a URL
//' url_modify(
//'   "https://user:pass@host.com/path?query#fragment",
//'   scheme = "http",
//'   host = "example.com"
//' )
//'
//' # Example 2: Add a query parameter to a URL
//' url_modify(
//'   "https://host.com/path", query = list(key1 = "value1", key2 = "value2")
//' )
//'
//' # Example 3: Change the fragment of a URL
//' url_modify("https://host.com/path#old_fragment", fragment = "new_fragment")
//'
//' @name url_modify
//' @export
//' @useDynLib urlparse _urlparse_url_modify
//' @importFrom Rcpp evalCpp
// [[Rcpp::export]]
std::string url_modify(const std::string &url,
                       Nullable<std::string> scheme = R_NilValue,
                       Nullable<std::string> user = R_NilValue,
                       Nullable<std::string> password = R_NilValue,
                       Nullable<std::string> host = R_NilValue,
                       Nullable<RObject> port = R_NilValue,
                       Nullable<std::string> path = R_NilValue,
                       Nullable<RObject> query = R_NilValue,
                       Nullable<std::string> fragment = R_NilValue)
{
  URL parsed_url = URLParser::parse(url);

  if (scheme.isNotNull())
  {
    parsed_url.scheme = Rcpp::as<std::string>(scheme);
  }
  if (user.isNotNull())
  {
    parsed_url.user = Rcpp::as<std::string>(user);
  }
  if (password.isNotNull())
  {
    parsed_url.password = Rcpp::as<std::string>(password);
  }
  if (host.isNotNull())
  {
    parsed_url.host = Rcpp::as<std::string>(host);
  }
  if (port.isNotNull())
  {
    // Check if port is an integer, double, or string and convert to string
    if (TYPEOF(port) == INTSXP)
    {
      parsed_url.port = std::to_string(Rcpp::as<int>(port));
    }
    else if (TYPEOF(port) == REALSXP)
    {
      parsed_url.port = std::to_string(Rcpp::as<int>(port));
    }
    else if (TYPEOF(port) == STRSXP)
    {
      parsed_url.port = Rcpp::as<std::string>(port);
    }
    else
    {
      stop("Port must be an integer, double, or string.");
    }
  }
  if (path.isNotNull())
  {
    parsed_url.path = Rcpp::as<std::string>(path);
    parsed_url.raw_path = parsed_url.path;
  }
  if (query.isNotNull())
  {
    if (TYPEOF(query) == STRSXP)
    {
      parsed_url.raw_query = Rcpp::as<std::string>(query);
    }
    else if (TYPEOF(query) == VECSXP)
    {
      parsed_url.raw_query = build_query_string(Rcpp::as<List>(query));
    }
    else
    {
      stop("Query must be a string or a list.");
    }
  }
  if (fragment.isNotNull())
  {
    parsed_url.fragment = Rcpp::as<std::string>(fragment);
  }

  return parsed_url.toString();
}

//' @rdname url_modify
//' @export
//' @useDynLib urlparse _urlparse_set_scheme
//' @importFrom Rcpp evalCpp
// [[Rcpp::export]]
std::string set_scheme(const std::string &url, const std::string &scheme)
{
  URL parsed_url = URLParser::parse(url);
  parsed_url.scheme = scheme;
  return parsed_url.toString();
}

//' @rdname url_modify
//' @export
//' @useDynLib urlparse _urlparse_set_host
//' @importFrom Rcpp evalCpp
// [[Rcpp::export]]
std::string set_user(const std::string &url, const std::string &user)
{
  URL parsed_url = URLParser::parse(url);
  parsed_url.user = user;
  return parsed_url.toString();
}

//' @rdname url_modify
//' @export
//' @useDynLib urlparse _urlparse_set_host
//' @importFrom Rcpp evalCpp
// [[Rcpp::export]]
std::string set_password(const std::string &url, const std::string &password)
{
  URL parsed_url = URLParser::parse(url);
  parsed_url.password = password;
  return parsed_url.toString();
}

//' @rdname url_modify
//' @export
//' @useDynLib urlparse _urlparse_set_host
//' @importFrom Rcpp evalCpp
// [[Rcpp::export]]
std::string set_host(const std::string &url, const std::string &host)
{
  URL parsed_url = URLParser::parse(url);
  parsed_url.host = host;
  return parsed_url.toString();
}

//' @rdname url_modify
//' @export
//' @useDynLib urlparse _urlparse_set_port
//' @importFrom Rcpp evalCpp
// [[Rcpp::export]]
std::string set_port(const std::string &url, RObject port)
{
  URL parsed_url = URLParser::parse(url);

  // Check if port is an integer, double, or string and convert to string
  if (TYPEOF(port) == INTSXP)
  {
    parsed_url.port = std::to_string(Rcpp::as<int>(port));
  }
  else if (TYPEOF(port) == REALSXP)
  {
    parsed_url.port = std::to_string(Rcpp::as<int>(port));
  }
  else if (TYPEOF(port) == STRSXP)
  {
    parsed_url.port = Rcpp::as<std::string>(port);
  }
  else
  {
    stop("Port must be an integer, double, or string.");
  }

  return parsed_url.toString();
}

//' @rdname url_modify
//' @export
//' @useDynLib urlparse _urlparse_set_port
//' @importFrom Rcpp evalCpp
// [[Rcpp::export]]
std::string set_path(const std::string &url, const std::string &path)
{
  URL parsed_url = URLParser::parse(url);
  parsed_url.raw_path = path;
  parsed_url.path = path;
  return parsed_url.toString();
}

//' @rdname url_modify
//' @export
//' @useDynLib urlparse _urlparse_set_query
//' @importFrom Rcpp evalCpp
// [[Rcpp::export]]
std::string set_query(const std::string &url, RObject query)
{
  URL parsed_url = URLParser::parse(url);

  if (TYPEOF(query) == STRSXP)
  {
    parsed_url.raw_query = Rcpp::as<std::string>(query);
  }
  else if (TYPEOF(query) == VECSXP)
  {
    parsed_url.raw_query = build_query_string(Rcpp::as<List>(query));
  }
  else
  {
    stop("Query must be a string or a list.");
  }

  return parsed_url.toString();
}

//' @rdname url_modify
//' @export
//' @useDynLib urlparse _urlparse_set_fragment
//' @importFrom Rcpp evalCpp
// [[Rcpp::export]]
std::string set_fragment(const std::string &url, const std::string &fragment)
{
  URL parsed_url = URLParser::parse(url);
  parsed_url.fragment = fragment;
  return parsed_url.toString();
}
