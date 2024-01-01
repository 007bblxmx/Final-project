#' 计算最大余弦相似度
#' @param query 字符串向量，表示查询词组
#' @param wordLists 列表，包含多个字符串向量，表示待比较的词组
#' @return 列表，包含最大相似度、对应词组和所有相似度
#' @export
calculateMaxCosineSimilarity <- function(query, wordLists) {
  result <- .Call("calculateMaxCosineSimilarity", as.character(query), as.list(wordLists))
  list(
    maxSimilarity = result$maxSimilarity,
    maxSimilarityWordList = result$maxSimilarityWordList,
    allSimilarities = result$allSimilarities
  )
}

