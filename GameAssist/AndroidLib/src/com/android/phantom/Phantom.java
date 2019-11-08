package com.android.phantom;

public class Phantom {
    static{
   	 	System.loadLibrary("appassist");
    }
    public static native void onResize(int ori, int width, int height, int devwidth, int devheight);
    public static native boolean onFrameMove();
    public static native void onRender();
    public static native void onCreate(String packageFile, String fileDir, int[] authcodes);//
    public static native void onDestroy();
    public static native void onLost();
    public static native int  onBack();
    public static native void setScorePoint(int type, int totalpoint);
    public static native void onRestore();
    public static native void onPause();
    public static native void onResume();
    public static native void onMouseEvent(int index, int evt, int x, int y);
    public static native void setDocumentDir(String documentDir);
    public static native void setPixels(int[] pixels, int width, int height, int textWidth, int textHeight);
    public static native void setInputText(String text);
    public static native void sendMessage(String type, String param, String param2);
    public static native void updateGravity(float gx, float gy, float gz);
    public static native void setByteArray(byte[] ptr);
    public static native void updateAcc(float x, float y, float z, long timestamp);
    public static native void updateGyro(float x, float y, float z, long timestamp);
    public static native void resetconn(String ip);
    public static native void researchProj(int userid);
	public static native void setRunIndex(int i);
	public static native void closeEdit(int iscancel, int id, String temp);
}
