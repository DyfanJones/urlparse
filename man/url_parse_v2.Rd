% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/RcppExports.R
\name{url_parse_v2}
\alias{url_parse_v2}
\title{Parses a vector URLs into a dataframe.}
\usage{
url_parse_v2(url)
}
\arguments{
\item{url}{A vector of strings, where each string is a URL to be parsed.}
}
\value{
A data frame with the following columns:
- href: The original URL.
- scheme: The scheme component of the URL (e.g., "http", "https").
- user: The user component of the URL.
- password: The password component of the URL.
- host: The host component of the URL.
- port: The port component of the URL.
- path: The decoded path component of the URL.
- raw_path: The raw path component of the URL.
- raw_query: The raw query component of the URL.
- fragment: The fragment component of the URL.
}
\description{
Parses a vector of URLs into their respective components.
It returns a data.frame where each row represents a URL,
and each column represents a specific component of the URL
such as the scheme, user, password, host, port, path, raw path,
raw query, and fragment.
}
\examples{
library(urlparse)
urls <- c("https://user:password@www.example.com:8080/path/to/resource?query=example#fragment",
          "http://www.test.com")
url_parse_v2(urls)

}
