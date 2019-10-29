#ifndef LIBAI_CLIENT_SO_WRAPPER_H_
#define LIBAI_CLIENT_SO_WRAPPER_H_

#include <string>

#include "HIAIModelManager.h"

namespace libai_client_so {
extern const std::string path;

const char* HIAI_GetVersion(void* handle);

HIAI_ModelManager* HIAI_ModelManager_create(
    void* handle, HIAI_ModelManagerListener* listener);

bool HIAI_CheckModelCompatibility_from_file(void* handle,
                                            HIAI_ModelManager* manager,
                                            const char* offline_model_path);

void HIAI_ModelManager_destroy(void* handle, HIAI_ModelManager* manager);

HIAI_MemBuffer* HIAI_MemBuffer_create_from_file(void* handle, const char* path);

HIAI_MemBuffer* HIAI_ReadBinaryProto_from_buffer(void* handle, void* buffer,
                                                 const unsigned int size);

HIAI_MemBuffer* HIAI_ReadBinaryProto_from_file(void* handle, const char* path);

void HIAI_MemBuffer_destroy(void* handle, HIAI_MemBuffer* membuf);

HIAI_MemBuffer* HIAI_MemBuffer_create(void* handle,
                                      HIAI_Framework frameworkType,
                                      HIAI_MemBuffer* inputModelBuffers[],
                                      const unsigned int inputModelBuffersNum);

bool HIAI_MemBuffer_export_file(void* handle, HIAI_MemBuffer* membuf,
                                const unsigned int buildSize,
                                const char* buildPath);

int HIAI_ModelManager_buildModel(void* handle, HIAI_ModelManager* manager,
                                 HIAI_Framework frameworkType,
                                 HIAI_MemBuffer* inputModelBuffers[],
                                 const unsigned int inputModelBuffersNum,
                                 HIAI_MemBuffer* outputModelBuffer,
                                 unsigned int* outModelSize);

}  // namespace libai_client_so

#endif