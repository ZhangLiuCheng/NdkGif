//
// Created by zhangliucheng on 2017/9/10.
//
#include "com_ylcq_ndkgif_GifHandler.h"
#include "gifLib/gif_lib.h"
#include <android/log.h>
#include <android/bitmap.h>
#include <malloc.h>

#define LOG_TAG "ZLC"
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)

typedef struct GifBean {
    int total_duration;
    int current_frame;
    int frame_duration;
    int total_frame;
} GifBean;

JNIEXPORT jlong JNICALL Java_com_ylcq_ndkgif_GifHandler_loadGif
        (JNIEnv *env, jclass clazz, jstring path) {
    LOGV("loadGif  ---> begin");
    int error;
    const char* filePath = env->GetStringUTFChars(path, 0);
    GifFileType *gifFileType = DGifOpenFileName(filePath, &error);
    if (error != D_GIF_SUCCEEDED) {
        LOGV("gif 解析错误");
        return 0;
    }
    DGifSlurp(gifFileType);
    GifBean *gifBean = (GifBean *)malloc(sizeof(GifBean));
    gifBean->current_frame = 0;
    gifBean->total_duration = 0;
    gifBean->total_frame = 0;
    gifFileType->UserData = gifBean;


    SavedImage *frame;
    ExtensionBlock *ext;
    int i, j, frame_delay;
    for (int i = 0; i < gifFileType->ImageCount; i++) {
        frame = &(gifFileType->SavedImages[i]);
        for (int j = 0; j < frame->ExtensionBlockCount; j++) {
            if (frame->ExtensionBlocks[j].Function == GRAPHICS_EXT_FUNC_CODE) {
                ext = &(frame->ExtensionBlocks[i]);
            }
        }
        if (ext) {
            frame_delay = 10 * (ext->Bytes[2] << 8 | ext ->Bytes[1]);
            LOGV("延迟时间 %d", frame_delay);
            gifBean->total_duration += frame_delay;
        }
    }
    gifBean->frame_duration = gifBean->total_duration / gifFileType->ImageCount;
    gifBean->total_frame = gifFileType->ImageCount;
    LOGV("总播放时间 %d", gifBean->total_duration);

    env->ReleaseStringUTFChars(path, filePath);
    LOGV("loadGif  ---> end");
    return (long long)gifFileType;
}

JNIEXPORT jint JNICALL Java_com_ylcq_ndkgif_GifHandler_getWidth
        (JNIEnv *env, jclass clazz, jlong gifHandler) {
    GifFileType *gifFileType = (GifFileType *)gifHandler;
    return gifFileType->SWidth;
}

JNIEXPORT jint JNICALL Java_com_ylcq_ndkgif_GifHandler_getHeight
        (JNIEnv *env, jclass clazz, jlong gifHandler) {
    GifFileType *gifFileType = (GifFileType *)gifHandler;
    return gifFileType->SHeight;
}

JNIEXPORT jint JNICALL Java_com_ylcq_ndkgif_GifHandler_getNextTime
        (JNIEnv *env, jclass clazz, jlong gifHandler) {
    GifFileType *gifFileType = (GifFileType *)gifHandler;
    GifBean *gifBean = (GifBean *)(gifFileType->UserData);
    return gifBean->frame_duration;
}

JNIEXPORT jint JNICALL Java_com_ylcq_ndkgif_GifHandler_updateFrame
        (JNIEnv *env, jclass clazz, jlong gifHandler, jobject bitmap) {
    GifFileType *gifFileType = (GifFileType *)gifHandler;
    GifBean *gifBean = (GifBean *)(gifFileType->UserData);

    AndroidBitmapInfo info;
    int *pixels;
    AndroidBitmap_getInfo(env, bitmap, &info);
    AndroidBitmap_lockPixels(env, bitmap, (void **)&pixels);

    SavedImage frame = gifFileType->SavedImages[gifBean->current_frame];
    GifImageDesc gifImageDesc = frame.ImageDesc;
    ColorMapObject *colorMapObject =frame.ImageDesc.ColorMap;
    ExtensionBlock extensionBlock;
    for (int i = 0; i < frame.ExtensionBlockCount; i++) {
        if (frame.ExtensionBlocks[i].Function == GRAPHICS_EXT_FUNC_CODE) {
            extensionBlock = frame.ExtensionBlocks[i];
            break;
        }
    }
    int *px = pixels;
    // info.stride代表每一行的像素点
    px = (int*)((char *)px + info.stride * gifImageDesc.Top);

    for (int y = gifImageDesc.Top; y < gifImageDesc.Top + gifImageDesc.Height; y++) {
        int *line = px;
        for (int x = gifImageDesc.Left; x < gifImageDesc.Left + gifImageDesc.Width; x++) {
            int loc = (y - gifImageDesc.Top) * gifImageDesc.Width + (x - gifImageDesc.Left);
            // 校准帧
            if (frame.RasterBits[loc] == extensionBlock.Bytes[3] && extensionBlock.Bytes[0]) {
                continue;
            }
            GifColorType *color = &colorMapObject->Colors[frame.RasterBits[loc]];
            line[x] = ((255&0xff) << 24) | ((color->Blue&0xff) << 16) | ((color->Green&0xff) << 8) | (color->Red&0xff);
        }
        px = (int*) ((char*)px + info.stride);
    }
    gifBean->current_frame +=1;
    if (gifBean -> current_frame >= gifBean->total_frame) {
        gifBean->current_frame = 0;
    }
    AndroidBitmap_unlockPixels(env, bitmap);
    return gifBean->frame_duration;
}