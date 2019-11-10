#include "rknn_api_wrapper.h"

#include <exception>
#include <random>
#include <vector>

#include "util.h"

#define CHECK_RET(call_func)                                     \
  do {                                                           \
    auto ret = call_func;                                        \
    if (ret != 0) {                                              \
      throw std::runtime_error(std::string(#call_func) + " = " + \
                               std::to_string(ret));             \
    }                                                            \
  } while (0)

RknnApiWrapper::RknnApiWrapper(const std::string &model_path,
                               bool enable_op_profiling)
    : enable_op_profiling_(enable_op_profiling) {
  FILE *file = fopen(model_path.c_str(), "rb");
  if (file == nullptr) {
    throw std::runtime_error("invalid model_path");
  }
  fseek(file, 0, SEEK_END);
  model_data_.resize(ftell(file));
  fseek(file, 0, SEEK_SET);
  if (model_data_.size() !=
      fread(model_data_.data(), 1, model_data_.size(), file)) {
    throw std::runtime_error("fail to read model at " + model_path);
  }

  auto init_flag = RKNN_FLAG_PRIOR_HIGH |
                   (enable_op_profiling ? RKNN_FLAG_COLLECT_PERF_MASK : 0);
  CHECK_RET(
      rknn_init(&ctx_, model_data_.data(), model_data_.size(), init_flag));

  rknn_input_output_num io_num;
  CHECK_RET(rknn_query(ctx_, RKNN_QUERY_IN_OUT_NUM, &io_num, sizeof(io_num)));
  num_inputs_ = io_num.n_input;
  num_outputs_ = io_num.n_output;

  inputs_attrs_.reserve(num_inputs_);
  for (uint32_t i = 0; i < num_inputs_; i++) {
    inputs_attrs_.push_back({.index = i});
    CHECK_RET(rknn_query(ctx_, RKNN_QUERY_INPUT_ATTR, inputs_attrs_.data() + i,
                         sizeof(rknn_tensor_attr)));
  }

  outputs_attrs_.reserve(num_outputs_);
  for (uint32_t i = 0; i < num_outputs_; i++) {
    outputs_attrs_.push_back({.index = i});
    CHECK_RET(rknn_query(ctx_, RKNN_QUERY_OUTPUT_ATTR,
                         outputs_attrs_.data() + i, sizeof(rknn_tensor_attr)));
  }
}

InferenceResult<float> RknnApiWrapper::Run(
    const std::vector<std::vector<float>> &data) {
  ASSERT(data.size() == num_inputs_);

  std::vector<rknn_input> inputs;
  inputs.reserve(data.size());
  for (uint32_t i = 0; i < data.size(); i++) {
    inputs.push_back({.index = i,
                      .buf = (void *)(data[i].data()),
                      .size = static_cast<uint32_t>(data[i].size()),
                      .pass_through = false,
                      .type = RKNN_TENSOR_FLOAT32,
                      .fmt = RKNN_TENSOR_NHWC});
  }
  CHECK_RET(rknn_inputs_set(ctx_, inputs.size(), inputs.data()));
  CHECK_RET(rknn_run(ctx_, nullptr));

  InferenceResult<float> infer_res;
  infer_res.outputs.resize(num_outputs_);
  std::vector<rknn_output> outputs;
  outputs.reserve(num_outputs_);
  for (uint32_t i = 0; i < num_outputs_; i++) {
    infer_res.outputs[i].resize(outputs_attrs_[i].n_elems);
    outputs.push_back({.want_float = true,
                       .is_prealloc = true,
                       .index = i,
                       .buf = infer_res.outputs[i].data(),
                       .size = static_cast<uint32_t>(
                           infer_res.outputs[i].size() * sizeof(float))});
  }
  CHECK_RET(rknn_outputs_get(ctx_, num_outputs_, outputs.data(), nullptr));

  if (!enable_op_profiling_) {
    infer_res.perf_detail = "";
  } else {
    rknn_perf_detail perf_detail;
    CHECK_RET(rknn_query(ctx_, RKNN_QUERY_PERF_DETAIL, &perf_detail,
                         sizeof(rknn_perf_detail)));
    infer_res.perf_detail = std::string(
        perf_detail.perf_data, perf_detail.perf_data + perf_detail.data_len);
  }
  rknn_perf_run perf_run;
  CHECK_RET(
      rknn_query(ctx_, RKNN_QUERY_PERF_RUN, &perf_run, sizeof(rknn_perf_run)));
  infer_res.time_ms = perf_run.run_duration / 1e3;

  return infer_res;
}

std::vector<std::vector<float>> RknnApiWrapper::GenerateCnnRandomInput() const {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  static std::uniform_real_distribution<> dis(-1, 1);

  std::vector<std::vector<float>> res;
  res.resize(num_inputs_);
  for (int i = 0; i < num_inputs_; i++) {
    res[i].reserve(inputs_attrs_[i].n_elems);
    for (int j = 0; j < inputs_attrs_[i].n_elems; j++)
      res[i].push_back(dis(gen));
  }

  return res;
}