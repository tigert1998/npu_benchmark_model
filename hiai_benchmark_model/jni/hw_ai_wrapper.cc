#include "hw_ai_wrapper.h"

#include <android/log.h>
#include <sys/time.h>
#include <cstring>
#include <memory>

#define LOG_TAG "SYNC_DDK_MSG"

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

using std::nullopt;

int HwAiWrapper::LoadModelFromFileSync(const std::string &model_name,
                                       const std::string &model_path,
                                       bool mix_flag) {
  LOGI("[LoadModelFromFileSync] model_path = %s", model_path.c_str());
  LOGI("[LoadModelFromFileSync] model_name = %s", model_name.c_str());

  model_buffer = HIAI_MixModelBuffer_Create_From_File(
      model_name.c_str(), model_path.c_str(), HIAI_MIX_DEVPREF_LOW, mix_flag);

  if (model_buffer == nullptr) {
    LOGI("mixModel is nullptr");
    return -1;
  }

  HIAI_MixModelBuffer *model_buf_arr[] = {model_buffer};
  manager = HIAI_MixModelManager_Create(nullptr);

  if (manager == nullptr) {
    LOGE("creat manager failed.");
    return -1;
  }

  int ret = HIAI_MixModel_LoadFromModelBuffers(manager, model_buf_arr, 1);

  LOGI("load model from file ret = %d", ret);
  if (ret == 0) {
    model_tensor_info =
        HIAI_MixModel_GetModelTensorInfo(manager, model_name.c_str());
  }

  return ret;
}

std::optional<std::vector<std::vector<float>>> HwAiWrapper::RunModelSync(
    const std::string &model_name,
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

  float time_use;
  struct timeval tpstart, tpend;
  gettimeofday(&tpstart, nullptr);

  int ret = HIAI_MixModel_RunModel(
      manager, inputs, model_tensor_info->input_cnt, outputs,
      model_tensor_info->output_cnt, 1000, model_name.c_str());

  LOGE("run model ret: %d", ret);

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

  return result;
}