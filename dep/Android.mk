
LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := libpnglib
LOCAL_CFLAGS    := -Werror

LOCAL_C_INCLUDES := $(LOCAL_PATH)/ \
    $(LOCAL_PATH)/png_c \
    $(LOCAL_PATH)/zip_c

LOCAL_SRC_FILES := \
    zlib_c/adler32.c \
    zlib_c/compress.c \
    zlib_c/crc32.c \
    zlib_c/deflate.c \
    zlib_c/gzclose.c \
    zlib_c/gzlib.c \
    zlib_c/gzread.c \
    zlib_c/gzwrite.c \
    zlib_c/infback.c \
    zlib_c/inffast.c \
    zlib_c/inflate.c \
    zlib_c/inftrees.c \
    zlib_c/trees.c \
    zlib_c/uncompr.c \
    zlib_c/zutil.c \
    png_c/png.c \
    png_c/pngerror.c \
    png_c/pngget.c \
    png_c/pngmem.c \
    png_c/pngread.c \
    png_c/pngpread.c \
    png_c/pngrio.c \
    png_c/pngrtran.c \
    png_c/pngrutil.c \
    png_c/pngset.c \
    png_c/pngtrans.c \
    png_c/pngwio.c \
    png_c/pngwrite.c \
    png_c/pngwtran.c \
    png_c/pngwutil.c

LOCAL_LDLIBS    := -llog
      
include $(BUILD_SHARED_LIBRARY)

