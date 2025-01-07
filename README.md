
<!-- README.md is generated from README.Rmd. Please edit that file -->

# urlparse

<!-- badges: start -->

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

Note: it is faster to use `url_modify` rather than then piping the
`set_*` functions. This is because `urlparse` has to parse the url
within each `set_*` to modify the url.

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
#> 1 piping            5.21µs   5.78µs   171115.        0B     17.1
#> 2 single_function   1.68µs   1.84µs   524051.        0B      0
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
#> 1 urlparse     1.76µs   1.97µs   473600.        0B      0  
#> 2 httr2       64.58µs  70.03µs    13928.   560.9KB     19.6
#> 3 curl        27.31µs  29.36µs    33024.   48.78KB     13.2
#> 4 urltools   123.82µs 134.25µs     7164.    2.17MB     23.2

show_relative(bm)
#> # A tibble: 4 × 6
#>   expression   min median `itr/sec` mem_alloc `gc/sec`
#>   <bch:expr> <dbl>  <dbl>     <dbl>     <dbl>    <dbl>
#> 1 urlparse     1      1       66.1        NaN      NaN
#> 2 httr2       36.6   35.6      1.94       Inf      Inf
#> 3 curl        15.5   14.9      4.61       Inf      Inf
#> 4 urltools    70.2   68.2      1          Inf      Inf

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
#> 1 urlparse     1.48µs    1.6µs   588255.      208B      0  
#> 2 curl         2.25µs   2.42µs   397364.    3.06KB      0  
#> 3 urltools     2.38µs   2.62µs   373601.    2.48KB      0  
#> 4 base        80.24µs  86.39µs    10099.   28.59KB     21.5

show_relative(bm)
#> # A tibble: 4 × 6
#>   expression   min median `itr/sec` mem_alloc `gc/sec`
#>   <bch:expr> <dbl>  <dbl>     <dbl>     <dbl>    <dbl>
#> 1 urlparse    1      1         58.2       1        NaN
#> 2 curl        1.53   1.51      39.3      15.0      NaN
#> 3 urltools    1.61   1.64      37.0      12.2      NaN
#> 4 base       54.4   54.0        1       141.       Inf

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
#> 1 urlparse    85.53µs  86.51µs    11349.    15.7KB     0   
#> 2 curl        92.21µs   93.6µs    10496.        0B     0   
#> 3 urltools   241.41µs 245.59µs     4029.    15.7KB     0   
#> 4 base         6.59ms   6.81ms      143.   332.6KB     7.96

show_relative(bm)
#> # A tibble: 4 × 6
#>   expression   min median `itr/sec` mem_alloc `gc/sec`
#>   <bch:expr> <dbl>  <dbl>     <dbl>     <dbl>    <dbl>
#> 1 urlparse    1      1         79.2       Inf      NaN
#> 2 curl        1.08   1.08      73.2       NaN      NaN
#> 3 urltools    2.82   2.84      28.1       Inf      NaN
#> 4 base       77.1   78.8        1         Inf      Inf

ggplot2::autoplot(bm)
```

<img src="man/figures/README-benchmark_encode_large-1.png" width="100%" />
