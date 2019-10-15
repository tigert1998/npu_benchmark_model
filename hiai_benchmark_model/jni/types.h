#ifndef TYPES_H_
#define TYPES_H_

#include <sstream>
#include <string>

template <typename T>
std::string ToString(const T &value) {
  std::stringstream ss;
  ss << value;
  return ss.str();
}

template <typename T>
T Parse(const std::string &str);

template <typename T>
std::string GetTypeString() {
  if constexpr (std::is_same_v<T, std::string>) {
    return "string";
  } else if constexpr (std::is_same_v<T, int32_t>) {
    return "int32_t";
  } else if constexpr (std::is_same_v<T, float>) {
    return "float";
  } else if constexpr (std::is_same_v<T, bool>) {
    return "bool";
  }
}

#endif
