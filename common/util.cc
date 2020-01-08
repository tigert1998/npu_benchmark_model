#include "util.h"

#include <chrono>
#include <thread>

#include <sys/types.h>
#include <unistd.h>

bool FileExists(const std::string &path) {
  FILE *fp = fopen(path.c_str(), "r+");
  if (fp == nullptr) return false;
  fclose(fp);
  return true;
}

std::vector<std::string> Split(const std::string &sep, const std::string &str) {
  std::vector<std::string> ans;
  uint32_t pre_p = 0;
  auto p = str.find(sep);
  while (p != std::string::npos) {
    ans.push_back(str.substr(pre_p, p - pre_p));
    pre_p = p + sep.size();
    p = str.find(sep, pre_p);
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

void Timeout(const std::function<void(void)> &task, double max_secs) {
  std::condition_variable cv;

  std::thread t([&]() {
    task();
    cv.notify_one();
  });

  std::mutex mtx;
  std::unique_lock lck(mtx);
  if (cv.wait_for(lck, std::chrono::microseconds(int(max_secs * 1e6))) ==
      std::cv_status::timeout) {
    std::cout << "[TIMEOUT]\n";
    exit(-1);
  }

  t.join();
}