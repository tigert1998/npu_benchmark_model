#ifndef MODEL_MANAGER_WRAPPER_H_
#define MODEL_MANAGER_WRAPPER_H_

#include "HIAIModelManager.h"

#include <string>

#include "inference_result.h"

class ModelManagerWrapper {
 private:
  HIAI_ModelManager *model_manager = nullptr;
  HIAI_TensorBuffer *input_tensor = nullptr;
  HIAI_TensorBuffer *output_tensor = nullptr;
  HIAI_ModelBuffer *model_buffer = nullptr;

 public:
  int LoadModelFromFileSync(const std::string &offline_model_name,
                            const std::string &offline_model_path);

  bool ModelCompatibilityProcessFromFile(
      const std::string &online_model,
      const std::string &online_model_parameter, const std::string &framework,
      const std::string &offline_model_path);
};

#endif