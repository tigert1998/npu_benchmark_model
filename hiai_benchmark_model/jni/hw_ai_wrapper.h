#ifndef HW_AI_WRAPPER_H_
#define HW_AI_WRAPPER_H_

#include <optional>
#include <string>
#include <vector>

#include "HIAIMixModel.h"

struct InferenceResult {
  double time_ms;
  std::vector<std::vector<float>> data;
};

class HwAiWrapper {
 private:
  HIAI_MixModelManager *manager = nullptr;
  HIAI_MixTensorBuffer *input_tensor = nullptr;
  HIAI_MixTensorBuffer *output_tensor = nullptr;
  HIAI_MixModelBuffer *model_buffer = nullptr;
  HIAI_MixModelTensorInfo *model_tensor_info = nullptr;

 public:
  std::string GetTfVersion() const;

  int LoadModelFromFileSync(const std::string &offline_model_name,
                            const std::string &offline_model_path,
                            bool mix_flag);

  std::optional<InferenceResult> RunModelSync(
      const std::string &offline_model_name,
      const std::vector<std::vector<float>> &data_buff);

  bool ModelCompatibilityProcessFromFile(std::string online_model_path,
                                         std::string online_model_parameter,
                                         std::string framework,
                                         std::string offline_model_path,
                                         bool mix_flag);

  std::vector<std::vector<float>> GenerateCnnRandomInput();
};

#endif