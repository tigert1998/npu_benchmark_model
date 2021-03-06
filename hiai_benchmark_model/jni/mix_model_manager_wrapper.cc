#include "mix_model_manager_wrapper.h"

#include <android/log.h>

#include <dlfcn.h>
#include <chrono>
#include <cstring>
#include <memory>
#include <random>

#include "util.h"

#define LOG_TAG "MIX_MODEL_MANAGER_WRAPPER_MSG"

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

using std::nullopt;

int MixModelManagerWrapper::LoadModelFromFileSync(
    const std::string &offline_model_name,
    const std::string &offline_model_path, bool mix_flag) {
  LOGI("[LoadModelFromFileSync] offline_model_path = %s",
       offline_model_path.c_str());
  LOGI("[LoadModelFromFileSync] offline_model_name = %s",
       offline_model_name.c_str());

  model_buffer = HIAI_MixModelBuffer_Create_From_File(
      offline_model_name.c_str(), offline_model_path.c_str(),
      HIAI_MIX_DEVPREF_LOW, mix_flag);

  if (model_buffer == nullptr) {
    LOGI("model_buffer == nullptr");
    return -1;
  }

  HIAI_MixModelBuffer *model_buf_arr[] = {model_buffer};
  manager = HIAI_MixModelManager_Create(nullptr);

  if (manager == nullptr) {
    LOGE("create manager failed.");
    return -1;
  }

  int ret = HIAI_MixModel_LoadFromModelBuffers(manager, model_buf_arr, 1);

  LOGI("load model from file ret = %d", ret);
  if (ret == 0) {
    model_tensor_info =
        HIAI_MixModel_GetModelTensorInfo(manager, offline_model_name.c_str());
  }

  return ret;
}

std::vector<std::vector<float>>
MixModelManagerWrapper::GenerateCnnRandomInput() {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  static std::uniform_real_distribution<> dis(-1, 1);

  std::vector<std::vector<float>> ret;
  ret.resize(model_tensor_info->input_cnt);
  int in_n, in_c, in_h, in_w;
  for (int i = 0, pos = 0; i < model_tensor_info->input_cnt; ++i) {
    in_n = model_tensor_info->input_shape[pos++];
    in_c = model_tensor_info->input_shape[pos++];
    in_h = model_tensor_info->input_shape[pos++];
    in_w = model_tensor_info->input_shape[pos++];
    ret[i].resize(in_n * in_c * in_h * in_w);
    for (int j = 0; j < ret[i].size(); j++) {
      ret[i][j] = dis(gen);
    }
  }
  return ret;
}

std::optional<InferenceResult> MixModelManagerWrapper::RunModelSync(
    const std::string &offline_model_name,
    const std::vector<std::vector<float>> &data_buff) {
  if (nullptr == manager || nullptr == model_tensor_info) {
    LOGE("please load model first");
    return nullopt;
  }

  HIAI_MixTensorBuffer *inputs[model_tensor_info->input_cnt];
  HIAI_MixTensorBuffer *outputs[model_tensor_info->output_cnt];
  std::vector<int> output_size(model_tensor_info->output_cnt);
  int in_n = 0;
  int in_c = 0;
  int in_h = 0;
  int in_w = 0;

  int out_n = 0;
  int out_c = 0;
  int out_h = 0;
  int out_w = 0;
  for (int i = 0, pos = 0; i < model_tensor_info->input_cnt; ++i) {
    if (nullptr != model_tensor_info->input_shape) {
      LOGI("input %d shape show as below: ", i);
      in_n = model_tensor_info->input_shape[pos++];
      in_c = model_tensor_info->input_shape[pos++];
      in_h = model_tensor_info->input_shape[pos++];
      in_w = model_tensor_info->input_shape[pos++];

      LOGI("input_n = %d, input_c = %d, input_h = %d, input_w = %d", in_n, in_c,
           in_h, in_w);
    }

    HIAI_MixTensorBuffer *input =
        HIAI_MixTensorBuffer_Create(in_n, in_c, in_h, in_w);  // NCHW
    if (nullptr == input) {
      LOGE("fail: HIAI_MixTensorBuffer_Create input");
      HIAI_MixModel_ReleaseModelTensorInfo(model_tensor_info);
      HIAI_MixModelBuffer_Destroy(model_buffer);
      HIAI_MixModelManager_Destroy(manager);
      return nullopt;
    }

    inputs[i] = input;
  }

  for (int i = 0, pos = 0; i < model_tensor_info->output_cnt; ++i) {
    if (nullptr != model_tensor_info->output_shape) {
      LOGI("output %d shape show as below : ", i);
      out_n = model_tensor_info->output_shape[pos++];
      out_c = model_tensor_info->output_shape[pos++];
      out_h = model_tensor_info->output_shape[pos++];
      out_w = model_tensor_info->output_shape[pos++];

      LOGI("output_n = %d, output_c = %d, output_h = %d, output_w = %d", out_n,
           out_c, out_h, out_w);
    }
    output_size[i] = out_n * out_c * out_h * out_w;
    // 3.3 malloc   output
    HIAI_MixTensorBuffer *output =
        HIAI_MixTensorBuffer_Create(out_n, out_c, out_h, out_w);

    if (nullptr == output) {
      LOGE("fail: HIAI_MixTensorBuffer_Create output");
      HIAI_MixModel_ReleaseModelTensorInfo(model_tensor_info);
      HIAI_MixModelBuffer_Destroy(model_buffer);
      HIAI_MixModelManager_Destroy(manager);
      return nullopt;
    }
    outputs[i] = output;
  }
  // init  inputtensor
  for (int i = 0; i < model_tensor_info->input_cnt; ++i) {
    float *in_data = (float *)HIAI_MixTensorBuffer_GetRawBuffer(inputs[i]);
    int size = HIAI_MixTensorBuffer_GetBufferSize(inputs[i]);
    memcpy(in_data, data_buff[i].data(), size);
  }

  double time_ms;
  {
    auto from_time = std::chrono::high_resolution_clock::now();
    int ret = HIAI_MixModel_RunModel(
        manager, inputs, model_tensor_info->input_cnt, outputs,
        model_tensor_info->output_cnt, std::numeric_limits<uint32_t>::max(),
        offline_model_name.c_str());
    auto to_time = std::chrono::high_resolution_clock::now();
    time_ms = std::chrono::duration_cast<std::chrono::nanoseconds>(to_time -
                                                                   from_time)
                  .count() /
              1e6;
    LOGE("run model ret: %d", ret);
  }

  std::vector<std::vector<float>> result(model_tensor_info->output_cnt);
  for (int o = 0; o < model_tensor_info->output_cnt; o++) {
    float *output_buffer =
        (float *)HIAI_MixTensorBuffer_GetRawBuffer(outputs[o]);
    result[o] = std::vector<float>(output_size[o]);
    std::copy(output_buffer, output_buffer + output_size[o], result[o].begin());
  }

  if (input_tensor != nullptr) {
    HIAI_MixTensorBufferr_Destroy(input_tensor);
    input_tensor = nullptr;
  }

  if (output_tensor != nullptr) {
    HIAI_MixTensorBufferr_Destroy(output_tensor);
    output_tensor = nullptr;
  }

  return {{.time_ms = time_ms, .data = result}};
}

