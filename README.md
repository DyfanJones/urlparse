
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

Modify an `url` through piping using the `set_*` functions or using the
stand alone `url_modify` function.

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
#> 1 piping            5.29µs    5.9µs   162854.        0B     16.3
#> 2 single_function    1.6µs    1.8µs   517151.        0B     51.7
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
#> 1 urlparse     1.72µs   1.93µs   488712.        0B      0  
#> 2 httr2       22.39µs  23.86µs    40406.  571.07KB     24.3
#> 3 curl        27.06µs  29.64µs    29324.        0B     14.7
#> 4 urltools   124.44µs 132.68µs     7119.    2.18MB     23.2

show_relative(bm)
#> # A tibble: 4 × 6
#>   expression   min median `itr/sec` mem_alloc `gc/sec`
#>   <bch:expr> <dbl>  <dbl>     <dbl>     <dbl>    <dbl>
#> 1 urlparse     1      1       68.6        NaN      NaN
#> 2 httr2       13.0   12.4      5.68       Inf      Inf
#> 3 curl        15.7   15.4      4.12       NaN      Inf
#> 4 urltools    72.3   68.9      1          Inf      Inf

ggplot2::autoplot(bm)
```

<img src="man/figures/README-benchmark-1.png" width="100%" />

Since `urlpase v0.1.999+` you can use the vectorised url parser
`url_parser_v2`

``` r
urls <- c(
  "https://www.example.com",
  "https://www.google.com/maps/place/Pennsylvania+Station/@40.7519848,-74.0015045,14.7z/data=!4m5!3m4!1s0x89c259ae15b2adcb:0x7955420634fd7eba!8m2!3d40.750568!4d-73.993519",
  "https://user_1:password_1@example.org:8080/dir/../api?q=1#frag",
  "https://user:password@example.com",
  "https://www.example.com:8080/search%3D1%2B3",
  "https://www.google.co.jp/search?q=\u30c9\u30a4\u30c4",
  "https://www.example.com:8080?var1=foo&var2=ba%20r&var3=baz+larry",
  "https://user:password@example.com:8080",
  "https://user:password@example.com",
  "https://user@example.com:8080",
  "https://user@example.com"
)
(bm <- bench::mark(
  urlparse = lapply(urls, urlparse::url_parse),
  urlparse_v2 = urlparse::url_parse_v2(urls),
  httr2 =  lapply(urls, httr2::url_parse),
  curl = lapply(urls, curl::curl_parse_url),
  urltools = urltools::url_parse(urls),
  check = F
))
#> # A tibble: 5 × 6
#>   expression       min   median `itr/sec` mem_alloc `gc/sec`
#>   <bch:expr>  <bch:tm> <bch:tm>     <dbl> <bch:byt>    <dbl>
#> 1 urlparse      19.4µs   21.3µs    46214.      200B    13.9 
#> 2 urlparse_v2   10.5µs     11µs    87963.     4.3KB     0   
#> 3 httr2        171.6µs  181.6µs     5232.        0B    10.2 
#> 4 curl         188.7µs  198.4µs     4895.        0B     8.14
#> 5 urltools       130µs  142.1µs     6569.        0B    10.2

show_relative(bm)
#> # A tibble: 5 × 6
#>   expression    min median `itr/sec` mem_alloc `gc/sec`
#>   <bch:expr>  <dbl>  <dbl>     <dbl>     <dbl>    <dbl>
#> 1 urlparse     1.85   1.94      9.44       Inf      Inf
#> 2 urlparse_v2  1      1        18.0        Inf      NaN
#> 3 httr2       16.4   16.5       1.07       NaN      Inf
#> 4 curl        18.0   18.1       1          NaN      Inf
#> 5 urltools    12.4   12.9       1.34       NaN      Inf

