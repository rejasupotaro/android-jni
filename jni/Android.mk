LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := ImageProcessing
LOCAL_SRC_FILES := ImageProcessing.c

ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
	LOCAL_CFLAGS	:= -DHAVE_NEON=1
	LOCAL_SRC_FILES += image_processing-intrinsics.c.neon
endif

LOCAL_STATIC_LIBRARIES := cpufeatures

LOCAL_LDLIBS    := -lm -llog -ljnigraphics

include $(BUILD_SHARED_LIBRARY)

$(call import-module,cpufeatures)