test_that("test encoding", {
  expected <- c(
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-._~%60%21%40%23%24%25%5E%26%2A%28%29%3D%2B%5B%7B%5D%7D%5C%7C%3B%3A%27%22%2C%3C%3E%2F%3F%20",
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-._~%60%21%40%23%24%25%5E%26%2A%28%29%3D+%5B%7B%5D%7D%5C%7C%3B%3A%27%22%2C%3C%3E%2F%3F%20",
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-._~%60%21%40%23%24%25%5E%26%2A%28%29%3D%2B%5B%7B%5D%7D%5C%7C%3B%3A%27%22,<>/?%20"
  )

  string <- "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-._~`!@#$%^&*()=+[{]}\\|;:'\",<>/? "
  safe <- c("", "+", ",<>/?")
  for (i in seq_along(safe)) {
    expect_equal(url_encoder(string, safe[i]), expected[i])
  }
})

test_that("check if non-ascii characters are correctly encoded", {
  string <- "なでçãкатынü"
  actual <- url_encoder(string)
  expect_equal(
    actual,
    "%E3%81%AA%E3%81%A7%C3%A7%C3%A3%D0%BA%D0%B0%D1%82%D1%8B%D0%BD%C3%BC"
  )
})


test_that("test decoded", {
  expected <- "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-._~`!@#$%^&*()=+[{]}\\|;:'\",<>/? "
  string <- c(
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-._~%60%21%40%23%24%25%5E%26%2A%28%29%3D%2B%5B%7B%5D%7D%5C%7C%3B%3A%27%22%2C%3C%3E%2F%3F%20",
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-._~%60%21%40%23%24%25%5E%26%2A%28%29%3D+%5B%7B%5D%7D%5C%7C%3B%3A%27%22%2C%3C%3E%2F%3F%20",
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-._~%60%21%40%23%24%25%5E%26%2A%28%29%3D%2B%5B%7B%5D%7D%5C%7C%3B%3A%27%22,<>/?%20"
  )
  for (i in seq_along(string)) {
    expect_equal(url_decoder(string[i]), expected)
  }
})

test_that("test NA", {
  expect_equal(url_encoder(NA), NA_character_)
  expect_equal(url_decoder(NA), NA_character_)
})
