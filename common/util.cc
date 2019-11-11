#include "util.h"

bool FileExists(const std::string &path) {
  FILE *fp = fopen(path.c_str(), "r+");
  if (fp == nullptr) return false;
  fclose(fp);
  return true;
}

std::vector<std::string> Split(const std::string &str, const std::string &pat) {
  std::vector<std::string> ans;
  uint32_t pre_p = 0;
  auto p = str.find(pat);
  while (p != std::string::npos) {
    ans.push_back(str.substr(pre_p, p - pre_p));
    pre_p = p + pat.size();
    p = str.find(pat, pre_p);
  }
  ans.push_back(str.substr(pre_p));
  return ans;
}

std::string Trim(const std::string &str) {
  int l, r;
  for (l = 0; l < str.length(); l++)
    if (!isspace(str[l])) break;
  if (l >= str.length()) return "";
  for (r = str.length() - 1; r >= 0; r--)
    if (!isspace(str[r])) break;
  return str.substr(l, r - l + 1);
}