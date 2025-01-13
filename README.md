
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
#> 1 piping            5.37µs    5.9µs   167991.        0B        0
#> 2 single_function   1.64µs    1.8µs   519161.        0B        0
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
#> 1 urlparse     1.72µs   1.89µs   494724.        0B      0  
#> 2 httr2       64.98µs  68.59µs    14019.   560.9KB     17.5
#> 3 curl        27.27µs  28.54µs    34199.   48.78KB     13.7
#> 4 urltools   124.52µs 130.13µs     7460.    2.17MB     20.9

show_relative(bm)
#> # A tibble: 4 × 6
#>   expression   min median `itr/sec` mem_alloc `gc/sec`
#>   <bch:expr> <dbl>  <dbl>     <dbl>     <dbl>    <dbl>
#> 1 urlparse     1      1       66.3        NaN      NaN
#> 2 httr2       37.7   36.4      1.88       Inf      Inf
#> 3 curl        15.8   15.1      4.58       Inf      Inf
#> 4 urltools    72.3   69.0      1          Inf      Inf

ggplot2::autoplot(bm)
#> Loading required namespace: tidyr
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
#> 1 urlparse      19.9µs   21.4µs    44755.      200B    17.9 
#> 2 urlparse_v2   10.5µs   11.2µs    87440.     4.3KB     8.74
#> 3 httr2        452.8µs  473.1µs     2060.    23.6KB    21.3 
#> 4 curl         190.5µs  201.4µs     4881.        0B     9.64
#> 5 urltools     130.3µs  136.7µs     7196.        0B    12.3

show_relative(bm)
#> # A tibble: 5 × 6
#>   expression    min median `itr/sec` mem_alloc `gc/sec`
#>   <bch:expr>  <dbl>  <dbl>     <dbl>     <dbl>    <dbl>
#> 1 urlparse     1.89   1.92     21.7        Inf     2.05
#> 2 urlparse_v2  1      1        42.4        Inf     1   
#> 3 httr2       43.0   42.3       1          Inf     2.44
#> 4 curl        18.1   18.0       2.37       NaN     1.10
#> 5 urltools    12.4   12.2       3.49       NaN     1.41

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
#> 1 urlparse     1.52µs   1.64µs   598950.      208B     0   
#> 2 curl          2.3µs   2.42µs   407439.    3.06KB     0   
#> 3 urltools     2.38µs   2.62µs   376010.    2.48KB     0   
#> 4 base        78.76µs  81.88µs    12031.   28.59KB     8.19

show_relative(bm)
#> # A tibble: 4 × 6
#>   expression   min median `itr/sec` mem_alloc `gc/sec`
#>   <bch:expr> <dbl>  <dbl>     <dbl>     <dbl>    <dbl>
#> 1 urlparse    1      1         49.8       1        NaN
#> 2 curl        1.51   1.47      33.9      15.0      NaN
#> 3 urltools    1.57   1.60      31.3      12.2      NaN
#> 4 base       51.9   49.9        1       141.       Inf

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
#> 1 urlparse    85.36µs  86.55µs    11420.    15.7KB     0   
#> 2 curl        92.05µs  93.69µs    10521.        0B     0   
#> 3 urltools   244.03µs 245.55µs     4047.    15.7KB     0   
#> 4 base         6.57ms   6.73ms      142.   332.2KB     9.84

show_relative(bm)
#> # A tibble: 4 × 6
#>   expression   min median `itr/sec` mem_alloc `gc/sec`
#>   <bch:expr> <dbl>  <dbl>     <dbl>     <dbl>    <dbl>
#> 1 urlparse    1      1         80.6       Inf      NaN
#> 2 curl        1.08   1.08      74.2       NaN      NaN
#> 3 urltools    2.86   2.84      28.6       Inf      NaN
#> 4 base       77.0   77.8        1         Inf      Inf

ggplot2::autoplot(bm)
```

<img src="man/figures/README-benchmark_encode_large-1.png" width="100%" />
