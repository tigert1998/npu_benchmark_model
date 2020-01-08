#ifndef UTIL_H_
#define UTIL_H_

#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

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

void Timeout(const std::function<void(void)> &task, double max_secs);

std::vector<std::string> Split(const std::string &sep, const std::string &str);

std::string Trim(const std::string &str);

template <template <typename...> class C, typename... Args>
std::string Join(const std::string &sep, const C<Args...> &container) {
  if (container.begin() == container.end()) return "";
  std::string ans = std::string(*container.begin());
  for (auto iter = std::next(container.begin()); iter != container.end();
       iter = std::next(iter)) {
    ans += sep + std::string(*iter);
  }
  return ans;
}

template <typename F, template <typename...> class C, typename... Args>
C<Args...> Filter(const F &func, const C<Args...> &container) {
  C<Args...> ans;
  std::copy_if(container.begin(), container.end(), std::back_inserter(ans),
               func);
  return ans;
}

template <typename F, template <typename...> class C, typename... Args>
auto Map(const F &func, const C<Args...> &container) {
  C<std::invoke_result_t<F, typename C<Args...>::value_type>> ans;
  std::transform(container.begin(), container.end(), std::back_inserter(ans),
                 func);
  return ans;
}

#endif