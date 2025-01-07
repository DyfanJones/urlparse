test_that("test build basic url", {
  expected <- 'https://www.example.com'
  actual <- url_build(list(
    scheme = "https",
    host = "www.example.com",
    port = "",
    path = "",
    raw_path = "",
    query = "",
    fragment = ""
  ))
  expect_equal(actual, expected)
  expect_equal(url_build(url_parse(expected)), expected)
})


test_that("test build complex url", {
  expected <- "https://www.google.com/maps/place/Pennsylvania+Station/@40.7519848,-74.0015045,14.7z/data=!4m5!3m4!1s0x89c259ae15b2adcb:0x7955420634fd7eba!8m2!3d40.750568!4d-73.993519"
  actual <- url_build(list(
    scheme = "https",
    user = "",
    password = "",
    host = "www.google.com",
    port = "",
    path = "/maps/place/Pennsylvania+Station/@40.7519848,-74.0015045,14.7z/data=!4m5!3m4!1s0x89c259ae15b2adcb:0x7955420634fd7eba!8m2!3d40.750568!4d-73.993519",
    query = "",
    fragment = ""
  ))
  expect_equal(actual, expected)
  expect_equal(url_build(url_parse(expected)), expected)
})

test_that("test build with username and password", {
  expected <- "https://user_1:password_1@example.org:8080/dir/../api?q=1#frag"
  actual <- url_build(list(
    scheme = "https",
    user = "user_1",
    password = "password_1",
    host = "example.org",
    port = "8080",
    path = "/dir/../api",
    raw_query = "q=1",
    fragment = "frag"
  ))
  expect_equal(actual, expected)
  expect_equal(url_build(url_parse(expected)), expected)
})

test_that("test build encoded path", {
  expected <- 'https://www.example.com:8080/search%3D1%2B3'
  actual <- url_build(list(
    scheme = "https",
    user = "",
    password = "",
    host = "www.example.com",
    port = "8080",
    path = "/search%3D1%2B3",
    query = "",
    fragment = ""
  ))
  expect_equal(actual, expected)
  expect_equal(url_build(url_parse(expected)), expected)
})


test_that("test build non-ascci path", {
  expected <- "https://www.google.co.jp/search?q=%E3%83%89%E3%82%A4%E3%83%84"
  actual <- url_build(list(
    scheme = "https",
    host = "www.google.co.jp",
    port = "",
    path = "/search",
    query = "q=%E3%83%89%E3%82%A4%E3%83%84",
    fragment = ""
  ))
  expect_equal(actual, expected)
  expect_equal(url_build(url_parse(expected)), expected)
})

test_that("test build port types", {
  expected <- 'https://www.example.com:8080?var1=foo&var2=ba%20r&var3=baz%2Blarry'
  port <- list(8080, 8080L, "8080")
  url_components <- list(
    scheme = "https",
    host = "www.example.com",
    path = "",
    query = "var1=foo&var2=ba%20r&var3=baz%2Blarry",
    fragment = ""
  )
  expect_equal(url_build(url_parse(expected)), expected)

  for (pp in port) {
    url_components[["port"]] <- pp
    expect_equal(url_build(url_components), expected)
  }

  url_components[["port"]] <- list(8080)
  expect_error(url_build(url_components))
})

test_that("test build query types", {
  expected <- 'https://www.example.com:8080?var1=foo&var2=ba%20r&var3=baz%2Blarry'
  query <- list(list(var1 = "foo", var2 = "ba r", var3 = "baz+larry"), "var1=foo&var2=ba%20r&var3=baz%2Blarry")
  url_components <- list(
    scheme = "https",
    host = "www.example.com",
    port = 8080,
    path = "",
    fragment = ""
  )
  expect_equal(url_build(url_parse(expected)), expected)
  for (qq in query) {
    url_components[["query"]] <- qq
    expect_equal(url_build(url_components), expected)
  }

  url_components[["query"]] <- 123
  expect_error(url_build(url_components))
})

