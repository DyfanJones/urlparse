
<!-- README.md is generated from README.Rmd. Please edit that file -->

# urlparse

<!-- badges: start -->

[![CRAN
status](https://www.r-pkg.org/badges/version/urlparse)](https://CRAN.R-project.org/package=urlparse)
[![R-CMD-check](https://github.com/DyfanJones/urlparse/actions/workflows/R-CMD-check.yaml/badge.svg)](https://github.com/DyfanJones/urlparse/actions/workflows/R-CMD-check.yaml)
<!-- badges: end -->

Fast and simple url parser for R. Initially developed for the
`paws.common` package. `urlparse` is influenced by python’s
`from urllib.parse import urlparse` and doesn’t split out username or
password from the host.

``` python
from urllib.parse import urlparse
urlparse("https://user:pass@host.com:8000/path?query=1#fragment")
#> ParseResult(scheme='https', netloc='user:pass@host.com:8000', path='/path', params='', query='query=1', fragment='fragment')
```

``` r
urlparse::url_parse("https://user:pass@host.com:8000/path?query=1#fragment")
#> $scheme
#> [1] "https"
#> 
#> $host
#> [1] "user:pass@host.com"
#> 
#> $port
#> [1] "8000"
#> 
#> $path
#> [1] "/path"
#> 
#> $raw_path
#> [1] ""
#> 
#> $query
#> $query$query
#> [1] "1"
#> 
#> 
#> $raw_query
#> [1] "query=1"
#> 
#> $fragment
#> [1] "fragment"
```

This allows `urlparse` to handle fairly complex url’s without
sacrificing any performance.

## Installation

You can install the development version of urlparse like so:

``` r
remotes::install_github("dyfanjones/urlparse")
```

r-universe installation:

``` r
install.packages("urlparse", repos = c("https://dyfanjones.r-universe.dev", "https://cloud.r-project.org"))
```

## Example

This is a basic example which shows you how to solve a common problem:

``` r
library(urlparse)
```

``` r
url_encoder("foo = bar + 5")
#> [1] "foo%20%3D%20bar%20%2B%205"

url_decoder(url_encoder("foo = bar + 5"))
#> [1] "foo = bar + 5"
```

Similar to python’s `from urllib.parse import quote`,
`urlparse::url_encoder` supports the `safe` parameter. The additional
ASCII characters that should not be encoded.

``` python
from urllib.parse import quote
quote("foo = bar + 5", safe = "+")
#> 'foo%20%3D%20bar%20+%205'
```

``` r
url_encoder("foo = bar + 5", safe = "+")
#> [1] "foo%20%3D%20bar%20+%205"
```

``` r

url <- "http://example.com"
set_scheme(url, "https") |>
  set_port(1234L) |>
  set_path("foo/bar") |>
  set_query("baz") |>
  set_fragment("quux")
#> [1] "https://example.com:1234/foo/bar?baz#quux"

url_modify(url, scheme = "https", port = 1234, path = "foo/bar", query = "baz", fragment = "quux")
#> [1] "https://example.com:1234/foo/bar?baz#quux"
```

Note: it is faster to use `url_modify` rather than piping the `set_*`
functions. This is because `urlparse` has to parse the url within each
`set_*` to modify the url.

``` r
url <- "http://example.com"
bench::mark(
  piping = {set_scheme(url, "https") |>
  set_port(1234L) |>
  set_path("foo/bar") |>
  set_query("baz") |>
  set_fragment("quux")},
  single_function = url_modify(url, scheme = "https", port = 1234, path = "foo/bar", query = "baz", fragment = "quux")
)
#> # A tibble: 2 × 6
#>   expression           min   median `itr/sec` mem_alloc `gc/sec`
#>   <bch:expr>      <bch:tm> <bch:tm>     <dbl> <bch:byt>    <dbl>
#> 1 piping            5.25µs   5.82µs   169955.        0B     17.0
#> 2 single_function   1.68µs   1.97µs   467288.        0B      0
```

## Benchmark:

### Parsing URL:

``` r
url <- "https://user:pass@host.com:8000/path?query=1#fragment"
(bm <- bench::mark(
  urlparse = urlparse::url_parse(url),
  httr2 = httr2::url_parse(url),
  curl = curl::curl_parse_url(url),
  urltools = urltools::url_parse(url),
  check = F
))
#> # A tibble: 4 × 6
#>   expression      min   median `itr/sec` mem_alloc `gc/sec`
#>   <bch:expr> <bch:tm> <bch:tm>     <dbl> <bch:byt>    <dbl>
#> 1 urlparse     1.76µs   1.97µs   480787.        0B      0  
#> 2 httr2       64.86µs  70.44µs    13855.   560.9KB     19.6
#> 3 curl        26.94µs  29.36µs    32701.   48.78KB     13.1
#> 4 urltools   123.49µs 133.13µs     7159.    2.17MB     23.2

show_relative(bm)
#> # A tibble: 4 × 6
#>   expression   min median `itr/sec` mem_alloc `gc/sec`
#>   <bch:expr> <dbl>  <dbl>     <dbl>     <dbl>    <dbl>
#> 1 urlparse     1      1       67.2        NaN      NaN
#> 2 httr2       36.8   35.8      1.94       Inf      Inf
#> 3 curl        15.3   14.9      4.57       Inf      Inf
#> 4 urltools    70.0   67.6      1          Inf      Inf

ggplot2::autoplot(bm)
#> Loading required namespace: tidyr
```

<img src="man/figures/README-benchmark-1.png" width="100%" />

### Encoding URL:

Note: `urltools` encode special characters to lower case hex i.e.: “?”
-\> “%3f” instead of “%3F”

``` r
string <- "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-._~`!@#$%^&*()=+[{]}\\|;:'\",<>/? "
(bm <- bench::mark(
  urlparse = urlparse::url_encoder(string),
  curl = curl::curl_escape(string),
  urltools = urltools::url_encode(string),
  base = URLencode(string, reserved = T),
  check = F
))
#> # A tibble: 4 × 6
#>   expression      min   median `itr/sec` mem_alloc `gc/sec`
#>   <bch:expr> <bch:tm> <bch:tm>     <dbl> <bch:byt>    <dbl>
#> 1 urlparse     1.48µs   1.64µs   592810.      208B      0  
#> 2 curl          2.3µs   2.42µs   402908.    3.06KB      0  
#> 3 urltools     2.38µs   2.62µs   376109.    2.48KB      0  
#> 4 base        80.32µs  87.04µs    11080.   28.59KB     23.6

show_relative(bm)
#> # A tibble: 4 × 6
#>   expression   min median `itr/sec` mem_alloc `gc/sec`
#>   <bch:expr> <dbl>  <dbl>     <dbl>     <dbl>    <dbl>
#> 1 urlparse    1      1         53.5       1        NaN
#> 2 curl        1.56   1.48      36.4      15.0      NaN
#> 3 urltools    1.61   1.60      33.9      12.2      NaN
#> 4 base       54.4   53.1        1       141.       Inf

ggplot2::autoplot(bm)
```

<img src="man/figures/README-benchmark_encode_small-1.png" width="100%" />

``` r
string <- "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-._~`!@#$%^&*()=+[{]}\\|;:'\",<>/? "
url <- paste0(sample(strsplit(string, "")[[1]], 1e4, replace = TRUE), collapse = "")
(bm <- bench::mark(
  urlparse = urlparse::url_encoder(url),
  curl = curl::curl_escape(url),
  urltools = urltools::url_encode(url),
  base = URLencode(url, reserved = T, repeated = T),
  check = F,
  filter_gc = F
))
#> # A tibble: 4 × 6
#>   expression      min   median `itr/sec` mem_alloc `gc/sec`
#>   <bch:expr> <bch:tm> <bch:tm>     <dbl> <bch:byt>    <dbl>
#> 1 urlparse    85.94µs  87.53µs    11280.    15.7KB     0   
#> 2 curl        93.28µs  94.75µs    10142.        0B     0   
#> 3 urltools   241.33µs 243.75µs     3999.    15.7KB     0   
#> 4 base         6.74ms   6.94ms      141.   332.9KB     7.95

show_relative(bm)
#> # A tibble: 4 × 6
#>   expression   min median `itr/sec` mem_alloc `gc/sec`
#>   <bch:expr> <dbl>  <dbl>     <dbl>     <dbl>    <dbl>
#> 1 urlparse    1      1         80.0       Inf      NaN
#> 2 curl        1.09   1.08      71.9       NaN      NaN
#> 3 urltools    2.81   2.78      28.4       Inf      NaN
#> 4 base       78.4   79.2        1         Inf      Inf

ggplot2::autoplot(bm)
```

<img src="man/figures/README-benchmark_encode_large-1.png" width="100%" />
