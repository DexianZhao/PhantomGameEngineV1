LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

subdirs := $(LOCAL_PATH)/../../../PhantomEngine/PhantomGamePlatform.mk
LOCAL_PROGUARD_FLAG_FILES := proguard.flags

include $(subdirs)

