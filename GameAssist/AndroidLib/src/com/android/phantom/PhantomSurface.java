package com.android.phantom;

import java.io.File;
import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.nio.IntBuffer;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.LinkedList;
import java.util.List;

import android.R.integer;
import android.app.Activity;
import android.app.ActivityManager;
import android.app.AlertDialog;
import android.app.Service;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;
import android.content.res.AssetFileDescriptor;
import android.content.res.Configuration;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Paint.FontMetrics;
import android.graphics.PixelFormat;
import android.graphics.Rect;
import android.graphics.Typeface;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.media.MediaPlayer.OnPreparedListener;
import android.media.SoundPool;
import android.media.SoundPool.OnLoadCompleteListener;
import android.net.Uri;
import android.opengl.GLSurfaceView;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.os.SystemClock;
import android.os.Vibrator;
import android.provider.Settings.Secure;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGL11;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.opengles.GL10;

import org.apache.http.conn.util.InetAddressUtils;


/**
 * A simple GLSurfaceView sub-class that demonstrate how to perform OpenGL ES
 * 2.0 rendering into a GL Surface. Note the following important details:
 * 
 * - The class must use a custom context factory to enable 2.0 rendering. See
 * ContextFactory class definition below.
 * 
 * - The class must use a custom EGLConfigChooser to be able to select an
 * EGLConfig that supports 2.0. This is done by providing a config specification
 * to eglChooseConfig() that has the attribute EGL10.ELG_RENDERABLE_TYPE
 * containing the EGL_OPENGL_ES2_BIT flag set. See ConfigChooser class
 * definition below.
 * 
 * - The class must select the surface's format, then choose an EGLConfig that
 * matches it exactly (with regards to red/green/blue/alpha channels bit
 * depths). Failure to do so would result in an EGL_BAD_MATCH error.
 */