ggplot2::autoplot(bm)
```

<img src="man/figures/README-benchmark_vectorise-1.png" width="100%" />

Note: `url_parse_v2` returns the parsed url as a `data.frame` this is
similar behaviour to `urltools` and `adaR`:

``` r
urlparse::url_parse_v2(urls)
#>                                                                                                                                                                       href
#> 1                                                                                                                                                  https://www.example.com
#> 2  https://www.google.com/maps/place/Pennsylvania+Station/@40.7519848,-74.0015045,14.7z/data=!4m5!3m4!1s0x89c259ae15b2adcb:0x7955420634fd7eba!8m2!3d40.750568!4d-73.993519
#> 3                                                                                                           https://user_1:password_1@example.org:8080/dir/../api?q=1#frag
#> 4                                                                                                                                        https://user:password@example.com
#> 5                                                                                                                              https://www.example.com:8080/search%3D1%2B3
#> 6                                                                                                                                 https://www.google.co.jp/search?q=ドイツ
#> 7                                                                                                         https://www.example.com:8080?var1=foo&var2=ba%20r&var3=baz+larry
#> 8                                                                                                                                   https://user:password@example.com:8080
#> 9                                                                                                                                        https://user:password@example.com
#> 10                                                                                                                                           https://user@example.com:8080
#> 11                                                                                                                                                https://user@example.com
#>    scheme   user   password             host port
#> 1   https                    www.example.com     
#> 2   https                     www.google.com     
#> 3   https user_1 password_1      example.org 8080
#> 4   https   user   password      example.com     
#> 5   https                    www.example.com 8080
#> 6   https                   www.google.co.jp     
#> 7   https                    www.example.com 8080
#> 8   https   user   password      example.com 8080
#> 9   https   user   password      example.com     
#> 10  https   user                 example.com 8080
#> 11  https   user                 example.com     
#>                                                                                                                                                 path
#> 1                                                                                                                                                   
#> 2  /maps/place/Pennsylvania+Station/@40.7519848,-74.0015045,14.7z/data=!4m5!3m4!1s0x89c259ae15b2adcb:0x7955420634fd7eba!8m2!3d40.750568!4d-73.993519
#> 3                                                                                                                                        /dir/../api
#> 4                                                                                                                                                   
#> 5                                                                                                                                        /search=1+3
#> 6                                                                                                                                            /search
#> 7                                                                                                                                                   
#> 8                                                                                                                                                   
#> 9                                                                                                                                                   
#> 10                                                                                                                                                  
#> 11                                                                                                                                                  
#>                                                                                                                                             raw_path
#> 1                                                                                                                                                   
#> 2  /maps/place/Pennsylvania+Station/@40.7519848,-74.0015045,14.7z/data=!4m5!3m4!1s0x89c259ae15b2adcb:0x7955420634fd7eba!8m2!3d40.750568!4d-73.993519
#> 3                                                                                                                                                   
#> 4                                                                                                                                                   
#> 5                                                                                                                                    /search%3D1%2B3
#> 6                                                                                                                                                   
#> 7                                                                                                                                                   
#> 8                                                                                                                                                   
#> 9                                                                                                                                                   
#> 10                                                                                                                                                  
#> 11                                                                                                                                                  
#>                                raw_query fragment
#> 1                                                
#> 2                                                
#> 3                                    q=1     frag
#> 4                                                
#> 5                                                
#> 6          q=%E3%83%89%E3%82%A4%E3%83%84         
#> 7  var1=foo&var2=ba%20r&var3=baz%2Blarry         
#> 8                                                
#> 9                                                
#> 10                                               
#> 11
```

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
#> 1 urlparse     1.48µs   1.64µs   581380.      208B     0   
#> 2 curl         2.25µs   2.58µs   349595.    3.03KB     0   
#> 3 urltools     2.34µs   2.54µs   381930.    2.48KB     0   
#> 4 base        78.84µs  82.33µs    11746.   28.59KB     8.25

show_relative(bm)
#> # A tibble: 4 × 6
#>   expression   min median `itr/sec` mem_alloc `gc/sec`
#>   <bch:expr> <dbl>  <dbl>     <dbl>     <dbl>    <dbl>
#> 1 urlparse    1      1         49.5       1        NaN
#> 2 curl        1.53   1.57      29.8      14.9      NaN
#> 3 urltools    1.58   1.55      32.5      12.2      NaN
#> 4 base       53.4   50.2        1       141.       Inf

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
#> 1 urlparse     86.4µs   88.3µs    10599.    15.8KB     0   
#> 2 curl         91.6µs   94.9µs    10306.        0B     0   
#> 3 urltools    241.8µs  247.8µs     3943.    15.8KB     0   
#> 4 base          6.7ms      7ms      138.   333.5KB     8.00

show_relative(bm)
#> # A tibble: 4 × 6
#>   expression   min median `itr/sec` mem_alloc `gc/sec`
#>   <bch:expr> <dbl>  <dbl>     <dbl>     <dbl>    <dbl>
#> 1 urlparse    1      1         76.8       Inf      NaN
#> 2 curl        1.06   1.08      74.7       NaN      NaN
#> 3 urltools    2.80   2.81      28.6       Inf      NaN
#> 4 base       77.6   79.3        1         Inf      Inf

ggplot2::autoplot(bm)
```

<img src="man/figures/README-benchmark_encode_large-1.png" width="100%" />
