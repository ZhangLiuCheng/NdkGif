LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := com_ylcq_ndkgif_GifHandler.cpp \
                   gifLib/dgif_lib.c \
                   gifLib/egif_lib.c \
                   gifLib/gif_err.c \
                   gifLib/gif_font.c \
                   gifLib/gif_hash.c \
                   gifLib/gifalloc.c \
                   gifLib/openbsd-reallocarray.c \
                   gifLib/quantize.c
LOCAL_MODULE = gifLib
LOCAL_LDLIBS := -llog
LOCAL_CFLAGS += -DS_IREAD -DS_IWRITE -DSIZE_MAX=10
LOCAL_CPPFLAGS += -fpermissive -Wwrite-strings

include $(BUILD_SHARED_LIBRARY)