public class PhantomSurface extends GLSurfaceView {
	private static String TAG = "PhantomSurface";
	private static final boolean DEBUG = false;
	private static final boolean ImmediateInput = false;
	public static int m_adViewShow = -1;
	public static int m_deviceWidth = 960;
	public static int m_deviceHeight = 640;
	public static String s_homeUrl = "http://www.aixspace.com/";
	public static PhantomActivity g_context;
	static String s_pakFile;
	static String s_androidid;
    public String addPackageFile() {
    	String packageName = g_context.getApplication().getPackageName();
    	String apkFilePath;
    	try{
    	apkFilePath = g_context.getApplication().getPackageManager().getApplicationInfo(packageName, 0).sourceDir;
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
            throw new RuntimeException("Unable to locate assets, aborting...");
        }
    	return apkFilePath;
    }
    public static void SetCallback(PhantomCallback c){
    	s_callback = c;
    }
    public static PhantomCallback s_callback = null;
    public static float s_gravity_x = 0;
    public static float s_gravity_y = 0;
    public static float s_gravity_z = 0;
//    public static float s_rotation_x = 0;
//    public static float s_rotation_y = 0;
//    public static float s_rotation_z = 0;
//    public static int s_type = 0;
    public static class AverageValue{
    	float[] xlist = null;
    	float[] ylist = null;
    	float[] zlist = null;
    	public float x = 0,y = 0,z = 0;
    	public int maxavg = 0;
    	int count = 0;
    	AverageValue(int _ma){
    		count = 0;
    		x=0;
    		y=0;
    		z=0;
    		maxavg = _ma;
        	xlist = new float[maxavg];
        	ylist = new float[maxavg];
        	zlist = new float[maxavg];
    	}
    	void add(float _x, float _y, float _z){
    		if(maxavg == 1)
    		{
    			x = _x;
    			y = _y;
    			z = _z;
    			return;
    		}
    		count=count+1;
			xlist[count%maxavg] = _x;
			ylist[count%maxavg] = _y;
			zlist[count%maxavg] = _z;
			int ac = count;
			if(ac>maxavg)ac=maxavg;
			x=y=z=0;
			for(int i=0;i<ac;i++)
			{
				x += xlist[i];
				y += ylist[i];
				z += zlist[i];
			}
			x /= (float)ac;
			y /= (float)ac;
			z /= (float)ac;
    	}
    };
	private static Looper mSensorLooper = null;
	private static SensorEventListener mSensorEventListener = null;
	private static final int[] INPUT_SENSORS = { 1, 4 };
	//public static AverageValue m_acc = new AverageValue(10);
	public PhantomSurface(PhantomActivity act, int[] codes) {
		super(act.getApplication());
        g_context = act;
        s_pakFile	=	addPackageFile();
        File str = g_context.getFilesDir();
        String path = str.getPath() + "/";
        DisplayMetrics metrics = new DisplayMetrics();
        s_androidid = Secure.getString(g_context.getContentResolver(),Secure.ANDROID_ID);
        g_context.getWindowManager().getDefaultDisplay().getMetrics(metrics);
        m_deviceWidth = metrics.widthPixels;
        m_deviceHeight = metrics.heightPixels;
        Phantom.onCreate(s_pakFile, path, codes);
        init(false, 24, 0);
		if (mSensorEventListener == null) {
			SensorManager mSensorMgr = (SensorManager) g_context.getSystemService(g_context.SENSOR_SERVICE);
			// 陀螺仪传感器的类型常量是Sensor.TYPE_GYROSCOPE
			// 加速传感器 TYPE_ACCELEROMETER
			// 近程传感器（Sensor.TYPE_PROXIMITY）
			// 线性加速度传感器（Sensor.TYPE_LINEAR_ACCELERATION）
			// 旋转向量传感器（Sensor.TYPE_ROTATION_VECTOR）
			// 磁场传感器（Sensor.TYPE_MAGNETIC_FIELD）
			// 压力传感器（Sensor.TYPE_PRESSURE）
			// 温度传感器（Sensor.TYPE_TEMPERATURE）
			// 重力传感器（TYPE_GRAVITY）
			Sensor sensorAcc = mSensorMgr
					.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);// TYPE_ACCELEROMETER);
			SensorEventListener lsn = new SensorEventListener() {
				public void onSensorChanged(SensorEvent e) {
					float x = e.values[SensorManager.DATA_X];
					float y = e.values[SensorManager.DATA_Y];
					float z = e.values[SensorManager.DATA_Z];
					int ori = g_context.getRequestedOrientation();
					int confori = g_context.getResources().getConfiguration().orientation;
					if (confori == Configuration.ORIENTATION_LANDSCAPE) {
						s_gravity_x = y*0.1f;
						s_gravity_y = x*0.1f;
					} else {
						s_gravity_x = -x*0.1f;
						s_gravity_y = y*0.1f;
					}
					s_gravity_z = z*0.1f;
					//m_acc.add(s_gravity_x * 0.1f, s_gravity_y * 0.1f, s_gravity_z * 0.1f);
				}

				public void onAccuracyChanged(Sensor arg0, int arg1) {
				}
			};
			mSensorMgr.registerListener(lsn, sensorAcc,
					SensorManager.SENSOR_DELAY_GAME);
			// mSensorMgr.registerListener(lsn, sensorOri,
			// SensorManager.SENSOR_DELAY_GAME);
			// mSensorMgr.registerListener(lsn, sensorMag,
			// SensorManager.SENSOR_DELAY_GAME);
			// mSensorMgr.registerListener(lsn, sensorRot,
			// SensorManager.SENSOR_DELAY_UI);
			mSensorEventListener = new SensorEventListener() {
				public void onSensorChanged(SensorEvent event) {
					if (event.sensor.getType() == 1) {
						Phantom.updateAcc(-event.values[1], event.values[0],
								event.values[2], event.timestamp);
					} else if (event.sensor.getType() == 4) {
						Phantom.updateGyro(-event.values[1], event.values[0], event.values[2], event.timestamp);
					}
				}

				public void onAccuracyChanged(Sensor sensor, int accuracy) {
				}
			};
			Thread sensorThread = new Thread(new Runnable() {
				public void run() {
					Looper.prepare();

					mSensorLooper = Looper.myLooper();
					Handler handler = new Handler();
					SensorManager sensorManager = (SensorManager) g_context
							.getSystemService("sensor");
					for (int sensorType : PhantomSurface.INPUT_SENSORS) {
						Sensor sensor = sensorManager
								.getDefaultSensor(sensorType);
						sensorManager.registerListener(mSensorEventListener,
								sensor, 0, handler);
					}
					Looper.loop();
				}
			});
			sensorThread.start();
		}
		//
	}
	public PhantomSurface(Context context, boolean translucent, int depth, int stencil) {
		super(context);
		init(translucent, depth, stencil);
	}

	private void init(boolean translucent, int depth, int stencil) {

		/*
		 * By default, GLSurfaceView() creates a RGB_565 opaque surface. If we
		 * want a translucent one, we should change the surface's format here,
		 * using PixelFormat.TRANSLUCENT for GL Surfaces is interpreted as any
		 * 32-bit surface with alpha by SurfaceFlinger.
		 */
		if (translucent) {
			this.getHolder().setFormat(PixelFormat.TRANSLUCENT);
		}
		
		/*
		 * Setup the context factory for 2.0 rendering. See ContextFactory class
		 * definition below
		 */
		setEGLContextFactory(new ContextFactory());

		/*
		 * We need to choose an EGLConfig that matches the format of our surface
		 * exactly. This is going to be done in our custom config chooser. See
		 * ConfigChooser class definition below.
		 */
		setEGLConfigChooser(translucent ? new ConfigChooser(8, 8, 8, 8, depth,
				stencil) : new ConfigChooser(5, 6, 5, 0, depth, stencil));

		/* Set the renderer responsible for frame rendering */
		// this.setEGLContextClientVersion(2);
		setRenderer(new Renderer());
	}

	private static class ContextFactory implements
			GLSurfaceView.EGLContextFactory {
		private static int EGL_CONTEXT_CLIENT_VERSION = 0x3098;
		public EGLContext createContext(EGL10 egl, EGLDisplay display,
				EGLConfig eglConfig) {
			Log.w(TAG, "creating OpenGL ES 2.0 context");
			checkEglError("Before eglCreateContext", egl);
			int[] attrib_list = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL10.EGL_NONE };
			EGLContext context = egl.eglCreateContext(display, eglConfig,EGL10.EGL_NO_CONTEXT, attrib_list);
			checkEglError("After eglCreateContext", egl);
			//String cont = egl.eglGetCurrentContext().toString();
			return context;
		}
		public void destroyContext(EGL10 egl, EGLDisplay display,
				EGLContext context) {
			egl.eglDestroyContext(display, context);
		}
	}

	private static void checkEglError(String prompt, EGL10 egl) {
		int error;
		while ((error = egl.eglGetError()) != EGL10.EGL_SUCCESS) {
			Log.e(TAG, String.format("%s: EGL error: 0x%x", prompt, error));
		}
	}

	private static class ConfigChooser implements
			GLSurfaceView.EGLConfigChooser {

		public ConfigChooser(int r, int g, int b, int a, int depth, int stencil) {
			mRedSize = r;
			mGreenSize = g;
			mBlueSize = b;
			mAlphaSize = a;
			mDepthSize = depth;
			mStencilSize = stencil;
		}

		/*
		 * This EGL config specification is used to specify 2.0 rendering. We
		 * use a minimum size of 4 bits for red/green/blue, but will perform
		 * actual matching in chooseConfig() below.
		 */
		private static int EGL_OPENGL_ES2_BIT = 4;
		private static int[] s_configAttribs2 = { EGL10.EGL_RED_SIZE, 4,
				EGL10.EGL_GREEN_SIZE, 4, EGL10.EGL_BLUE_SIZE, 4,
				EGL10.EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT, 
				EGL10.EGL_SAMPLE_BUFFERS, 1,
				EGL10.EGL_SAMPLES, 4,
				EGL10.EGL_NONE };

		public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display) {

			/*
			 * Get the number of minimally matching EGL configurations
			 */
			int[] num_config = new int[1];
			egl.eglChooseConfig(display, s_configAttribs2, null, 0, num_config);

			int numConfigs = num_config[0];

			if (numConfigs <= 0) {
				throw new IllegalArgumentException(
						"No configs match configSpec");
			}
			/*
			 * Allocate then read the array of minimally matching EGL configs
			 */
			EGLConfig[] configs = new EGLConfig[numConfigs];
			egl.eglChooseConfig(display, s_configAttribs2, configs, numConfigs,
					num_config);

			if (DEBUG) {
				printConfigs(egl, display, configs);
			}
			/*
			 * Now return the "best" one
			 */
			return chooseConfig(egl, display, configs);
		}

		public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display,
				EGLConfig[] configs) {
			for (EGLConfig config : configs) {
				int d = findConfigAttrib(egl, display, config,
						EGL10.EGL_DEPTH_SIZE, 0);
				int s = findConfigAttrib(egl, display, config,
						EGL10.EGL_STENCIL_SIZE, 0);

				// We need at least mDepthSize and mStencilSize bits
				if (d < mDepthSize || s < mStencilSize)
					continue;

				// We want an *exact* match for red/green/blue/alpha
				int r = findConfigAttrib(egl, display, config,
						EGL10.EGL_RED_SIZE, 0);
				int g = findConfigAttrib(egl, display, config,
						EGL10.EGL_GREEN_SIZE, 0);
				int b = findConfigAttrib(egl, display, config,
						EGL10.EGL_BLUE_SIZE, 0);
				int a = findConfigAttrib(egl, display, config,
						EGL10.EGL_ALPHA_SIZE, 0);

				if (r == mRedSize && g == mGreenSize && b == mBlueSize
						&& a == mAlphaSize)
					return config;
			}
			return null;
		}

		private int findConfigAttrib(EGL10 egl, EGLDisplay display,
				EGLConfig config, int attribute, int defaultValue) {

			if (egl.eglGetConfigAttrib(display, config, attribute, mValue)) {
				return mValue[0];
			}
			return defaultValue;
		}

		private void printConfigs(EGL10 egl, EGLDisplay display,
				EGLConfig[] configs) {
			int numConfigs = configs.length;
			Log.w(TAG, String.format("%d configurations", numConfigs));
			for (int i = 0; i < numConfigs; i++) {
				Log.w(TAG, String.format("Configuration %d:\n", i));
				printConfig(egl, display, configs[i]);
			}
		}

		private void printConfig(EGL10 egl, EGLDisplay display, EGLConfig config) {
			int[] attributes = { EGL10.EGL_BUFFER_SIZE, EGL10.EGL_ALPHA_SIZE,
					EGL10.EGL_BLUE_SIZE,
					EGL10.EGL_GREEN_SIZE,
					EGL10.EGL_RED_SIZE,
					EGL10.EGL_DEPTH_SIZE,
					EGL10.EGL_STENCIL_SIZE,
					EGL10.EGL_CONFIG_CAVEAT,
					EGL10.EGL_CONFIG_ID,
					EGL10.EGL_LEVEL,
					EGL10.EGL_MAX_PBUFFER_HEIGHT,
					EGL10.EGL_MAX_PBUFFER_PIXELS,
					EGL10.EGL_MAX_PBUFFER_WIDTH,
					EGL10.EGL_NATIVE_RENDERABLE,
					EGL10.EGL_NATIVE_VISUAL_ID,
					EGL10.EGL_NATIVE_VISUAL_TYPE,
					0x3030, // EGL10.EGL_PRESERVED_RESOURCES,
					EGL10.EGL_SAMPLES,
					EGL10.EGL_SAMPLE_BUFFERS,
					EGL10.EGL_SURFACE_TYPE,
					EGL10.EGL_TRANSPARENT_TYPE,
					EGL10.EGL_TRANSPARENT_RED_VALUE,
					EGL10.EGL_TRANSPARENT_GREEN_VALUE,
					EGL10.EGL_TRANSPARENT_BLUE_VALUE,
					0x3039, // EGL10.EGL_BIND_TO_TEXTURE_RGB,
					0x303A, // EGL10.EGL_BIND_TO_TEXTURE_RGBA,
					0x303B, // EGL10.EGL_MIN_SWAP_INTERVAL,
					0x303C, // EGL10.EGL_MAX_SWAP_INTERVAL,
					EGL10.EGL_LUMINANCE_SIZE, EGL10.EGL_ALPHA_MASK_SIZE,
					EGL10.EGL_COLOR_BUFFER_TYPE, EGL10.EGL_RENDERABLE_TYPE,
					0x3042 // EGL10.EGL_CONFORMANT
			};
			String[] names = { "EGL_BUFFER_SIZE", "EGL_ALPHA_SIZE",
					"EGL_BLUE_SIZE", "EGL_GREEN_SIZE", "EGL_RED_SIZE",
					"EGL_DEPTH_SIZE", "EGL_STENCIL_SIZE", "EGL_CONFIG_CAVEAT",
					"EGL_CONFIG_ID", "EGL_LEVEL", "EGL_MAX_PBUFFER_HEIGHT",
					"EGL_MAX_PBUFFER_PIXELS", "EGL_MAX_PBUFFER_WIDTH",
					"EGL_NATIVE_RENDERABLE", "EGL_NATIVE_VISUAL_ID",
					"EGL_NATIVE_VISUAL_TYPE", "EGL_PRESERVED_RESOURCES",
					"EGL_SAMPLES", "EGL_SAMPLE_BUFFERS", "EGL_SURFACE_TYPE",
					"EGL_TRANSPARENT_TYPE", "EGL_TRANSPARENT_RED_VALUE",
					"EGL_TRANSPARENT_GREEN_VALUE",
					"EGL_TRANSPARENT_BLUE_VALUE", "EGL_BIND_TO_TEXTURE_RGB",
					"EGL_BIND_TO_TEXTURE_RGBA", "EGL_MIN_SWAP_INTERVAL",
					"EGL_MAX_SWAP_INTERVAL", "EGL_LUMINANCE_SIZE",
					"EGL_ALPHA_MASK_SIZE", "EGL_COLOR_BUFFER_TYPE",
					"EGL_RENDERABLE_TYPE", "EGL_CONFORMANT" };
			int[] value = new int[1];
			for (int i = 0; i < attributes.length; i++) {
				int attribute = attributes[i];
				String name = names[i];
				if (egl.eglGetConfigAttrib(display, config, attribute, value)) {
					Log.w(TAG, String.format("  %s: %d\n", name, value[0]));
				} else {
					// Log.w(TAG, String.format("  %s: failed\n", name));
					while (egl.eglGetError() != EGL10.EGL_SUCCESS)
						;
				}
			}
		}

		// Subclasses can adjust these values:
		protected int mRedSize;
		protected int mGreenSize;
		protected int mBlueSize;
		protected int mAlphaSize;
		protected int mDepthSize;
		protected int mStencilSize;
		private int[] mValue = new int[1];
	}

	private static final class RenderMessage {
		int code;
		int index;
		int x, y;
		String temp;
		String param;
		String param2;
		//float gx,gy,gz;
	}
	public boolean onKey(int keyCode, KeyEvent event) {
		if (keyCode == KeyEvent.KEYCODE_BACK) {
			SendInput(0, command_id_back, 0, 0);
			return false;
		}
		return false;
	}
	
    public boolean isPtIn(Rect rc, int x, int y)
    {
    	if(x<rc.left||x>rc.right||y<rc.top||y>rc.bottom)
    		return false;
    	return true;
    }
    public boolean onTouch(MotionEvent event)
    {
        int eventaction = event.getAction();
        int index = event.getActionIndex();
//        int x = (int) event.getRawX();
//        int y = (int) event.getRawY();
//        Rect rc = new Rect();
//        getGlobalVisibleRect(rc);
//        if(isPtIn(rc, x, y)){
//	        x -= rc.left;
//	        y -= rc.top;
//	        if(x<0||y<0)
//	        	return true;
        switch (eventaction) {
	        case MotionEvent.ACTION_DOWN:
	        {
	        	for(int i=0;i<event.getPointerCount();i++){
	        		int x = (int) event.getX(i);
	        		int y = (int) event.getY(i);
	        		SendInput(event.getPointerId(i), MotionEvent.ACTION_DOWN, x, y);
	        	}
	        }
	        break;
	        case MotionEvent.ACTION_UP:
	        {
	        	for(int i=0;i<event.getPointerCount();i++){
	        		int x = (int) event.getX(i);
	        		int y = (int) event.getY(i);
	        		SendInput(event.getPointerId(i), MotionEvent.ACTION_UP, x, y);
	        	}
	        }
	        break;
	        case MotionEvent.ACTION_POINTER_1_DOWN:////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	        {
	        	int x = (int) event.getX(0);
	        	int y = (int) event.getY(0);
	        	SendInput(0, MotionEvent.ACTION_DOWN, x, y);
	        }
	        break;
	        case MotionEvent.ACTION_POINTER_1_UP:
	        {
	        	int x = (int) event.getX(0);
	        	int y = (int) event.getY(0);
	        	SendInput(0, MotionEvent.ACTION_UP, x, y);
	        }
	        break;
	      
	        case MotionEvent.ACTION_CANCEL:
	        {
	        	for(int i=0;i<event.getPointerCount();i++){
	        		int x = (int) event.getX(i);
	        		int y = (int) event.getY(i);
	        		SendInput(event.getPointerId(i), MotionEvent.ACTION_UP, x, y);
	        	}
	        }
	        break;
	        case MotionEvent.ACTION_MOVE:
	        {
	        	String str = String.format("a2index:%d,count:%d,(%d,%d)", index,event.getPointerCount(),(int) event.getRawX(),(int) event.getRawY());
	        	for(int i=0;i<event.getPointerCount();i++)
	        	{
	        		int id1 = event.getPointerId(i);
	        		str += String.format(",id%d:%d(%.0f,%.0f)", i, id1,event.getX(i), event.getY(i));
	        	}
	        	Log.i("lww", str);
	        	for(int i=0;i<event.getPointerCount();i++){
	        		int x = (int) event.getX(i);
	        		int y = (int) event.getY(i);
	        		//Phantom.onMouseEvent(event.getPointerId(i), 1, x, y);
	        		SendInput(event.getPointerId(i), MotionEvent.ACTION_MOVE, x, y);
	        	}
	        }
	        break;
	        case MotionEvent.ACTION_POINTER_2_DOWN:
	        {
	        	int x = (int) event.getX(1);
	        	int y = (int) event.getY(1);
	        	SendInput(1, MotionEvent.ACTION_DOWN, x, y);
	        }
	        break;
	        case MotionEvent.ACTION_POINTER_2_UP:
	        {
	        	int x = (int) event.getX(1);
	        	int y = (int) event.getY(1);
	        	SendInput(1, MotionEvent.ACTION_UP, x, y);
	        }
	        break;
	        case MotionEvent.ACTION_POINTER_3_DOWN:
	        {
	        	int x = (int) event.getX(2);
	        	int y = (int) event.getY(2);
	        	SendInput(1, MotionEvent.ACTION_DOWN, x, y);
	        }
	        break;
	        case MotionEvent.ACTION_POINTER_3_UP:
	        {
	        	int x = (int) event.getX(2);
	        	int y = (int) event.getY(2);
	        	SendInput(1, MotionEvent.ACTION_UP, x, y);
	        }
	        break;
//	            case MotionEvent.ACTION_DOWN:
//	            	SendInput(MotionEvent.ACTION_DOWN, x, y);
//	                break;
//	            case MotionEvent.ACTION_MOVE:
//	            	SendInput(MotionEvent.ACTION_MOVE, x, y);
//	                // finger moves on the screen
//	                break;
//	            case MotionEvent.ACTION_UP:
//	            	SendInput(MotionEvent.ACTION_UP, x, y);
//	                // finger leaves the screen
//	                break;
	        }
