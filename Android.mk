LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := AdvancedAimingMod.SA
LOCAL_LDLIBS := -llog
LOCAL_SRC_FILES := AdvancedAimingMod.cpp
LOCAL_CFLAGS :=  -w -O3 -mfloat-abi=softfp -mfpu=neon -DNDEBUG
include $(BUILD_SHARED_LIBRARY)