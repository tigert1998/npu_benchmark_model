#include "libai_client_so_wrapper.h"

#include <android/log.h>
#include <dlfcn.h>

#define LOG_TAG "LIBAI_CLIENT_SO_WRAPPER_MSG"

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

namespace libai_client_so {
const std::string path = "/vendor/lib64/libai_client.so";

const char* HIAI_GetVersion(void* handle) {
  const char* error;
  auto func = (const char* (*)())dlsym(handle, "HIAI_GetVersion");
  if ((error = dlerror()) != nullptr) {
    LOGE("HIAI_GetVersion ERROR: dlsym fail: %s", error);
    return nullptr;
  }
  return func();
}

HIAI_ModelManager* HIAI_ModelManager_create(
    void* handle, HIAI_ModelManagerListener* listener) {
  const char* error;
  auto func = (HIAI_ModelManager * (*)(HIAI_ModelManagerListener*))
      dlsym(handle, "HIAI_ModelManager_create");
  if ((error = dlerror()) != nullptr) {
    LOGE("HIAI_ModelManager_create ERROR: dlsym fail: %s", error);
    return nullptr;
  }

  return func(listener);
}

bool HIAI_CheckModelCompatibility_from_file(void* handle,
                                            HIAI_ModelManager* manager,
                                            const char* offline_model_path) {
  const char* error;
  auto func = (bool (*)(HIAI_ModelManager*, const char*))dlsym(
      handle, "HIAI_CheckModelCompatibility_from_file");
  if ((error = dlerror()) != nullptr) {
    LOGE("HIAI_CheckModelCompatibility_from_file ERROR: dlsym fail: %s", error);
    return false;
  }

  return func(manager, offline_model_path);
}

void HIAI_ModelManager_destroy(void* handle, HIAI_ModelManager* manager) {
  const char* error;
  auto func =
      (void (*)(HIAI_ModelManager*))dlsym(handle, "HIAI_ModelManager_destroy");
  if ((error = dlerror()) != nullptr) {
    LOGE("HIAI_ModelManager_destroy ERROR: dlsym fail: %s", error);
    return;
  }
  func(manager);
  return;
}

HIAI_MemBuffer* HIAI_MemBuffer_create_from_file(void* handle,
                                                const char* path) {
  const char* error;
  auto func = (HIAI_MemBuffer * (*)(const char*))
      dlsym(handle, "HIAI_MemBuffer_create_from_file");
  if ((error = dlerror()) != nullptr) {
    LOGE("HIAI_MemBuffer_create_from_file ERROR: dlsym fail: %s", error);
    return nullptr;
  }
  return func(path);
}

HIAI_MemBuffer* HIAI_ReadBinaryProto_from_buffer(void* handle, void* buffer,
                                                 const unsigned int size) {
  const char* error;
  auto func = (HIAI_MemBuffer * (*)(void*, const unsigned int))
      dlsym(handle, "HIAI_ReadBinaryProto_from_buffer");
  if ((error = dlerror()) != NULL) {
    LOGE("HIAI_ReadBinaryProto_from_buffer ERROR: dlsym fail: %s", error);
    return NULL;
  }
  return func(buffer, size);
}

HIAI_MemBuffer* HIAI_ReadBinaryProto_from_file(void* handle, const char* path) {
  const char* error;
  auto func = (HIAI_MemBuffer * (*)(const char*))
      dlsym(handle, "HIAI_ReadBinaryProto_from_file");
  if ((error = dlerror()) != NULL) {
    LOGE("HIAI_ReadBinaryProto_from_file ERROR: dlsym fail: %s", error);
    return NULL;
  }
  return func(path);
}

void HIAI_MemBuffer_destroy(void* handle, HIAI_MemBuffer* membuf) {
  const char* error;
  auto func =
      (void (*)(HIAI_MemBuffer*))dlsym(handle, "HIAI_MemBuffer_destroy");
  if ((error = dlerror()) != NULL) {
    LOGE("HIAI_MemBuffer_destroy ERROR: dlsym fail: %s", error);
    return;
  }
  func(membuf);
  return;
}

HIAI_MemBuffer* HIAI_MemBuffer_create(void* handle,
                                      HIAI_Framework frameworkType,
                                      HIAI_MemBuffer* inputModelBuffers[],
                                      const unsigned int inputModelBuffersNum) {
  const char* error;
  auto func = (HIAI_MemBuffer *
               (*)(HIAI_Framework, HIAI_MemBuffer* [], const unsigned int))
      dlsym(handle, "HIAI_MemBuffer_create");
  if ((error = dlerror()) != NULL) {
    LOGE("HIAI_MemBuffer_create ERROR: dlsym fail: %s", error);
    return NULL;
  }
  return func(frameworkType, inputModelBuffers, inputModelBuffersNum);
}

bool HIAI_MemBuffer_export_file(void* handle, HIAI_MemBuffer* membuf,
                                const unsigned int buildSize,
                                const char* buildPath) {
  const char* error;
  auto func = (bool (*)(HIAI_MemBuffer*, const unsigned int, const char*))dlsym(
      handle, "HIAI_MemBuffer_export_file");
  if ((error = dlerror()) != NULL) {
    LOGE("HIAI_MemBuffer_export_file ERROR: dlsym fail: %s", error);
    return false;
  }
  return func(membuf, buildSize, buildPath);
}

int HIAI_ModelManager_buildModel(void* handle, HIAI_ModelManager* manager,
                                 HIAI_Framework frameworkType,
                                 HIAI_MemBuffer* inputModelBuffers[],
                                 const unsigned int inputModelBuffersNum,
                                 HIAI_MemBuffer* outputModelBuffer,
                                 unsigned int* outModelSize) {
  const char* error;
  auto func = (int (*)(HIAI_ModelManager*, HIAI_Framework,
                       HIAI_MemBuffer * inputModelBuffers[], const unsigned int,
                       HIAI_MemBuffer*, unsigned int*))
      dlsym(handle, "HIAI_ModelManager_buildModel");
  if ((error = dlerror()) != NULL) {
    LOGE("HIAI_ModelManager_buildModel ERROR: dlsym fail: %s", error);
    return 1;
  }
  return func(manager, frameworkType, inputModelBuffers, inputModelBuffersNum,
              outputModelBuffer, outModelSize);
}

}  // namespace libai_client_so