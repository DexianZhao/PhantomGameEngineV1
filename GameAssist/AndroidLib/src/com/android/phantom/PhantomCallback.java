package com.android.phantom;

public interface PhantomCallback {
	abstract void onFrameMove();
	abstract String closeEdit();
	abstract void openEdit(String file, int l, int top, int width, int height, int id);
	abstract void showAd(int arg1, int arg2);
	abstract void showBuy(String string);
	abstract void restoreBuy(String string);
	abstract void onLuaMessage(String name, String param, String param2);
}
