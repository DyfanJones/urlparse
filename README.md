
<!-- README.md is generated from README.Rmd. Please edit that file -->

# urlparse

<!-- badges: start -->

[![CRAN
status](https://www.r-pkg.org/badges/version/urlparse)](https://CRAN.R-project.org/package=urlparse)
[![R-CMD-check](https://github.com/DyfanJones/urlparse/actions/workflows/R-CMD-check.yaml/badge.svg)](https://github.com/DyfanJones/urlparse/actions/workflows/R-CMD-check.yaml)
[![Codecov test
coverage](https://codecov.io/gh/DyfanJones/urlparse/graph/badge.svg)](https://app.codecov.io/gh/DyfanJones/urlparse)
<!-- badges: end -->

Fast and simple url parser for R. Initially developed for the
`paws.common` package.

``` r
urlparse::url_parse("https://user:pass@host.com:8000/path?query=1#fragment")
#> $scheme
#> [1] "https"
#> 
#> $user
#> [1] "user"
#> 
#> $password
#> [1] "pass"
#> 
#> $host
#> [1] "host.com"
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
#> 1 piping            5.29µs   5.86µs   169276.        0B        0
#> 2 single_function   1.64µs    1.8µs   537573.        0B        0
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
#> 1 urlparse     1.72µs   1.93µs   480739.        0B      0  
#> 2 httr2       64.74µs  69.08µs    13961.   560.9KB     17.4
#> 3 curl        27.22µs  28.82µs    33183.   48.78KB     13.3
#> 4 urltools   125.09µs 135.77µs     7071.    2.17MB     18.8

show_relative(bm)
#> # A tibble: 4 × 6
#>   expression   min median `itr/sec` mem_alloc `gc/sec`
#>   <bch:expr> <dbl>  <dbl>     <dbl>     <dbl>    <dbl>
#> 1 urlparse     1      1       68.0        NaN      NaN
#> 2 httr2       37.6   35.9      1.97       Inf      Inf
#> 3 curl        15.8   15.0      4.69       Inf      Inf
#> 4 urltools    72.6   70.5      1          Inf      Inf

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
#> 1 urlparse     1.56µs   1.68µs   578208.      208B     0   
#> 2 curl         2.25µs   2.42µs   401982.    3.06KB     0   
#> 3 urltools     2.46µs   2.71µs   364688.    2.48KB     0   
#> 4 base        78.19µs  83.15µs    11612.   28.59KB     9.80

show_relative(bm)
#> # A tibble: 4 × 6
#>   expression   min median `itr/sec` mem_alloc `gc/sec`
#>   <bch:expr> <dbl>  <dbl>     <dbl>     <dbl>    <dbl>
#> 1 urlparse    1      1         49.8       1        NaN
#> 2 curl        1.45   1.44      34.6      15.0      NaN
#> 3 urltools    1.58   1.61      31.4      12.2      NaN
#> 4 base       50.2   49.5        1       141.       Inf

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
#> 1 urlparse    87.29µs   88.8µs    10966.    15.9KB     0   
#> 2 curl         94.3µs   95.6µs    10249.        0B     0   
#> 3 urltools   239.15µs  243.9µs     4026.    15.9KB     0   
#> 4 base         6.79ms    7.1ms      137.   334.2KB     9.95

show_relative(bm)
#> # A tibble: 4 × 6
#>   expression   min median `itr/sec` mem_alloc `gc/sec`
#>   <bch:expr> <dbl>  <dbl>     <dbl>     <dbl>    <dbl>
#> 1 urlparse    1      1         79.9       Inf      NaN
#> 2 curl        1.08   1.08      74.6       NaN      NaN
#> 3 urltools    2.74   2.75      29.3       Inf      NaN
#> 4 base       77.8   80.0        1         Inf      Inf

ggplot2::autoplot(bm)
```

<img src="man/figures/README-benchmark_encode_large-1.png" width="100%" />
