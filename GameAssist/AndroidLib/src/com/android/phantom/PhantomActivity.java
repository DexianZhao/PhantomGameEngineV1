package com.android.phantom;

import java.io.File;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import org.json.JSONObject;

import com.android.phantom.*;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.graphics.Rect;
import android.opengl.GLSurfaceView;
import android.os.BatteryManager;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnKeyListener;
import android.view.ViewGroup.LayoutParams;
import android.view.WindowManager;
import android.widget.RelativeLayout;

public class PhantomActivity extends Activity {

	public PhantomSurface engine;
	public int	m_codes[] = null;
    @Override public boolean onTouchEvent(MotionEvent event)
    {
    	int aindex = event.getActionIndex();
    	int count = event.getPointerCount();
    	int e = event.getAction();
        int x = (int) event.getRawX();
        int y = (int) event.getRawY();
    	String str = String.format("aindex:%d,count:%d,event:%d(%d,%d)", aindex,count,e,x,y);
    	for(int i=0;i<count;i++)
    	{
    		int id1 = event.getPointerId(i);
    		str += String.format(",id%d:%d(%.0f,%.0f)", i, id1,event.getX(i), event.getY(i));
    	}
    	Log.i("lww", str);
    	return engine.onTouch(event);
    }
    @Override protected void onCreate(Bundle icicle) {
        super.onCreate(icicle);
        if(m_codes==null)
        {
        	m_codes = new int[1024];
        	for(int i=0;i<1024;i++)
        		m_codes[i] = 0;
        }
        engine = new PhantomSurface(this, m_codes);
        setContentView(engine);
        IntentFilter ifilter = new IntentFilter(Intent.ACTION_BATTERY_CHANGED);
        Intent batteryStatus = registerReceiver(null, ifilter);
        //你可以读到充电状态,如果在充电，可以读到是usb还是交流电
        // 是否在充电
        int status = batteryStatus.getIntExtra(BatteryManager.EXTRA_STATUS, -1);
        boolean isCharging = status == BatteryManager.BATTERY_STATUS_CHARGING ||
                             status == BatteryManager.BATTERY_STATUS_FULL;
        Log.v("AAB", "test");
        if(isCharging)
        	getWindow().setFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON, WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
    }
   
	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		return engine.onKey(keyCode, event);
	}
    @Override protected void onStart()
    {
        super.onStart();
    }
    @Override protected void onStop()
    {
     	super.onStop();
    }
    @Override protected void onDestroy()
    {
    	super.onDestroy();
    }
    @Override protected void onPause() {
    	Phantom.onPause();
        super.onPause();
        engine.onPause();        
    }
    @Override protected void onResume() { 
    	Phantom.onResume();
        super.onResume();
        engine.onResume();
    }
}


