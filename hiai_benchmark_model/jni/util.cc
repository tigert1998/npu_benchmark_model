#include "util.h"

bool FileExists(const std::string &path) {
  FILE *fp = fopen(path.c_str(), "r+");
  if (fp == nullptr) return false;
  fclose(fp);
  return true;
}