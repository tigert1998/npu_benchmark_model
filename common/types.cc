#include "types.h"

template <>
std::string ToString(const bool &value) {
  return value ? "true" : "false";
}

template <>
std::string Parse<std::string>(const std::string &str) {
  return str;
}

template <>
int32_t Parse<int32_t>(const std::string &str) {
  return std::stoi(str);
}

template <>
float Parse<float>(const std::string &str) {
  return std::stof(str);
}

template <>
bool Parse<bool>(const std::string &str) {
  if (str == "true") {
    return true;
  } else if (str == "false") {
    return false;
  } else {
    throw std::invalid_argument("");
  }
}