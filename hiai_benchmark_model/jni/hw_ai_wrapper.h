#ifndef HW_AI_WRAPPER_H_
#define HW_AI_WRAPPER_H_

#include <optional>
#include <string>
#include <vector>

#include "HIAIMixModel.h"

class HwAiWrapper {
 private:
  HIAI_MixModelManager *manager = nullptr;
  HIAI_MixTensorBuffer *input_tensor = nullptr;
  HIAI_MixTensorBuffer *output_tensor = nullptr;
  HIAI_MixModelBuffer *model_buffer = nullptr;
  HIAI_MixModelTensorInfo *model_tensor_info = nullptr;

 public:
  int LoadModelFromFileSync(const std::string &model_name,
                            const std::string &model_path, bool mix_flag);

  std::optional<std::vector<std::vector<float>>> RunModelSync(
      const std::string &model_name,
      const std::vector<std::vector<float>> &data_buff);

  bool ModelCompatibilityProcessFromFile(std::string online_model,
                                         std::string online_model_parameter,
                                         std::string framework,
                                         std::string offline_model,
                                         bool mix_flag);
};

#endif