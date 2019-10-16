LOCAL_PATH:= $(call my-dir)
DDK_RELATIVE_TO_JNI_PATH = ../../ddk150
DDK_PATH := $(LOCAL_PATH)/$(DDK_RELATIVE_TO_JNI_PATH)

include $(CLEAR_VARS)
LOCAL_MODULE := hiai
LOCAL_SRC_FILES := $(DDK_RELATIVE_TO_JNI_PATH)/lib64/libhiai.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := tensorflow_adapter
LOCAL_SRC_FILES := $(DDK_RELATIVE_TO_JNI_PATH)/lib64/libtensorflow_adapter.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := tensorflow_inference
LOCAL_SRC_FILES := $(DDK_RELATIVE_TO_JNI_PATH)/lib64/libtensorflow_inference.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := hiai_benchmark_model

LOCAL_C_INCLUDES += $(DDK_PATH)/include

LOCAL_SHARED_LIBRARIES := hiai

LOCAL_SRC_FILES := \
	main.cc hw_ai_wrapper.cc types.cc

LOCAL_LDFLAGS += \
	-Wl,-L$(DDK_PATH)/lib64
LOCAL_LDLIBS += \
	-llog

LOCAL_CPPFLAGS += -std=c++17 -rpath -frtti -fexceptions -lpthread  
# LOCAL_CPPFLAGS += -std=gnu++14 -rpath -frtti -fexceptions -lpthread  -L $(DDK_PATH)/lib64/
# LOCAL_CPPFLAGS += -std=c++14 -rpath -frtti -fexceptions

include $(BUILD_EXECUTABLE)
