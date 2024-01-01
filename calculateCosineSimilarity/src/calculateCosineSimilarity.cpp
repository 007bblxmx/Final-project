#include <Rcpp.h>
#include <iostream>
#include <cmath>
#include <unordered_map>
#include <algorithm>
#include <numeric>

using namespace Rcpp;

// 将 R 的字符串向量转换为 C++ 的字符串向量
std::vector<std::string> asStringVector(CharacterVector words) {
  int n = words.size();
  std::vector<std::string> result(n);
  for (int i = 0; i < n; ++i) {
    result[i] = Rcpp::as<std::string>(words[i]);
  }
  return result;
}

// 将词组转换为词频向量
std::unordered_map<std::string, double> convertToWordVector(const std::vector<std::string>& words) {
  std::unordered_map<std::string, double> wordVector;
  for (const auto& word : words) {
    wordVector[word]++;
  }
  return wordVector;
}

// 计算两个词频向量之间的余弦相似度
double cosineSimilarity(const std::unordered_map<std::string, double>& vec1, const std::unordered_map<std::string, double>& vec2) {
  double dotProduct = 0.0;
  double normVec1 = 0.0;
  double normVec2 = 0.0;

  // 获取两个词频向量的所有词
  std::vector<std::string> allKeys;
  for (const auto& entry : vec1) {
    allKeys.push_back(entry.first);
  }
  for (const auto& entry : vec2) {
    allKeys.push_back(entry.first);
  }

  // 去除重复的词
  allKeys.erase(std::unique(allKeys.begin(), allKeys.end()), allKeys.end());

  // 计算余弦相似度的分子和分母
  for (const auto& word : allKeys) {
    double value1 = vec1.count(word) > 0 ? vec1.at(word) : 0.0;
    double value2 = vec2.count(word) > 0 ? vec2.at(word) : 0.0;

    dotProduct += value1 * value2;
    normVec1 += value1 * value1;
    normVec2 += value2 * value2;
  }

  // 计算余弦相似度
  return dotProduct / (std::sqrt(normVec1) * std::sqrt(normVec2));
}

// 计算多个词组与给定词组的余弦相似度，返回最大相似度及对应词组
// [[Rcpp::export]]
List
calculateMaxCosineSimilarity(CharacterVector query, ListOf<CharacterVector> wordLists) {
  std::vector<std::string> queryVec = asStringVector(query);
  std::unordered_map<std::string, double> queryWordVector = convertToWordVector(queryVec);

  int numWordLists = wordLists.size();
  double maxSimilarity = -1.0;  // 初始化为负数，确保任何合法相似度都会大于它
  std::vector<std::string> maxSimilarityWordList;
  NumericVector allSimilarities(numWordLists);

  for (int i = 0; i < numWordLists; ++i) {
    std::vector<std::string> wordList = asStringVector(wordLists[i]);  // 使用引用避免不必要的复制
    std::unordered_map<std::string, double> wordListWordVector = convertToWordVector(wordList);

    double similarity = cosineSimilarity(queryWordVector, wordListWordVector);

    // 存储所有相似度
    allSimilarities[i] = similarity;

    if (similarity > maxSimilarity) {
      maxSimilarity = similarity;
      maxSimilarityWordList = std::move(wordList);  // 移动语义避免不必要的复制
    }
  }

  return List::create(Named("maxSimilarity") = maxSimilarity,
                      Named("maxSimilarityWordList") = maxSimilarityWordList,
                      Named("allSimilarities") = allSimilarities);
}
