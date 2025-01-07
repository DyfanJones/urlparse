
<!-- README.md is generated from README.Rmd. Please edit that file -->

# urlparse

<!-- badges: start -->

[![CRAN
status](https://www.r-pkg.org/badges/version/urlparse)](https://CRAN.R-project.org/package=urlparse)
[![R-CMD-check](https://github.com/DyfanJones/urlparse/actions/workflows/R-CMD-check.yaml/badge.svg)](https://github.com/DyfanJones/urlparse/actions/workflows/R-CMD-check.yaml)
[![Codecov test
coverage](https://codecov.io/gh/DyfanJones/urlparse/graph/badge.svg)](https://app.codecov.io/gh/DyfanJones/urlparse)
[![urlparse status
badge](https://dyfanjones.r-universe.dev/urlparse/badges/version)](https://dyfanjones.r-universe.dev/urlparse)
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
#> 1 piping            5.29µs   5.86µs   169576.        0B        0
#> 2 single_function   1.64µs    1.8µs   507863.        0B        0
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
#> 1 urlparse     1.68µs   1.84µs   503156.        0B      0  
#> 2 httr2       64.86µs  68.59µs    14312.   560.9KB     17.4
#> 3 curl        27.22µs  28.54µs    34390.   48.78KB     13.8
#> 4 urltools   124.35µs 129.03µs     7604.    2.17MB     20.9

show_relative(bm)
#> # A tibble: 4 × 6
#>   expression   min median `itr/sec` mem_alloc `gc/sec`
#>   <bch:expr> <dbl>  <dbl>     <dbl>     <dbl>    <dbl>
#> 1 urlparse     1      1       66.2        NaN      NaN
#> 2 httr2       38.6   37.2      1.88       Inf      Inf
#> 3 curl        16.2   15.5      4.52       Inf      Inf
#> 4 urltools    74.0   69.9      1          Inf      Inf

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
#> 1 urlparse     1.48µs   1.56µs   623378.      208B     0   
#> 2 curl          2.3µs   2.42µs   399842.    3.06KB     0   
#> 3 urltools     2.42µs   2.67µs   370964.    2.48KB     0   
#> 4 base        79.09µs  83.15µs    11703.   28.59KB     8.24

show_relative(bm)
#> # A tibble: 4 × 6
#>   expression   min median `itr/sec` mem_alloc `gc/sec`
#>   <bch:expr> <dbl>  <dbl>     <dbl>     <dbl>    <dbl>
#> 1 urlparse    1      1         53.3       1        NaN
#> 2 curl        1.56   1.55      34.2      15.0      NaN
#> 3 urltools    1.64   1.71      31.7      12.2      NaN
#> 4 base       53.6   53.4        1       141.       Inf

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
#> 1 urlparse    86.06µs  87.41µs    11291.    15.8KB     0   
#> 2 curl        92.95µs  94.26µs    10209.        0B     0   
#> 3 urltools    238.7µs 244.16µs     3950.    15.8KB     0   
#> 4 base         6.72ms   6.84ms      141.   333.2KB     9.91

show_relative(bm)
#> # A tibble: 4 × 6
#>   expression   min median `itr/sec` mem_alloc `gc/sec`
#>   <bch:expr> <dbl>  <dbl>     <dbl>     <dbl>    <dbl>
#> 1 urlparse    1      1         80.2       Inf      NaN
#> 2 curl        1.08   1.08      72.5       NaN      NaN
#> 3 urltools    2.77   2.79      28.1       Inf      NaN
#> 4 base       78.1   78.2        1         Inf      Inf

ggplot2::autoplot(bm)
```

<img src="man/figures/README-benchmark_encode_large-1.png" width="100%" />
