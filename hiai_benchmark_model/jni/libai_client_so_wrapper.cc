#include "libai_client_so_wrapper.h"

#include <android/log.h>
#include <dlfcn.h>

#define LOG_TAG "LIBAI_CLIENT_SO_WRAPPER_MSG"

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

#define RET_IF_DLERROR(value)                              \
  if (const char* error = dlerror(); error != nullptr) {   \
    LOGE("%s ERROR: dlsym fail: %s", __FUNCTION__, error); \
    return value;                                          \
  }

namespace libai_client_so {
const std::string path = "/vendor/lib64/libai_client.so";

const char* HIAI_GetVersion(void* handle) {
  auto func = (const char* (*)())dlsym(handle, "HIAI_GetVersion");
  RET_IF_DLERROR(nullptr);
  return func();
}

HIAI_ModelManager* HIAI_ModelManager_create(
    void* handle, HIAI_ModelManagerListener* listener) {
  auto func = (HIAI_ModelManager * (*)(HIAI_ModelManagerListener*))
      dlsym(handle, "HIAI_ModelManager_create");
  RET_IF_DLERROR(nullptr);
  return func(listener);
}

bool HIAI_CheckModelCompatibility_from_file(void* handle,
                                            HIAI_ModelManager* manager,
                                            const char* offline_model_path) {
  auto func = (bool (*)(HIAI_ModelManager*, const char*))dlsym(
      handle, "HIAI_CheckModelCompatibility_from_file");
  RET_IF_DLERROR(false);
  return func(manager, offline_model_path);
}

void HIAI_ModelManager_destroy(void* handle, HIAI_ModelManager* manager) {
  auto func =
      (void (*)(HIAI_ModelManager*))dlsym(handle, "HIAI_ModelManager_destroy");
  RET_IF_DLERROR();
  func(manager);
}

HIAI_MemBuffer* HIAI_MemBuffer_create_from_file(void* handle,
                                                const char* path) {
  auto func = (HIAI_MemBuffer * (*)(const char*))
      dlsym(handle, "HIAI_MemBuffer_create_from_file");
  RET_IF_DLERROR(nullptr);
  return func(path);
}

HIAI_MemBuffer* HIAI_ReadBinaryProto_from_buffer(void* handle, void* buffer,
                                                 const unsigned int size) {
  auto func = (HIAI_MemBuffer * (*)(void*, const unsigned int))
      dlsym(handle, "HIAI_ReadBinaryProto_from_buffer");
  RET_IF_DLERROR(nullptr);
  return func(buffer, size);
}

HIAI_MemBuffer* HIAI_ReadBinaryProto_from_file(void* handle, const char* path) {
  auto func = (HIAI_MemBuffer * (*)(const char*))
      dlsym(handle, "HIAI_ReadBinaryProto_from_file");
  RET_IF_DLERROR(nullptr);
  return func(path);
}

void HIAI_MemBuffer_destroy(void* handle, HIAI_MemBuffer* membuf) {
  auto func =
      (void (*)(HIAI_MemBuffer*))dlsym(handle, "HIAI_MemBuffer_destroy");
  RET_IF_DLERROR();
  func(membuf);
}

HIAI_MemBuffer* HIAI_MemBuffer_create(void* handle,
                                      HIAI_Framework frameworkType,
                                      HIAI_MemBuffer* inputModelBuffers[],
                                      const unsigned int inputModelBuffersNum) {
  auto func = (HIAI_MemBuffer *
               (*)(HIAI_Framework, HIAI_MemBuffer* [], const unsigned int))
      dlsym(handle, "HIAI_MemBuffer_create");
  RET_IF_DLERROR(nullptr);
  return func(frameworkType, inputModelBuffers, inputModelBuffersNum);
}

bool HIAI_MemBuffer_export_file(void* handle, HIAI_MemBuffer* membuf,
                                const unsigned int buildSize,
                                const char* buildPath) {
  auto func = (bool (*)(HIAI_MemBuffer*, const unsigned int, const char*))dlsym(
      handle, "HIAI_MemBuffer_export_file");
  RET_IF_DLERROR(false);
  return func(membuf, buildSize, buildPath);
}

int HIAI_ModelManager_buildModel(void* handle, HIAI_ModelManager* manager,
                                 HIAI_Framework frameworkType,
                                 HIAI_MemBuffer* inputModelBuffers[],
                                 const unsigned int inputModelBuffersNum,
                                 HIAI_MemBuffer* outputModelBuffer,
                                 unsigned int* outModelSize) {
  auto func = (int (*)(HIAI_ModelManager*, HIAI_Framework,
                       HIAI_MemBuffer * inputModelBuffers[], const unsigned int,
                       HIAI_MemBuffer*, unsigned int*))
      dlsym(handle, "HIAI_ModelManager_buildModel");
  RET_IF_DLERROR(1);
  return func(manager, frameworkType, inputModelBuffers, inputModelBuffersNum,
              outputModelBuffer, outModelSize);
}

}  // namespace libai_client_so