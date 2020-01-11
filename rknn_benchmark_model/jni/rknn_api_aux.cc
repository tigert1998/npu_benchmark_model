#include "rknn_api_aux.h"

#include "util.h"

std::string RknnReturnCodeToString(int32_t ret) {
#define CASE(enum_item) \
  case enum_item: {     \
    return #enum_item;  \
  }

  switch (ret) {
    CASE(RKNN_SUCC)
    CASE(RKNN_ERR_FAIL)
    CASE(RKNN_ERR_TIMEOUT)
    CASE(RKNN_ERR_DEVICE_UNAVAILABLE)
    CASE(RKNN_ERR_MALLOC_FAIL)
    CASE(RKNN_ERR_PARAM_INVALID)
    CASE(RKNN_ERR_MODEL_INVALID)
    CASE(RKNN_ERR_CTX_INVALID)
    CASE(RKNN_ERR_INPUT_INVALID)
    CASE(RKNN_ERR_OUTPUT_INVALID)
    CASE(RKNN_ERR_DEVICE_UNMATCH)
    CASE(RKNN_ERR_INCOMPATILE_PRE_COMPILE_MODEL)
  }
  return std::to_string(ret);

#undef CASE
}

RknnError::RknnError(std::int32_t code, std::string call_func_str)
    : code_(code), call_func_str_(call_func_str) {}

std::int32_t RknnError::code() const { return code_; }

const char *RknnError::what() const noexcept {
  return (call_func_str_ + " = " + RknnReturnCodeToString(code_)).c_str();
}

#define CASE(enum_item) \
  case enum_item: {     \
    os << #enum_item;   \
    break;              \
  }

std::ostream &operator<<(std::ostream &os, const rknn_tensor_format &data) {
  switch (data) {
    CASE(RKNN_TENSOR_NCHW)
    CASE(RKNN_TENSOR_NHWC)
    CASE(RKNN_TENSOR_FORMAT_MAX)
  }
  return os;
}

std::ostream &operator<<(std::ostream &os, const rknn_tensor_type &data) {
  switch (data) {
    CASE(RKNN_TENSOR_FLOAT32)
    CASE(RKNN_TENSOR_FLOAT16)
    CASE(RKNN_TENSOR_INT8)
    CASE(RKNN_TENSOR_UINT8)
    CASE(RKNN_TENSOR_INT16)
    CASE(RKNN_TENSOR_TYPE_MAX)
  }
  return os;
}

std::ostream &operator<<(std::ostream &os, const rknn_tensor_qnt_type &data) {
  switch (data) {
    CASE(RKNN_TENSOR_QNT_NONE)
    CASE(RKNN_TENSOR_QNT_DFP)
    CASE(RKNN_TENSOR_QNT_AFFINE_ASYMMETRIC)
    CASE(RKNN_TENSOR_QNT_MAX)
  }
  return os;
}

#define OS_DATA(member) os << "\t." #member " = " << data.member << ",\n"

#define OS_DATA_VALUE(member, value) os << "\t." #member " = " << value << ",\n"

std::ostream &operator<<(std::ostream &os, const rknn_tensor_attr &data) {
  os << "{\n";
  OS_DATA(index);
  OS_DATA(n_dims);
  OS_DATA_VALUE(dims,
                std::vector<uint32_t>(data.dims, data.dims + data.n_dims));
  OS_DATA(name);

  OS_DATA(n_elems);
  OS_DATA(size);

  OS_DATA(fmt);
  OS_DATA(type);
  OS_DATA(qnt_type);
  OS_DATA_VALUE(fl, static_cast<int32_t>(data.fl));
  OS_DATA(zp);
  OS_DATA(scale);
  os << "}";
  return os;
}

std::ostream &operator<<(std::ostream &os, const rknn_devices_id &data) {
  os << "{\n";
  OS_DATA(n_devices);
  OS_DATA_VALUE(types, std::vector<const char *>(data.types,
                                                 data.types + data.n_devices));
  OS_DATA_VALUE(ids,
                std::vector<const char *>(data.ids, data.ids + data.n_devices));
  os << "}";
  return os;
}

std::ostream &operator<<(std::ostream &os, const rknn_sdk_version &data) {
  os << "{\n";
  OS_DATA(api_version);
  OS_DATA(drv_version);
  os << "}";
  return os;
}

PerfDetailTable::PerfDetailTable(const std::string &perf_detail) {
  auto lines = Filter([](const std::string &line) { return !line.empty(); },
                      Map(Trim, Split("\n", perf_detail)));
  titles = Map(Trim, Split(":", lines[0]));
  titles.pop_back();

  cells.reserve(lines.size() - 1);
  for (int i = 1; i < lines.size(); i++) {
    cells.push_back(
        Filter([](const std::string &line) { return !line.empty(); },
               Map(Trim, Split(" ", lines[i]))));
  }

  // remove Time(us)
  stats.resize(cells.size());
  titles.pop_back();
  for (int i = 0; i < cells.size(); i++) {
    double time_us = std::stof(cells[i].back());
    stats[i].UpdateStat(time_us);
    cells[i].pop_back();
  }
}

void PerfDetailTable::Merge(const PerfDetailTable &perf_detail) {
  ASSERT(cells.size() == perf_detail.cells.size());
  for (int i = 0; i < cells.size(); i++) {
    // assert id equation
    ASSERT(cells[i][0] == perf_detail.cells[i][0]);
    ASSERT(perf_detail.stats[i].count() == 1);
    stats[i].UpdateStat(perf_detail.stats[i].first());
  }
}

std::ostream &operator<<(std::ostream &os, const PerfDetailTable &table) {
  for (int i = 0; i < table.titles.size(); i++) {
    os << (i == 0 ? "" : ",") << table.titles[i];
  }
  os << "," << Join(",", std::vector({"avg(us)", "min", "max", "std"}));
  os << std::endl;

  for (int i = 0; i < table.cells.size(); i++) {
    for (int j = 0; j < table.cells[i].size(); j++) {
      os << (j == 0 ? "" : ",") << table.cells[i][j];
    }
    os << ","
       << Join(",",
               Map([](double x) -> std::string { return std::to_string(x); },
                   std::vector{table.stats[i].avg(), table.stats[i].min(),
                               table.stats[i].max(),
                               table.stats[i].std_deviation()}));
    os << std::endl;
  }
  return os;
}