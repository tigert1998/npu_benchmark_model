#ifndef RKNN_API_WRAPPER_
#define RKNN_API_WRAPPER_

#include "rknn_api.h"

#include <string>
#include <vector>

template <typename T>
struct InferenceResult {
  std::vector<std::vector<T>> outputs;
  std::string perf_detail;
  double time_ms;
};

class RknnApiWrapper {
 private:
  rknn_context ctx_;
  std::string model_data_;
  int num_inputs_, num_outputs_;
  bool enable_op_profiling_;

  std::vector<rknn_tensor_attr> inputs_attrs_, outputs_attrs_;

 public:
  explicit RknnApiWrapper(const std::string &model_path,
                          bool enable_op_profiling);

  InferenceResult<float> Run(const std::vector<std::vector<float>> &data);

  std::vector<std::vector<float>> GenerateCnnRandomInput() const;
};

#endif