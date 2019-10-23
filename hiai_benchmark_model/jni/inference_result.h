#ifndef INFERENCE_RESULT_H_
#define INFERENCE_RESULT_H_

#include <vector>

struct InferenceResult {
  double time_ms;
  std::vector<std::vector<float>> data;
};

#endif