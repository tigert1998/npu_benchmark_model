LOCAL_PATH:= $(call my-dir)
COMMON_RELATIVE_TO_JNI_PATH = ../../common
COMMON_PATH := $(LOCAL_PATH)/$(COMMON_RELATIVE_TO_JNI_PATH)

include $(CLEAR_VARS)

LOCAL_MODULE := random_test

LOCAL_C_INCLUDES += \
	$(COMMON_PATH)

LOCAL_SRC_FILES := \
	main.cc \
	$(patsubst jni/%,%,$(wildcard $(COMMON_PATH)/*.cc))

LOCAL_LDFLAGS += \
	-Wl
LOCAL_LDLIBS += \
	-llog

LOCAL_CPPFLAGS += -std=c++17 -rpath -frtti -fexceptions -lpthread

include $(BUILD_EXECUTABLE)
