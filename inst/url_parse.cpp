#include <Rcpp.h>
#include "encoding.h"

#include <string>
#include <vector>
#include <algorithm>
#include <sstream>

using namespace Rcpp;

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
        url << host;
        if (!port.empty())
        {
            url << ":" << port;
        }
        if (!raw_path.empty())
        {
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
                                     { return ch == ':' || ch == '/' || ch == '?' || ch == '#'; });
        result.host = std::string(it, host_end);
        it = host_end;

        // Parse port (if present)
        if (it != end && *it == ':')
        {
            auto port_end = std::find_if(it + 1, end, [](char ch)
                                         { return ch == '/' || ch == '?' || ch == '#'; });
            result.port = std::string(it + 1, port_end); // Skip ':'
            it = port_end;
        }

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

// [[Rcpp::export]]
Rcpp::List url_parse_v2(const std::string &url)
{
    URL parsed_url = URLParser::parse(url);
    std::string raw_path = parsed_url.path.empty() ? "/" : parsed_url.path;
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
        Rcpp::Named("host") = parsed_url.host,
        Rcpp::Named("port") = parsed_url.port,
        Rcpp::Named("path") = path,
        Rcpp::Named("raw_path") = raw_path,
        Rcpp::Named("query") = query,
        Rcpp::Named("raw_query") = raw_query,
        Rcpp::Named("fragment") = parsed_url.fragment);

    return result;
}
