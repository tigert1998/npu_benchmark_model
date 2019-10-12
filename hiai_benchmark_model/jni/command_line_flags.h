#ifndef COMMAND_LINE_FLAGS_H_
#define COMMAND_LINE_FLAGS_H_

#include <map>
#include <string>
#include <cstring>

template <typename T>
class Flag {
 public:
  std::string name_;
  T *data_;

  Flag(const std::string &name, T *data) : name_(name), data_(data) {}

  bool Parse(const std::string &);
};

template <>
bool Flag<std::string>::Parse(const std::string &str) {
  *data_ = str;
  return true;
}

template <>
bool Flag<int>::Parse(const std::string &str) {
  try {
    *data_ = std::stoi(str);
  } catch (...) {
    return false;
  }
  return true;
}

template <>
bool Flag<float>::Parse(const std::string &str) {
  try {
    *data_ = std::stof(str);
  } catch (...) {
    return false;
  }
  return true;
}

class Flags {
 public:
  template <typename... Ts>
  static bool Parse(int argc, char **argv, Flag<Ts>... flags) {
    std::map<std::string, std::string> dic;
    for (int i = 1; i < argc; i++) {
      char *arg = argv[i];
      if (arg[0] != '-' || arg[1] != '-') {
        return false;
      }
      std::string key, value;
      if (int eq_pos = std::string(arg).find_first_of('=');
          eq_pos == std::string::npos) {
        key = std::string(arg, 2, strlen(arg) - 2);
        value = "";
      } else {
        key = std::string(arg, 2, eq_pos - 2);
        value = std::string(arg, eq_pos + 1, strlen(arg) - eq_pos - 1);
      }
      if (dic.count(key)) {
        return false;
      }
      dic[key] = value;
    }
    return ParseRecursively(dic, flags...);
  }

 private:
  template <typename T, typename... Ts>
  static bool ParseRecursively(const std::map<std::string, std::string> &dic,
                               Flag<T> flag, Flag<Ts>... flags) {
    if (auto iter = dic.find(flag.name_); iter != dic.end()) {
      flag.Parse(iter->second);
    }
    return ParseRecursively(dic, flags...);
  }

  static bool ParseRecursively(const std::map<std::string, std::string> &dic) {
    return true;
  }
};

#endif