bool MixModelManagerWrapper::ModelCompatibilityProcessFromFile(
    std::string online_model, std::string online_model_parameter,
    std::string framework, std::string offline_model_path, bool mix_flag) {
  LOGI("online_model = %s", online_model.c_str());
  LOGI("online_model_parameter = %s", online_model_parameter.c_str());
  LOGI("framework = %s", framework.c_str());
  LOGI("offline_model_path = %s", offline_model_path.c_str());

  HIAI_Mix_Framework model_framework;
  if (framework == "caffe") {
    model_framework = HIAI_Mix_Framework::HIAI_MIX_FRAMEWORK_CAFFE;
  } else if (framework == "caffe_8bit") {
    model_framework = HIAI_Mix_Framework::HIAI_MIX_FRAMEWORK_CAFFE_8BIT;
  } else if (framework == "tensorflow") {
    model_framework = HIAI_Mix_Framework::HIAI_MIX_FRAMEWORK_TENSORFLOW;
  } else if (framework == "tensorflow_8bit") {
    model_framework = HIAI_Mix_Framework::HIAI_MIX_FRAMEWORK_TENSORFLOW_8BIT;
  } else {
    return false;
  }

  HIAI_MixModelManager *mix_model_manager =
      HIAI_MixModelManager_Create(nullptr);
  LOGI("mix_model_manager %s nullptr",
       mix_model_manager == nullptr ? "==" : "!=");

  // FIXME
  // const char *hiai_version = HIAI_ModelManager_GetVersion(mix_model_manager);
  const char *hiai_version = "100.150.032.000";

  LOGI("hiai_version = %s", hiai_version);
  ResultCode result_code;
  if (std::string(hiai_version) == "000.000.000.000") {
    result_code = NO_NPU;
  } else {
    bool check = FileExists(offline_model_path) &&
                 HIAI_CheckMixModelCompatibility_From_File(
                     mix_model_manager, mix_flag, offline_model_path.c_str());
    LOGI("check result = %d", check);
    if (check) {
      result_code = CHECK_MODEL_COMPATIBILITY_SUCCESS;
    } else {
      int res = HIAI_MixModel_BuildModel_FromPath(
          mix_model_manager, model_framework, online_model.c_str(),
          online_model_parameter.c_str(), offline_model_path.c_str(), mix_flag);
      LOGI("HIAI_MixModel_BuildModel_FromPath() = %d", res);
      if (res != 0) {
        result_code = BUILD_ONLINE_MODEL_FAILED;
      } else {
        result_code = BUILD_ONLINE_MODEL_SUCCESS;
      }
    }
  }

  LOGI("result_code = %d", result_code);
  HIAI_MixModelManager_Destroy(mix_model_manager);

  return result_code == CHECK_MODEL_COMPATIBILITY_SUCCESS ||
         result_code == BUILD_ONLINE_MODEL_SUCCESS;
}

std::string MixModelManagerWrapper::GetTfVersion() {
  auto handle = dlopen("libtensorflow_inference.so", RTLD_LOCAL | RTLD_LAZY);
  auto fptr = (const char *(*)())dlsym(handle, "TF_Version");
  return fptr();
}
