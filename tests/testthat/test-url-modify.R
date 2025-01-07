test_that("test modify", {
  url <- 'https:/example.com'
  expected <- "https://user:pass@example_v2.com:8080/search?var1=bob&var2=1%2B2#frag"

  url_components <- list(
    url = url,
    scheme = "https",
    host = "example_v2.com",
    user = "user",
    password = "pass",
    path = "search",
    query = list(var1 = "bob", var2 = "1+2"),
    fragment = "frag"
  )
  port <- list(8080L, 8080, "8080")
  for (pp in port) {
    url_components[["port"]] <- pp
    expect_equal(do.call(url_modify, url_components), expected)
  }

  url_components[["query"]] <- "var1=bob&var2=1%2B2"
  expect_equal(do.call(url_modify, url_components), expected)

  url_components[["query"]] <- 123
  expect_error(do.call(url_modify, url_components))

  url_components[["port"]] <- list(123)
  url_components[["query"]] <- "var1=bob&var2=1%2B2"
  expect_error(do.call(url_modify, url_components))
})

test_that("test query unexpected type", {
  url <- 'https:/example.com'
  expect_error(
    url_modify(url, query = 1)
  )
})

test_that("test query unexpected type", {
  url <- 'https:/example.com'
  expect_error(
    url_modify(url, port = list(1))
  )
})

test_that("test modifing pipe", {
  url <- 'https://example.com'
  expected <- "https://user:pass@example_v2.com:8080/search?var1=bob&var2=1%2B2#frag"

  actual <- set_scheme(url, "https") |>
    set_user("user") |>
    set_password("pass") |>
    set_host("example_v2.com") |>
    set_path("search") |>
    set_query(list(var1 = "bob", var2 = "1+2")) |>
    set_fragment("frag")


  port <- list(8080L, 8080, "8080")
  for (pp in port) {
    expect_equal(set_port(actual, pp), expected)
  }
  actual <- set_port(actual, "8080")
  expect_equal(set_query(actual, "var1=bob&var2=1%2B2"), expected)

  expect_error(set_query(actual, 1))
  expect_error(set_port(actual, list(1)))
})
