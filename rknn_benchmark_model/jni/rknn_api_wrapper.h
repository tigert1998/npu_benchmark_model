#ifndef RKNN_API_WRAPPER_
#define RKNN_API_WRAPPER_

#include "rknn_api.h"
#include "rknn_api_aux.h"

#include <string>
#include <vector>

struct InferenceResult {
  std::vector<std::vector<float>> outputs;
  PerfDetailTable perf_detail;
  double time_ms;
};

class RknnApiWrapper {
 private:
  rknn_context ctx_;
  std::string model_data_;
  int num_inputs_, num_outputs_;
  bool enable_op_profiling_;

  std::vector<rknn_tensor_attr> inputs_attrs_, outputs_attrs_;
  rknn_devices_id devices_;
  rknn_sdk_version sdk_version_;

 public:
  explicit RknnApiWrapper(const std::string &model_path,
                          bool enable_op_profiling, bool debug_flag);

  ~RknnApiWrapper();

  InferenceResult Run(const std::vector<std::vector<float>> &data) const;

  std::vector<std::vector<float>> GenerateCnnRandomInput() const;
};

#endif