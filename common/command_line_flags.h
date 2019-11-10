#ifndef COMMAND_LINE_FLAGS_H_
#define COMMAND_LINE_FLAGS_H_

#include <cstring>
#include <map>
#include <sstream>
#include <string>

#include "types.h"

template <typename T>
class Flag {
 public:
  std::string name, remark = "";
  T *data;

  Flag(const std::string &name, T *data)
      : name(name), data(data), default_value(*data) {}
  Flag(const std::string &name, T *data, const std::string &remark)
      : name(name), data(data), default_value(*data), remark(remark) {}

  bool Parse(const std::string &str) {
    try {
      *data = ::Parse<T>(str);
    } catch (...) {
      return false;
    }
    return true;
  }

  std::string GetHelpInfo() const {
    return "--" + name + "=" + ToString(default_value) + "\t" +
           GetTypeString<T>() + "\t" + remark;
  }

  std::string GetDataInfo() const {
    return name + ": [" + ToString(*data) + "]";
  }

 private:
  T default_value;
};

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
    auto ret = ParseRecursively(dic, flags...);
    if (!ret) {
      puts("Flags:");
      PrintHelpInfoRecursively(flags...);
    } else {
      PrintDataInfoRecursively(flags...);
    }
    return ret;
  }

 private:
  template <typename T, typename... Ts>
  static bool ParseRecursively(std::map<std::string, std::string> &dic,
                               Flag<T> flag, Flag<Ts>... flags) {
    if (auto iter = dic.find(flag.name); iter != dic.end()) {
      if (!flag.Parse(iter->second)) return false;
      dic.erase(iter);
    }
    return ParseRecursively(dic, flags...);
  }

  inline static bool ParseRecursively(
      const std::map<std::string, std::string> &dic) {
    return dic.empty();
  }

  template <typename T, typename... Ts>
  static void PrintHelpInfoRecursively(Flag<T> flag, Flag<Ts>... flags) {
    printf("\t%s\n", flag.GetHelpInfo().c_str());
    PrintHelpInfoRecursively(flags...);
  }

  inline static void PrintHelpInfoRecursively() {}

  template <typename T, typename... Ts>
  static void PrintDataInfoRecursively(Flag<T> flag, Flag<Ts>... flags) {
    printf("%s\n", flag.GetDataInfo().c_str());
    PrintDataInfoRecursively(flags...);
  }

  inline static void PrintDataInfoRecursively() {}
};

#endif