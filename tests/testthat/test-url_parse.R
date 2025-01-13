test_that("test basic url", {
  url <- 'https://www.example.com'
  actual <- url_parse(url)
  expect_equal(actual, list(
    scheme = "https",
    user = "",
    password = "",
    host = "www.example.com",
    port = "",
    path = "",
    raw_path = "",
    query = list(),
    raw_query = "",
    fragment = ""
  ))
})


test_that("test complex url", {
  url <- "https://www.google.com/maps/place/Pennsylvania+Station/@40.7519848,-74.0015045,14.7z/data=!4m5!3m4!1s0x89c259ae15b2adcb:0x7955420634fd7eba!8m2!3d40.750568!4d-73.993519"
  actual <- url_parse(url)
  expect_equal(actual, list(
    scheme = "https",
    user = "",
    password = "",
    host = "www.google.com",
    port = "",
    path = "/maps/place/Pennsylvania+Station/@40.7519848,-74.0015045,14.7z/data=!4m5!3m4!1s0x89c259ae15b2adcb:0x7955420634fd7eba!8m2!3d40.750568!4d-73.993519",
    raw_path = "/maps/place/Pennsylvania+Station/@40.7519848,-74.0015045,14.7z/data=!4m5!3m4!1s0x89c259ae15b2adcb:0x7955420634fd7eba!8m2!3d40.750568!4d-73.993519",
    query = list(),
    raw_query = "",
    fragment = ""
  ))
})

test_that("test username and password", {
  url <- "https://user_1:password_1@example.org:8080/dir/../api?q=1#frag"
  actual <- url_parse(url)
  expect_equal(actual, list(
    scheme = "https",
    user = "user_1",
    password = "password_1",
    host = "example.org",
    port = "8080",
    path = "/dir/../api",
    raw_path = "",
    query = list(q = "1"),
    raw_query = "q=1",
    fragment = "frag"
  ))
})

test_that("test encoded path", {
  url <- 'https://www.example.com:8080/search%3D1%2B3'
  actual <- url_parse(url)
  expect_equal(actual, list(
    scheme = "https",
    user = "",
    password = "",
    host = "www.example.com",
    port = "8080",
    path = "/search=1+3",
    raw_path = "/search%3D1%2B3",
    query = list(),
    raw_query = "",
    fragment = ""
  ))
})


test_that("test utf-8", {
  url <- "https://www.google.co.jp/search?q=\u30c9\u30a4\u30c4"
  actual <- url_parse(url)
  expect_equal(actual, list(
    scheme = "https",
    user = "",
    password = "",
    host = "www.google.co.jp",
    port = "",
    path = "/search",
    raw_path = "",
    query = list(q= "\u30c9\u30a4\u30c4"),
    raw_query = "q=%E3%83%89%E3%82%A4%E3%83%84",
    fragment = ""
  ))
})

test_that("test encoded query", {
  url <- 'https://www.example.com:8080?var1=foo&var2=ba r&var3=baz+larry'
  actual <- url_parse(url)
  expect_equal(actual, list(
    scheme = "https",
    user = "",
    password = "",
    host = "www.example.com",
    port = "8080",
    path = "",
    raw_path = "",
    query = list(var1 = "foo", var2 = "ba r", var3 = "baz+larry"),
    raw_query = "var1=foo&var2=ba%20r&var3=baz%2Blarry",
    fragment = ""
  ))
})

test_that("test user, password, host combinations", {
  url <- 'https://user:password@example.com:8080'
  actual <- url_parse(url)
  expect_equal(actual, list(
    scheme = "https",
    user = "user",
    password = "password",
    host = "example.com",
    port = "8080",
    path = "",
    raw_path = "",
    query = list(),
    raw_query = "",
    fragment = ""
  ))

  url <- 'https://user:password@example.com'
  actual <- url_parse(url)
  expect_equal(actual, list(
    scheme = "https",
    user = "user",
    password = "password",
    host = "example.com",
    port = "",
    path = "",
    raw_path = "",
    query = list(),
    raw_query = "",
    fragment = ""
  ))

  url <- 'https://user@example.com:8080'
  actual <- url_parse(url)
  expect_equal(actual, list(
    scheme = "https",
    user = "user",
    password = "",
    host = "example.com",
    port = "8080",
    path = "",
    raw_path = "",
    query = list(),
    raw_query = "",
    fragment = ""
  ))

  url <- 'https://user@example.com'
  actual <- url_parse(url)
  expect_equal(actual, list(
    scheme = "https",
    user = "user",
    password = "",
    host = "example.com",
    port = "",
    path = "",
    raw_path = "",
    query = list(),
    raw_query = "",
    fragment = ""
  ))
})


test_that("check url_parse_v2 parse vectorised urls", {
  urls <- c(
    "https://www.example.com",
    "https://www.google.com/maps/place/Pennsylvania+Station/@40.7519848,-74.0015045,14.7z/data=!4m5!3m4!1s0x89c259ae15b2adcb:0x7955420634fd7eba!8m2!3d40.750568!4d-73.993519",
    "https://user_1:password_1@example.org:8080/dir/../api?q=1#frag",
    "https://user:password@example.com",
    "https://www.example.com:8080/search%3D1%2B3",
    "https://www.google.co.jp/search?q=\u30c9\u30a4\u30c4",
    "https://www.example.com:8080?var1=foo&var2=ba r&var3=baz+larry",
    "https://user:password@example.com:8080",
    "https://user:password@example.com",
    "https://user@example.com:8080",
    "https://user@example.com"
  )
  expected <- data.frame(
    href = urls,
    scheme = rep("https", 11),
    user = c(
      "", "", "user_1", "user", "", "", "", "user", "user", "user", "user"
    ),
    password = c(
      "", "", "password_1", "password", "", "", "", "password", "password", "", ""
    ),
    host = c(
      "www.example.com", "www.google.com", "example.org", "example.com",
      "www.example.com", "www.google.co.jp", "www.example.com",
      "example.com", "example.com", "example.com", "example.com"),
    port = c("", "", "8080", "", "8080", "", "8080", "8080", "", "8080", ""),
    path = c(
      "", "/maps/place/Pennsylvania+Station/@40.7519848,-74.0015045,14.7z/data=!4m5!3m4!1s0x89c259ae15b2adcb:0x7955420634fd7eba!8m2!3d40.750568!4d-73.993519",
      "/dir/../api", "", "/search=1+3", "/search", "", "", "", "", ""),
    raw_path = c(
      "", "/maps/place/Pennsylvania+Station/@40.7519848,-74.0015045,14.7z/data=!4m5!3m4!1s0x89c259ae15b2adcb:0x7955420634fd7eba!8m2!3d40.750568!4d-73.993519",
      "", "", "/search%3D1%2B3", "", "", "", "", "", ""),
    raw_query = c(
      "", "", "q=1", "", "", "q=%E3%83%89%E3%82%A4%E3%83%84", "var1=foo&var2=ba%20r&var3=baz%2Blarry", "", "", "", ""),
    fragment = c(
      "", "", "frag", "", "", "", "", "", "", "", ""
    )
  )
  actual <- url_parse_v2(urls)
  expect_equal(actual, expected)
})
