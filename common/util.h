#ifndef UTIL_H_
#define UTIL_H_

#include <fstream>
#include <string>

#define ASSERT(value)                                                   \
  if (!static_cast<bool>(value)) {                                      \
    fprintf(stderr, "Assertion failed in \"%s\", line %d.\n", __FILE__, \
            __LINE__);                                                  \
    exit(-1);                                                           \
  }

bool FileExists(const std::string &path);

template <typename T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &vec) {
  os << "{";
  for (int i = 0; i < vec.size(); i++) {
    os << (i > 0 ? ", " : "") << vec[i];
  }
  os << "}";
  return os;
}

#endif