//	    }
    	return true;
    }
    
    public static void sendMessage(String type, String param, String param2){
		RenderMessage cmd = new RenderMessage();
		cmd.code = 1000;
		cmd.temp = type;
		cmd.param = param;
		cmd.param2=param2;
		synchronized (mRenderQueue) {
			mRenderQueue.add(cmd);
		}
    }
	public void SendInput(int index, int code, int x, int y) {
		RenderMessage cmd = new RenderMessage();
		cmd.code = code;
		cmd.index = index;
		cmd.x = x;
		cmd.y = y;
		synchronized (mRenderQueue) {
			mRenderQueue.add(cmd);
		}
	}
	public static void SendInput2(int index, int code, int x, int y, String str) {
		RenderMessage cmd = new RenderMessage();
		cmd.code = code;
		cmd.index = index;
		cmd.x = x;
		cmd.y = y;
		cmd.temp = str;
		synchronized (mRenderQueue) {
			mRenderQueue.add(cmd);
		}
	}
	private static LinkedList<RenderMessage> mRenderQueue = new LinkedList<RenderMessage>();

	static int command_id_open = 1;
	static int command_id_play = 2;
	static int command_id_stop = 3;
	static int command_id_pause = 4;
	static int command_id_resume = 5;
	static int command_id_openurl = 6;
	static int command_id_openedit = 7;
	static int command_id_closeedit = 8;
	static int command_id_showad = 9;
	static int command_id_hidead = 10;
	static int command_id_buy = 11;
	static int command_id_restore = 12;

	//in
	public static int command_id_back = 100;
	public static int command_id_wallpoint = 101;
	
	private static LinkedList<RenderMessage> tempList = new LinkedList<RenderMessage>();
	private static class Renderer implements GLSurfaceView.Renderer {
		public void onDrawFrame(GL10 gl) {
			boolean bHasCmd = false;
			RenderMessage cmd = null;
			synchronized (mRenderQueue){
				if(mRenderQueue.size()>0)
					tempList.addAll(mRenderQueue);
				mRenderQueue.clear();
			}
				while (tempList.size() > 0) {
					cmd = tempList.removeFirst();
					switch (cmd.code) {
					case MotionEvent.ACTION_DOWN:
						Phantom.onMouseEvent(cmd.index, 2, cmd.x, cmd.y);
						break;
					case MotionEvent.ACTION_MOVE:
						Phantom.onMouseEvent(cmd.index, 1, cmd.x, cmd.y);
						break;
					case MotionEvent.ACTION_UP:
						Phantom.onMouseEvent(cmd.index, 4, cmd.x, cmd.y);
						break;
					case 1000:
						{
							Phantom.sendMessage(cmd.temp, cmd.param, cmd.param2);
						}
						break;
					case 1001:
					{
						Phantom.closeEdit(cmd.index, cmd.x, cmd.temp);
					}
					break;
					case 100:
						if(Phantom.onBack()==0)
						{
							g_context.finish();
							return;
						}
						break;
					case 8://PhantomSurface.command_id_closeedit:
						{
							//Phantom.closeEdit(cmd.x, cmd.temp);
						}
						break;
					}
				}
	        Phantom.updateGravity(s_gravity_x,s_gravity_y,s_gravity_z);//.x, m_acc.y, m_acc.z);
			SystemClock.sleep(5);
			if(s_callback!=null)
				s_callback.onFrameMove();
			Phantom.onFrameMove();
			Phantom.onRender();
		}
		private final float[] mRotationMatrix = new float[16];
		private final float[] mRvalues = new float[3];
		public void onSurfaceChanged(GL10 gl, int width, int height) {
			int oriv = g_context.getRequestedOrientation();
			int ori = 1;
			if(oriv==ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE)
				ori = 3;
			else if(oriv==ActivityInfo.SCREEN_ORIENTATION_PORTRAIT)
				ori = 1;
			Phantom.onResize(ori, width, height, m_deviceWidth, m_deviceHeight);
//			gl.glEnable(GL10.GL_MULTISAMPLE);
		}
		public void onSurfaceCreated(GL10 gl, EGLConfig config) {
			Phantom.onLost();
			Phantom.onRestore();
		}
		
	}
	public static void SaveConfig(String key, String value){
		SharedPreferences preferences=g_context.getSharedPreferences("phantom",Context.MODE_PRIVATE);
		Editor editor=preferences.edit();
		editor.putString(key, value);
		editor.commit();
	}
	
	public static String LoadConfig(String key){
		SharedPreferences preferences=g_context.getSharedPreferences("phantom",Context.MODE_PRIVATE);
		return preferences.getString(key, "");
	}
	public static String GetDeviceName(){
	    Build bd = new Build();    
	    String model = bd.MODEL + bd.ID;
	    return model;
	}
	public static String GetDeviceID(){
	    return s_androidid;
	}
	public static String GetHomeUrl(){
	    return s_homeUrl;
	}
	public static void	Utf8ToWideOS(String str){
		try {
			byte[] result = str.getBytes("UnicodeLittle");
			Phantom.setByteArray(result);
		} catch (UnsupportedEncodingException e) {
			// TODO Auto-generated catch block 
			e.printStackTrace();
		}
	}
	public static void	WideToUtf8OS(byte[] str, int length){
		
	}
	
	public static void SetViewOrientation(int ori){
		Message msg = new Message();
		msg.what = MSGID_SETORIENTATION;
		msg.arg1 = ori;
		mHandler.sendMessage(msg);
	}
	public static class SoundInfo implements OnLoadCompleteListener {
		public SoundInfo() {		};
		public int m_id = 0;
		public int m_soundID = -1;
		public int m_streamID = -1;
		public boolean m_play = false;
		public boolean m_comp = false;
		private float m_leftVolume = 1;
		private float m_rightVolume = 1;
		private float m_fRate = 1;
		public String fileName;
		public void Pause(int streamID) {		}
		public void Resume(int streamID) {		}
		public void Stop(int streamID) {		}
		public void SetVolume(int streamID, float v1, float v2) {		}
		public void SetRate(int streamID, float v1) {		}
		public SoundPool soundPool = null;
		public void Open(String path) {
			fileName = path;
			m_soundID = -1;
			if (soundPool == null)
				soundPool = new SoundPool(10, AudioManager.STREAM_MUSIC, 100);
			try {
				AssetFileDescriptor afd = g_context.getAssets().openFd(path);
				if (afd != null) {
					m_soundID = soundPool.load(afd.getFileDescriptor(), afd.getStartOffset(), afd.getLength(), 0);
					soundPool.setOnLoadCompleteListener(this);
					m_play = false;
					return;
				}
			} catch (Exception e) {

			}
			m_soundID = soundPool.load(path, 0);
			if(m_soundID>0)
			{
				soundPool.setOnLoadCompleteListener(this);
			}
			m_play = false;
		}
		public int Play(float leftVolume, float rightVolume, float fRate) {
			if (m_soundID >= 0) {
				if(m_streamID>=0)
					soundPool.stop(m_streamID);
				m_leftVolume = leftVolume;
				m_rightVolume = rightVolume;
				m_fRate = fRate;
				m_play = true;
				if (m_comp)
				{
					m_streamID = soundPool.play(m_soundID, leftVolume, rightVolume, 1, 0, fRate);
					return m_streamID;
				}
				return -1;
			}
			return -1;
		}

		public void onLoadComplete(SoundPool soundPool, int sampleId, int status) {
			//if (sampleId == m_soundID) {
				if (m_comp == false) {
					m_comp = true;
					if (m_play)
					{
						m_streamID = soundPool.play(m_soundID, m_leftVolume, m_rightVolume, 1, 0, m_fRate);
					}
				}
			//}
			// TODO Auto-generated method stub

		}
	}

	private static final class MediaInfo implements OnPreparedListener {
		MediaPlayer media = null;
		int id;
		boolean m_playing = false;
		boolean m_prepare = false;
		MediaInfo[] 	copys = null;
		int m_index = 0;
		int maxcount = 1;
		String fileName;
		public void Setup(int count){
			if(copys!=null)return;
			maxcount = count;
			if(maxcount>1){
				copys = new MediaInfo[maxcount-1];
				for(int i=0;i<(maxcount-1);i++){
					copys[i] = new MediaInfo();
					copys[i].OpenT(fileName,  0);
				}
			}
		}
		public void OpenT(String file, int count) {
			fileName = file;
			m_index = 0;
			if (media != null) {
				media.release();
				media = null;
			}
			try {
				AssetFileDescriptor afd = g_context.getAssets().openFd(file);
				if (afd != null) {
					if (media == null)
						media = new MediaPlayer();
					media.setDataSource(afd.getFileDescriptor(),
							afd.getStartOffset(), afd.getLength());
					media.setOnPreparedListener(this);
					media.prepareAsync();
					m_playing = false;
					m_prepare = false;
					return;
				}

			} catch (Exception e) {

			}
			if (media == null)
				media = new MediaPlayer();
			try {
				media.setDataSource(file);
			} catch (IllegalArgumentException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
				return;
			} catch (IllegalStateException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
				return;
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
				return;
			}
			media.setOnPreparedListener(this);
			media.prepareAsync();
			m_playing = false;
		}

		public void StopT() {
			if(media==null)return;
			if (media.isPlaying())
			{
				media.pause();
				media.seekTo(0);
			}
			m_playing = false;
		}

		public void PlayT(int loopCount, float fVolume) {
			if(maxcount>1){
				m_index = (m_index+1)%maxcount;
				if(m_index>0){
					copys[m_index-1].StopT();
					copys[m_index-1].PlayT(loopCount, fVolume);
					return;
				}
			}
			StopT();
			if (media != null) {
				try {
					//media.prepareAsync();
					media.setVolume(fVolume, fVolume);
					media.setLooping(loopCount!=0);
					if(m_prepare)
						media.start();
					m_playing = true;
				} catch (Exception e) {
				}
			}
		}
		public void SetVolume(float fVolume) {
			if (media != null) {
				try {
					media.setVolume(fVolume, fVolume);
				} catch (Exception e) {
				}
			}
		}
		
		public void onPrepared(MediaPlayer mp) {
			m_prepare = true;
			if(m_playing)
				mp.start();
		}
	}
	private static LinkedList<MediaInfo> mMediaInfos = new LinkedList<MediaInfo>();
	private static LinkedList<SoundInfo> mSoundinfos = new LinkedList<SoundInfo>();
	protected static final int MSGID_SETORIENTATION = 20;
	protected static final int MSGID_OPENWAVE	= 21;
	protected static final int MSGID_PLAYWAVE	= 22;
	protected static final int MSGID_CLOSEWAVE	= 23;
	protected static final int MSGID_OPENMP3	= 24;
	protected static final int MSGID_PLAYMP3	= 25;
	protected static final int MSGID_PAUSEMP3	= 26;
	protected static final int MSGID_RESUMEMP3	= 27;
	protected static final int MSGID_CLOSEMP3	= 28;
	protected static final int MSGID_STOPMP3	= 29;
	protected static final int MSGID_SETVOLUME	= 30;
	
	protected static final int MSGID_SHOWAD		= 31;
	protected static final int MSGID_SHOWBUY	= 32;
	protected static final int MSGID_RESTOREBUY	= 33;
	protected static final int MSGID_OPENURL	= 34;
	protected static final int MSGID_OPENEDIT	= 35;
	protected static final int MSGID_CLOSEEDIT	= 36;
	protected static final int MSGID_GOTO_MAIN	= 37;
	protected static final int MSGID_CLEAR_PROJ	= 38;
	protected static final int MSGID_ADD_PROJ	= 39;
	protected static final int MSGID_UPDATE_PROJ	= 40;
	protected static final int MSGID_ALERT		= 41;
	
	protected static final int MSGID_SETUPWAVE	= 42;
	protected static final int MSGID_FROMLUAMESSAGE	= 43;

	public interface MainCallback {
		abstract void gotoMain();
	}
	public static MainCallback	s_maincb = null;

	public interface ProjCallback {
		abstract void clearProj();
		abstract void updateProj();
		abstract void addProj(String name, String tm, int index);
	}
	public static ProjCallback	s_projcb = null;
	private static Handler mHandler = new Handler() {
		@Override
		public void handleMessage(Message msg) {
			switch (msg.what) {
			case MSGID_SETORIENTATION:{
				if(msg.arg1==0)
					g_context.setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
				else
					g_context.setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
			}
			break;
			case MSGID_ALERT:{
				AlertDialog.Builder builder = new AlertDialog.Builder(g_context);
				builder.setTitle("Message");
				builder.setMessage(msg.obj.toString());
				builder.setNegativeButton("Cancel", new DialogInterface.OnClickListener() { 
		            public void onClick(DialogInterface dialog, int which) { 
		                // TODO Auto-generated method stub  
		            } 
		        });
				builder.create();
				builder.show();
			}
			break;
			case MSGID_CLEAR_PROJ:{
				if(s_projcb!=null)
					s_projcb.clearProj();
			}
			break;
			case MSGID_UPDATE_PROJ:{
				if(s_projcb!=null)
					s_projcb.updateProj();				
			}
			break;
			case MSGID_ADD_PROJ:{
				if(s_projcb!=null)
				{
					String[] ret = msg.obj.toString().split(",");
					if(ret.length==2)
						s_projcb.addProj(ret[0], ret[1], msg.arg1);
				}
			}
			break;
			case MSGID_GOTO_MAIN:{
				if(s_maincb!=null)
					s_maincb.gotoMain();
			}
			break;
			case MSGID_OPENWAVE:
			{
//				String file = msg.obj.toString();
//				MediaInfo info = new MediaInfo();
//				info.OpenT(file,5);
//				//info.PlayT();
//				info.id = msg.arg1;
//				mMediaInfos.add(info);
				SoundInfo info = new SoundInfo();
				String file = msg.obj.toString();
				info.Open(file);
				info.m_id = msg.arg1;
				mSoundinfos.add(info);
			}
			break;
			case MSGID_PLAYWAVE:
			{
//				for (int k = 0; k < mMediaInfos.size(); k++) {
//					if (mMediaInfos.get(k).id == msg.arg1) {
//						mMediaInfos.get(k).PlayT(0, (float)msg.arg2 / 10000);
//						break;
//					}
//				}
				for(int i=0;i<mSoundinfos.size();i++){
					if(mSoundinfos.get(i).m_id == msg.arg1){
						mSoundinfos.get(i).Play(1, (float)msg.arg2 / 10000, 1);
						break;
					}
				}
			}
			break;
			case MSGID_FROMLUAMESSAGE:{
				Bundle b = msg.getData();
				String name = b.getString("name");
				String param = b.getString("param");
				String param2 = b.getString("param2");
				if(s_callback!=null)
					s_callback.onLuaMessage(name, param, param2);
			}
			break;
			case MSGID_SETUPWAVE:
			{
//				for (int k = 0; k < mMediaInfos.size(); k++) {
//					if (mMediaInfos.get(k).id == msg.arg1) {
//						mMediaInfos.get(k).Setup(msg.arg2);
//						break;
//					}
//				}
			}
			break;
			case MSGID_OPENMP3:
			{
				String file = msg.obj.toString();
				boolean find = false;
				for(int i=0;i<mMediaInfos.size();i++){
					if(mMediaInfos.get(i).fileName==file)
					{
						mMediaInfos.get(i).id = msg.arg1;
						find=true;
					}
				}
				if(find)
					break;
				MediaInfo info = new MediaInfo();
				info.OpenT(file,0);
				//info.PlayT();
				info.id = msg.arg1;
				mMediaInfos.add(info);
			}
			break;
			case MSGID_PLAYMP3:
			{
				for (int k = 0; k < mMediaInfos.size(); k++) {
					if (mMediaInfos.get(k).id == msg.arg1) {
						String str = msg.obj.toString();
						mMediaInfos.get(k).PlayT(Integer.parseInt(str), (float)msg.arg2 / 10000);
						break;
					}
				}
			}
			break;
			case MSGID_SETVOLUME:
			{
				for (int k = 0; k < mMediaInfos.size(); k++) {
					if (mMediaInfos.get(k).id == msg.arg1) {
							mMediaInfos.get(k).SetVolume((float)msg.arg2 / 10000);
							break;
					}
				}
			}
			break;
			case MSGID_STOPMP3:
			{
				for (int k = 0; k < mMediaInfos.size(); k++) {
					if (mMediaInfos.get(k).id == msg.arg1) {
							mMediaInfos.get(k).StopT();
							break;
					}
				}
			}
			break;
			case MSGID_PAUSEMP3:
			{
				for (int k = 0; k < mMediaInfos.size(); k++) {
					if (mMediaInfos.get(k).id == msg.arg1) {
						if(mMediaInfos.get(k).media!=null)
							mMediaInfos.get(k).media.pause();
						break;
					}
				}
			}
			break;
			case MSGID_RESUMEMP3:
			{
				for (int k = 0; k < mMediaInfos.size(); k++) {
					if (mMediaInfos.get(k).id == msg.arg1) {
						if(mMediaInfos.get(k).media!=null)
							mMediaInfos.get(k).media.start();
						break;
					}
				}
			}
			break;
			case MSGID_CLOSEMP3:
			{
				for (int k = 0; k < mMediaInfos.size(); k++) {
					if (mMediaInfos.get(k).id == msg.arg1) {
						mMediaInfos.get(k).StopT();
						break;
					}
				}
			}
			break;
			case MSGID_SHOWAD:
			{
				if(s_callback!=null)
					s_callback.showAd(msg.arg1, msg.arg2);
			}
			break;
			case MSGID_SHOWBUY:
			{
				if(s_callback!=null)
					s_callback.showBuy(msg.obj.toString());
			}
			break;
			case MSGID_RESTOREBUY:
			{
				if(s_callback!=null)
					s_callback.restoreBuy(msg.obj.toString());
			}
			break;
			case MSGID_OPENURL:
			{
				Intent intent = new Intent();
				intent.setData(Uri.parse(msg.obj.toString()));
				intent.setAction(Intent.ACTION_VIEW);
				g_context.startActivity(intent); 
			}
			break;
			case MSGID_OPENEDIT:
			{
				Rect rc = (Rect) msg.obj;
				Bundle b = msg.getData();
				if(s_callback!=null){
				s_callback.openEdit(b.getString("Text"), rc.left, rc.top, rc.right, rc.bottom, msg.arg1);
				}
			}
			break;
			case MSGID_CLOSEEDIT:
			{
				if(s_callback!=null)
				{
					String str = s_callback.closeEdit();
					SendInput2(0, command_id_closeedit, msg.arg1, 0, str);
					if(msg.arg1==0)
					{
						
					}
				}
			}
			break;
			default:
				break;
			}
		}
	};
	public static int GetTextureSize(int n){
		int v = 2;
		for(int i=0;i<32;i++)
		{
			if(n<=v)
				return v;
			v = v * 2;
		}
		return 1024;
	}
	public static void DrawTexts(String font, String str, int fontSize, int fontStyle, int alignment)
	{
		Paint p = new Paint(Paint.DEV_KERN_TEXT_FLAG);//.ANTI_ALIAS_FLAG);
		fontSize=fontSize;
		//字体设置
		//String fontType = "Arial";
		int style = Typeface.NORMAL;
		if(fontStyle == 1)
			style = Typeface.BOLD;
		else if(fontStyle == 2)
			style = Typeface.ITALIC;
		else if(fontStyle == 3)
			style = Typeface.BOLD_ITALIC;
		Typeface typeface = Typeface.create(font, style);
		p.setAntiAlias(true);
		//字体为红色
		p.setColor(0xffffffff);
		p.setTypeface(typeface);
		p.setTextSize(fontSize);
		List<String> strlist = new ArrayList<String>(); 
		int last = 0;
		//str += "_";
		for(int i=0;i<=str.length();i++)
		{
			boolean bend = false;
			if(i==str.length())
			{
				i++;
				bend = true;
			}
			else{
				if(str.charAt(i)=='\r')
					continue;
				if(str.charAt(i)=='\n')
					bend = true;
			}
			if(bend){
				if(i>str.length())
					i--;
				strlist.add(str.substring(last, i));
				last = i+1;
			}
		}
		FontMetrics fm = p.getFontMetrics();
		int sizeW = 0;
		int sizeH = fontSize * strlist.size();
		List<Rect> rects = new ArrayList<Rect>();
		//
		if(strlist.size()==1){
			//绘制字体
			Rect bounds1 = new Rect();
		    p.getTextBounds(str, 0, str.length(), bounds1);
			sizeW = bounds1.width() + 2;
		}
		else
		{
			for(int i=0;i<strlist.size();i++){
				Rect rctemp = new Rect();
			    p.getTextBounds(strlist.get(i), 0, strlist.get(i).length(), rctemp);
			    rects.add(rctemp);
			    int w = rctemp.width() + 2;
			    if(w>sizeW)
			    	sizeW = w;
			}
		}
		//
		int bmw = GetTextureSize(sizeW);
		int bmh = GetTextureSize(sizeH);
		Bitmap bitmap = Bitmap.createBitmap(bmw, bmh, Bitmap.Config.ARGB_8888);
		Canvas canvas = new Canvas(bitmap);
		//背景颜色
		canvas.drawColor(0x00000000);
		float top = fontSize + fm.top - fm.ascent;
		if(strlist.size()==1){
			canvas.drawText(str, 0, top, p);
		}
		else
		{
			for(int i=0;i<strlist.size();i++){
				Rect rctemp = rects.get(i);
				int left = 0;
				if((alignment&0x0001)>0)
					left = (sizeW - rctemp.width())/2;
				else if((alignment&0x0002)>0)
					left = (sizeW - rctemp.width());
				canvas.drawText(strlist.get(i), left, top + i * fontSize, p);
			}
		}
		int[] pixels = new int[bitmap.getWidth()*bitmap.getHeight()];
		bitmap.getPixels(pixels, 0, bitmap.getWidth(), 0, 0, bitmap.getWidth(), bitmap.getHeight());
		Phantom.setPixels(pixels, bitmap.getWidth(), bitmap.getHeight(), sizeW, sizeH);//bounds1.height());
	}
	public static void OnGotoMain(){
		Message msg = new Message();
		msg.what = MSGID_GOTO_MAIN;
		mHandler.sendMessage(msg);
	}
	public static void ClearProject(){
		Message msg = new Message();
		msg.what = MSGID_CLEAR_PROJ;
		mHandler.sendMessage(msg);
	}
	public static void UpdateProject(){
		Message msg = new Message();
		msg.what = MSGID_UPDATE_PROJ;
		mHandler.sendMessage(msg);
	}
	public static void FromLuaMessage(String sName, String sParam, String sParam2){
		Message msg = new Message();
		msg.what = MSGID_FROMLUAMESSAGE;
		Bundle bundleData = new Bundle();
		bundleData.putString("name", sName);
		bundleData.putString("param", sParam);
		bundleData.putString("param2", sParam2);
		msg.setData(bundleData);
		mHandler.sendMessage(msg);
	}
	public static void AddProject(String projName, String time, int index){
		Message msg = new Message();
		msg.what = MSGID_ADD_PROJ;
		msg.arg1 = index;
		msg.obj = projName+","+time;
		mHandler.sendMessage(msg);
	}
	public static void onAlert(String m){
		Message msg = new Message();
		msg.what = MSGID_ALERT;
		msg.obj = m;
		mHandler.sendMessage(msg);
	}
	public static int g_mid = 1;
	public static int OpenWave(String file){
		g_mid++;
		Message msg = new Message();
		msg.what = MSGID_OPENWAVE;
		msg.arg1 = g_mid;
		msg.obj = file;
		mHandler.sendMessage(msg);
		return g_mid;
	}
	public static void PlayWave(int id, float fVolume){
		Message msg = new Message();
		msg.what = MSGID_PLAYWAVE;
		msg.arg1 = id;
		msg.arg2 = (int) (fVolume*10000);
		mHandler.sendMessage(msg);
	}
	public static void CloseWave(int id){
		Message msg = new Message();
		msg.what = MSGID_CLOSEWAVE;
		msg.arg1 = id;
		mHandler.sendMessage(msg);
	}
	public static int OpenMp3(String path){
		g_mid++;
		Message msg = new Message();
		msg.what = MSGID_OPENMP3;
		msg.arg1 = g_mid;
		msg.obj = path;
		mHandler.sendMessage(msg);
		return g_mid;
	}
	public static void SetupWave(int id, int wavecount)
	{
		Message msg = new Message();
		msg.what = MSGID_SETUPWAVE;
		msg.arg1 = id;
		msg.arg2 = wavecount;
		mHandler.sendMessage(msg);
	}
	public static void PlayMp3(int id, int loopCount, float fVolume){
		Message msg = new Message();
		msg.what = MSGID_PLAYMP3;
		msg.arg1 = id;
		msg.arg2 = (int)(fVolume*10000);
		msg.obj = loopCount;
		mHandler.sendMessage(msg);
	}
	public static void PauseMp3(int id){
		Message msg = new Message();
		msg.what = MSGID_PAUSEMP3;
		msg.arg1 = id;
		mHandler.sendMessage(msg);
	}
	public static void ResumeMp3(int id){
		Message msg = new Message();
		msg.what = MSGID_RESUMEMP3;
		msg.arg1 = id;
		mHandler.sendMessage(msg);
	}
	public static void StopMp3(int id){
		Message msg = new Message();
		msg.what = MSGID_STOPMP3;
		msg.arg1 = id;
		mHandler.sendMessage(msg);
	}
	public static void CloseMp3(int id){
		Message msg = new Message();
		msg.what = MSGID_CLOSEMP3;
		msg.arg1 = id;
		mHandler.sendMessage(msg);
	}
	public static void SetVolumeMp3(int id, float fVolume){
		Message msg = new Message();
		msg.what = MSGID_SETVOLUME;
		msg.arg1 = id;
		msg.arg2 = (int)(fVolume*10000);
		mHandler.sendMessage(msg);
	}
	public static void ShowAd(int type, int bshow){
		Message msg = new Message();
		msg.what = MSGID_SHOWAD;
		msg.arg1 = type;
		msg.arg2 = bshow;
		mHandler.sendMessage(msg);
	}
	//显示购买对话框
	public static void ShowBuy(String str){
		Message msg = new Message();
		msg.what = MSGID_SHOWBUY;
		msg.obj = str;
		mHandler.sendMessage(msg);
	}
	//回复购买对话框
	public static void RestoreBuy(String str){
		Message msg = new Message();
		msg.what = MSGID_RESTOREBUY;
		msg.obj = str;
		mHandler.sendMessage(msg);
	}
	//如果是""则进入评价页面
	public static void OpenUrl(String str){
		Message msg = new Message();
		msg.what = MSGID_OPENURL;
		msg.obj = str;
		mHandler.sendMessage(msg);
	}
	public static int GetMemoryFree()
	{
		ActivityManager am = (ActivityManager)g_context.getSystemService(Context.ACTIVITY_SERVICE);
		ActivityManager.MemoryInfo mi = new ActivityManager.MemoryInfo();
		am.getMemoryInfo(mi);
		return (int) mi.availMem;
	}
	public static void OnVibrator(int mill){
		Vibrator vib = (Vibrator) g_context.getSystemService(Service.VIBRATOR_SERVICE);
		vib.vibrate(mill);
	}
	
	public static void OpenEdit(String strDefault, int x, int y, int w, int h, int id){
		Message msg = new Message();
		msg.what = MSGID_OPENEDIT;
		msg.arg1 = id;
		msg.arg2 = y;
		Rect rc = new Rect();
		rc.left=x;
		rc.top=y;
		rc.right=x+w;
		rc.bottom=y+h;
		Bundle bundleData = new Bundle();  
		bundleData.putString("Text", strDefault);  
		msg.setData(bundleData); 
		msg.obj = rc;
		mHandler.sendMessage(msg);
	}
	public static void CloseEdit(int iscancel){
		Message msg = new Message();
		msg.what = MSGID_OPENEDIT;
		mHandler.sendMessage(msg);
	}
    public static String GetLocalIP(){
        try {
            for (Enumeration<NetworkInterface> mEnumeration = NetworkInterface.getNetworkInterfaces(); mEnumeration.hasMoreElements();)
            {
                NetworkInterface intf = mEnumeration.nextElement();
                for (Enumeration<InetAddress> enumIPAddr = intf.getInetAddresses(); enumIPAddr.hasMoreElements();) {
                    InetAddress inetAddress = enumIPAddr.nextElement();
                    String ret = inetAddress.getHostAddress().toString();
                     if (!inetAddress.isLoopbackAddress()&&InetAddressUtils.isIPv4Address(inetAddress.getHostAddress())){
                        return ret;
                    }
                }
            }
        } catch (SocketException ex) {
            System.err.print("error");
        }
        return "";
    }
	
}
