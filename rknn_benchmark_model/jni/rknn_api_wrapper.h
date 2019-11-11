#ifndef RKNN_API_WRAPPER_
#define RKNN_API_WRAPPER_

#include "rknn_api.h"

#include <string>
#include <vector>

struct InferenceResult {
  std::vector<std::vector<float>> outputs;
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
                          bool enable_op_profiling, bool debug_flag);

  ~RknnApiWrapper();

  InferenceResult Run(const std::vector<std::string> &data) const;

  std::vector<std::string> GenerateCnnRandomInput() const;
};

#endif