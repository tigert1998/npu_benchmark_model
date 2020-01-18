LOCAL_PATH:= $(call my-dir)
SDK_RELATIVE_TO_JNI_PATH = ../../rknn_api_1_2_0
SDK_PATH := $(LOCAL_PATH)/$(SDK_RELATIVE_TO_JNI_PATH)
COMMON_RELATIVE_TO_JNI_PATH = ../../common
COMMON_PATH := $(LOCAL_PATH)/$(COMMON_RELATIVE_TO_JNI_PATH)
STB_RELATIVE_TO_JNI_PATH = ../../third_party/stb
STB_PATH := $(LOCAL_PATH)/$(STB_RELATIVE_TO_JNI_PATH)
DBG_RELATIVE_TO_JNI_PATH = ../../third_party/dbg-macro
DBG_PATH := $(LOCAL_PATH)/$(DBG_RELATIVE_TO_JNI_PATH)

include $(CLEAR_VARS)
LOCAL_MODULE := rknn
LOCAL_SRC_FILES := $(SDK_RELATIVE_TO_JNI_PATH)/lib64/librknn_api.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := rknn_benchmark_model

LOCAL_C_INCLUDES += \
	$(SDK_PATH)/include \
	$(COMMON_PATH) \
	$(STB_PATH) \
	$(DBG_PATH)

LOCAL_SHARED_LIBRARIES := rknn

LOCAL_SRC_FILES := \
	main.cc rknn_api_wrapper.cc rknn_api_aux.cc latency_test.cc accuracy_test.cc \
	$(patsubst jni/%,%,$(wildcard $(COMMON_PATH)/*.cc))

LOCAL_LDFLAGS += \
	-Wl,-L$(SDK_PATH)/lib64
LOCAL_LDLIBS += \
	-llog

LOCAL_CPPFLAGS += -std=c++17 -rpath -frtti -fexceptions -lpthread

include $(BUILD_EXECUTABLE)
