library(XML)
library(ggplot2)

args <- commandArgs(trailingOnly = TRUE)
data <- xmlParse(args[1])

for (test_case in data["//TestCase"]) {
  test_case_name <- xpathSApply(test_case, "./@name", paste)
  results <- xpathSApply(test_case, "./BenchmarkResults")
  samples <- xpathSApply(test_case, ".//sample", xmlValue)
  names <- xpathSApply(test_case, "./BenchmarkResults/@name", paste)
  names <- rep(names, each = 100)

  df <- data.frame(
    names = names,
    samples = as.numeric(samples)
  )

  p <- ggplot(df, aes(x = names, y = samples, fill = names)) +
    geom_boxplot(size = 0.5, staplewidth = 0.5) +
    theme_minimal() +
    labs(x = "method", y = "times", title = test_case_name) +
    scale_y_continuous(limits = c(0, NA))
  print(p)
  test_case_name |>
    gsub(pattern = "\\s", replacement = "_") |>
    paste(".png", sep = "") |>
    ggsave()
}
