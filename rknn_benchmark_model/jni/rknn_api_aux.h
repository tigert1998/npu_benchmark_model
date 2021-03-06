#ifndef RKNN_API_AUX_
#define RKNN_API_AUX_

#include <iostream>
#include <string>
#include <vector>

#include "rknn_api.h"
#include "stat.h"

std::ostream &operator<<(std::ostream &os, const rknn_tensor_format &);

std::ostream &operator<<(std::ostream &os, const rknn_tensor_type &);

std::ostream &operator<<(std::ostream &os, const rknn_tensor_qnt_type &);

std::ostream &operator<<(std::ostream &os, const rknn_tensor_attr &);

std::ostream &operator<<(std::ostream &os, const rknn_devices_id &);

std::ostream &operator<<(std::ostream &os, const rknn_sdk_version &);

struct PerfDetailTable {
  std::vector<std::string> titles;
  std::vector<std::vector<std::string>> cells;
  std::vector<Stat<double>> stats;

  PerfDetailTable() = default;
  explicit PerfDetailTable(const std::string &perf_detail);

  void Merge(const PerfDetailTable &);
};

std::ostream &operator<<(std::ostream &os, const PerfDetailTable &);

std::string RknnReturnCodeToString(int32_t ret);

class RknnError : public std::exception {
 private:
  std::int32_t code_;
  std::string call_func_str_;

 public:
  RknnError(std::int32_t code, std::string call_func_str);

  std::int32_t code() const;

  const char *what() const noexcept;
};

#endif