#include "model_manager_wrapper.h"

#include <android/log.h>
#include <dlfcn.h>
#include <sys/system_properties.h>

#include "libai_client_so_wrapper.h"
#include "util.h"

#define LOG_TAG "MODEL_MANAGER_WRAPPER_MSG"

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

using std::string;

int ModelManagerWrapper::LoadModelFromFileSync(
    const std::string& offline_model_name,
    const std::string& offline_model_path) {
  LOGI("[ModelManagerWrapper::LoadModelFromFileSync] offline_model_path = %s",
       offline_model_path.c_str());

  HIAI_ModelBuffer* model_buf_1 = HIAI_ModelBuffer_create_from_file(
      offline_model_name.c_str(), offline_model_path.c_str(),
      HIAI_DEVPREF_HIGH);
  HIAI_ModelBuffer* model_buf_arr[] = {model_buf_1};
  model_manager = HIAI_ModelManager_create(nullptr);
  if (model_buf_1 == nullptr) {
    LOGI("model_buf_1 == nullptr");
  }

  int ret =
      HIAI_ModelManager_loadFromModelBuffers(model_manager, model_buf_arr, 1);

  LOGI("load model from file ret = %d", ret);

  return ret;
}

ResultCode BuildModel(void* handle, HIAI_ModelManager* manager,
                      string online_model, string online_model_parameter,
                      HIAI_Framework framework, string offline_model_path) {
  // create online MemBuffer
  HIAI_MemBuffer* online_bufs[2];
  if (framework == HIAI_Framework::HIAI_FRAMEWORK_CAFFE) {
    online_bufs[0] = libai_client_so::HIAI_MemBuffer_create_from_file(
        handle, online_model.c_str());
  } else if (framework == HIAI_Framework::HIAI_FRAMEWORK_TENSORFLOW) {
    online_bufs[0] = libai_client_so::HIAI_ReadBinaryProto_from_file(
        handle, online_model.c_str());
  } else {
    return UNSUPPORT_FRAMEWORK;
  }
  if (online_bufs[0] == nullptr) {
    LOGE(
        "BuildModel ERROR: online_bufs[0] create from file fail. Please check "
        "the log.");
    return INVALID_ONLINE_MODEL;
  }

  online_bufs[1] = libai_client_so::HIAI_MemBuffer_create_from_file(
      handle, online_model_parameter.c_str());
  if (online_bufs[1] == nullptr) {
    libai_client_so::HIAI_MemBuffer_destroy(handle, online_bufs[0]);
    return INVALID_ONLINE_MODEL_PARA;
  }

  // create offline MemBuffer
  HIAI_MemBuffer* offline_buf =
      libai_client_so::HIAI_MemBuffer_create(handle, framework, online_bufs, 2);
  if (offline_buf == nullptr) {
    libai_client_so::HIAI_MemBuffer_destroy(handle, online_bufs[0]);
    libai_client_so::HIAI_MemBuffer_destroy(handle, online_bufs[1]);
    return INVALID_OFFLINE_MODEL;
  }

  // call libai_client_so::HIAI_ModelManager_buildModel
  uint32_t offline_model_size;  // offline_model_size

  ResultCode result_code;
  if (0 != libai_client_so::HIAI_ModelManager_buildModel(
               handle, manager, framework, online_bufs, 2, offline_buf,
               &offline_model_size)) {
    result_code = BUILD_ONLINE_MODEL_FAILED;
  } else {
    result_code = BUILD_ONLINE_MODEL_SUCCESS;
    bool gen_result = libai_client_so::HIAI_MemBuffer_export_file(
        handle, offline_buf, offline_model_size, offline_model_path.c_str());
    if (!gen_result) {
      result_code = GENERATE_OFFLINE_MODEL_FAILED;
    }
  }

  // destroy
  libai_client_so::HIAI_MemBuffer_destroy(handle, online_bufs[0]);
  // destroy online models
  libai_client_so::HIAI_MemBuffer_destroy(handle, online_bufs[1]);
  // destroy offline model
  libai_client_so::HIAI_MemBuffer_destroy(handle, offline_buf);

  return result_code;
}

ResultCode ModelCompatibilityProcessFromFileOutFile(
    string online_model, string online_model_parameter,
    HIAI_Framework framework, string offline_model_path) {
  dlerror();
  void* handle = dlopen(libai_client_so::path.c_str(), RTLD_LAZY);
  if (nullptr == handle) {
    LOGE("ModelCompatibilityProcessFromFileOutFile ERROR: dlopen fail: %s.",
         dlerror());
    return DL_OPEN_FAILD;
  }

  // FIXME
  // std::string current_version = libai_client_so::HIAI_GetVersion(handle);
  const std::string current_version = "100.150.032.000";

  // create a modelManager instance
  HIAI_ModelManager* manager =
      libai_client_so::HIAI_ModelManager_create(handle, nullptr);
  if (manager == nullptr) {
    dlclose(handle);
    return CREATE_MODELMANAGER_FAILED;
  }

  ResultCode result_code;
  if (FileExists(offline_model_path) &&
      libai_client_so::HIAI_CheckModelCompatibility_from_file(
          handle, manager, offline_model_path.c_str())) {
    result_code = CHECK_MODEL_COMPATIBILITY_SUCCESS;
    // the offlienModel can run on this NPU.
  } else {
    if (!FileExists(online_model) || !FileExists(online_model_parameter)) {
      result_code = INVALID_ONLINE_MODEL;
    } else {
      result_code =
          BuildModel(handle, manager, online_model, online_model_parameter,
                     framework, offline_model_path);
    }
  }

  libai_client_so::HIAI_ModelManager_destroy(handle, manager);
  dlclose(handle);

  return result_code;
}

bool ModelManagerWrapper::ModelCompatibilityProcessFromFile(
    const std::string& online_model, const std::string& online_model_parameter,
    const std::string& framework, const std::string& offline_model_path) {
  LOGI("online_model = %s", online_model.c_str());
  LOGI("online_model_parameter = %s", online_model_parameter.c_str());
  LOGI("framework = %s", framework.c_str());
  LOGI("offline_model_path = %s", offline_model_path.c_str());

  HIAI_Framework model_framework;
  if (framework.compare("caffe") == 0) {
    model_framework = HIAI_Framework::HIAI_FRAMEWORK_CAFFE;
  } else if (framework.compare("tensorflow") == 0) {
    model_framework = HIAI_Framework::HIAI_FRAMEWORK_TENSORFLOW;
  } else {
    return false;
  }

  auto result_code = ModelCompatibilityProcessFromFileOutFile(
      online_model, online_model_parameter, model_framework,
      offline_model_path);
  LOGI("ModelCompatibilityProcessFromFileOutFile() = %d", result_code);

  return result_code == CHECK_MODEL_COMPATIBILITY_SUCCESS ||
         result_code == BUILD_ONLINE_MODEL_SUCCESS;
}

// TODO: RunModelSync