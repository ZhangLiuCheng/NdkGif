package com.ylcq.ndkgif;

import android.graphics.Bitmap;

public class GifHandler {

    static {
        System.loadLibrary("timeGif");
    }

    private long gifPoint;

    private GifHandler(long gifPoint) {
        this.gifPoint = gifPoint;
    }

    public long getGifPoint() {
        return this.gifPoint;
    }

    public static GifHandler load(String path) {
        long gifPoint = loadGif(path);
        GifHandler gifHandler = new GifHandler(gifPoint);
        return gifHandler;
    }

    public static native long loadGif(String path);
    public static native int getWidth(long gifPoint);
    public static native int getHeight(long gifPoint);
    public static native int getNextTime(long gifPoint);
    public static native int updateFrame(long gifPoint, Bitmap bitmap);

}
