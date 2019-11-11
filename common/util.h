#ifndef UTIL_H_
#define UTIL_H_

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#define ASSERT(value)                                                   \
  if (!static_cast<bool>(value)) {                                      \
    fprintf(stderr, "Assertion failed in \"%s\", line %d.\n", __FILE__, \
            __LINE__);                                                  \
    exit(-1);                                                           \
  }

#define LOG(value) std::cout << #value " = " << value << std::endl;

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

std::vector<std::string> Split(const std::string &str, const std::string &pat);

std::string Trim(const std::string &str);

template <typename F, template <typename...> class C, typename... Args>
C<Args...> Filter(const C<Args...> &container, const F &func) {
  C<Args...> ans;
  std::copy_if(container.begin(), container.end(), std::back_inserter(ans),
               func);
  return ans;
}

template <typename F, template <typename...> class C, typename... Args>
C<Args...> Map(const C<Args...> &container, const F &func) {
  C<Args...> ans;
  std::transform(container.begin(), container.end(), std::back_inserter(ans),
                 func);
  return ans;
}

#endif