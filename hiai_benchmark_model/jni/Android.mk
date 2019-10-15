LOCAL_PATH:= $(call my-dir)
DDK_PATH := $(LOCAL_PATH)/../../ddk150

include $(CLEAR_VARS)

LOCAL_MODULE := main

LOCAL_C_INCLUDES += $(DDK_PATH)/include

LOCAL_SRC_FILES := \
	main.cc hw_ai_wrapper.cc

LOCAL_LDFLAGS += -Wl -L$(DDK_PATH)/lib64
LOCAL_LDLIBS += \
	-lhiai \
	-llog

LOCAL_CPPFLAGS += -std=c++17 -frtti -fexceptions -lpthread  
# LOCAL_CPPFLAGS += -std=gnu++14 -rpath -frtti -fexceptions -lpthread  -L $(DDK_PATH)/lib64/
# LOCAL_CPPFLAGS += -std=c++14 -rpath -frtti -fexceptions

include $(BUILD_EXECUTABLE)
