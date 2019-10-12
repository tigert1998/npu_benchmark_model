LOCAL_PATH := $(call my-dir)
ifeq ($(TARGET_ARCH_ABI),arm64-v8a)
  SO_PATH := $(LOCAL_PATH)/lib64
else
  SO_PATH := $(LOCAL_PATH)/lib
endif
