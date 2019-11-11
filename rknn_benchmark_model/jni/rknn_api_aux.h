#ifndef RKNN_API_AUX_
#define RKNN_API_AUX_

#include <iostream>
#include <string>
#include <vector>

#include "rknn_api.h"

std::ostream &operator<<(std::ostream &os, const rknn_tensor_format &);

std::ostream &operator<<(std::ostream &os, const rknn_tensor_type &);

std::ostream &operator<<(std::ostream &os, const rknn_tensor_qnt_type &);

std::ostream &operator<<(std::ostream &os, const rknn_tensor_attr &);

struct PerfDetailTable {
  std::vector<std::string> titles;
  std::vector<std::vector<std::string>> cells;

  explicit PerfDetailTable(const std::string &perf_detail);
};

std::ostream &operator<<(std::ostream &os, const PerfDetailTable &);

#endif