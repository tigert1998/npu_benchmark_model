LOCAL_PATH:= $(call my-dir)

HIAI_SDK_HOME := $(LOCAL_PATH)/../../ai_ddk_mixmodel_lib

# ifeq ($(TARGET_ARCH_ABI),arm64-v8a)
# HIAI_SDK_LIB_PATH := $(HIAI_SDK_HOME)/lib64
# else
# HIAI_SDK_LIB_PATH := $(HIAI_SDK_HOME)/lib
# endif

include $(CLEAR_VARS)
LOCAL_MODULE := main

# LOCAL_CFLAGS += -D__ndk1=__1 \

LOCAL_C_INCLUDES += $(HIAI_SDK_HOME)/include

LOCAL_SRC_FILES := \
	main.cc hw_ai_wrapper.cc

$(warning " gbg LOCALPATH is $(HIAI_SDK_HOME)")

LOCAL_LDFLAGS += -Wl,-L $(HIAI_SDK_HOME)/lib64
LOCAL_LDLIBS += \
	-llog \
    -lhiai \
		
# LOCAL_C_INCLUDES  := $HIAI_SDK_HOME
LOCAL_CPPFLAGS += -std=gnu++17 -frtti -fexceptions -lpthread  
# LOCAL_CPPFLAGS += -std=gnu++14 -rpath -frtti -fexceptions -lpthread  -L $(HIAI_SDK_HOME)/lib64/
# LOCAL_CPPFLAGS += -std=c++14 -rpath -frtti -fexceptions
	
# include $(BUILD_SHARED_LIBRARY)
include $(BUILD_EXECUTABLE)
