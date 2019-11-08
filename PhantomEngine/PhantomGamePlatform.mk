#	文件       :   PhantomEngine
#	软件名称   :   幻影游戏引擎, Phantom Game Engine, Phantom Engine
#	设计编写者 :   赵德贤(英文:cosmic-life)
#	MSN        :   yuzhou_995@hotmail.com
#	版权所有 赵德贤.  保留所有权利.
#	Copyright (C) 2005-2010 ZhaoDeXian - All Rights Reserved.
#	版权所有 2005-2010 赵德贤 - 保留所有权利.

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := libappassist
LOCAL_CFLAGS    := -Werror
LOCAL_CFLAGS    := -Wno-psabi
LOCAL_CFLAGS	:= -w
LOCAL_CXXFLAGS  := -fsigned-char
LOCAL_CFLAGS	+= -DENGINE_LIB_MODE

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../include \
    $(LOCAL_PATH)/../dep/png_c \
    $(LOCAL_PATH)/../dep/zip_c \
    $(LOCAL_PATH)/../dep/lua \
	$(LOCAL_PATH)/../dep/jpeg_c \
    $(LOCAL_PATH)/req \
    $(LOCAL_PATH)/../dep \
	$(LOCAL_PATH)/../AllGames/base
	
LOCAL_SRC_FILES := \
	/../dep/jpeg_c/jaricom.c \
	/../dep/jpeg_c/jcapimin.c \
	/../dep/jpeg_c/jcapistd.c \
	/../dep/jpeg_c/jcarith.c \
	/../dep/jpeg_c/jccoefct.c \
	/../dep/jpeg_c/jccolor.c \
	/../dep/jpeg_c/jcdctmgr.c \
	/../dep/jpeg_c/jchuff.c \
	/../dep/jpeg_c/jcinit.c \
	/../dep/jpeg_c/jcmainct.c \
	/../dep/jpeg_c/jcmarker.c \
	/../dep/jpeg_c/jcmaster.c \
	/../dep/jpeg_c/jcomapi.c \
	/../dep/jpeg_c/jcparam.c \
	/../dep/jpeg_c/jcprepct.c \
	/../dep/jpeg_c/jcsample.c \
	/../dep/jpeg_c/jctrans.c \
	/../dep/jpeg_c/jdapimin.c \
	/../dep/jpeg_c/jdapistd.c \
	/../dep/jpeg_c/jdarith.c \
	/../dep/jpeg_c/jdatadst.c \
	/../dep/jpeg_c/jdatasrc.c \
	/../dep/jpeg_c/jdcoefct.c \
	/../dep/jpeg_c/jdcolor.c \
	/../dep/jpeg_c/jddctmgr.c \
	/../dep/jpeg_c/jdhuff.c \
	/../dep/jpeg_c/jdinput.c \
	/../dep/jpeg_c/jdmainct.c \
	/../dep/jpeg_c/jdmarker.c \
	/../dep/jpeg_c/jdmaster.c \
	/../dep/jpeg_c/jdmerge.c \
	/../dep/jpeg_c/jdpostct.c \
	/../dep/jpeg_c/jdsample.c \
	/../dep/jpeg_c/jdtrans.c \
	/../dep/jpeg_c/jerror.c \
	/../dep/jpeg_c/jfdctflt.c \
	/../dep/jpeg_c/jfdctfst.c \
	/../dep/jpeg_c/jfdctint.c \
	/../dep/jpeg_c/jidctflt.c \
	/../dep/jpeg_c/jidctfst.c \
	/../dep/jpeg_c/jidctint.c \
	/../dep/jpeg_c/jmemmgr.c \
	/../dep/jpeg_c/jmemnobs.c \
	/../dep/jpeg_c/jquant1.c \
	/../dep/jpeg_c/jquant2.c \
	/../dep/jpeg_c/jutils.c \
    /../dep/zlib_c/adler32.c \
    /../dep/zlib_c/compress.c \
    /../dep/zlib_c/crc32.c \
    /../dep/zlib_c/deflate.c \
    /../dep/zlib_c/gzclose.c \
    /../dep/zlib_c/gzlib.c \
    /../dep/zlib_c/gzread.c \
    /../dep/zlib_c/gzwrite.c \
    /../dep/zlib_c/infback.c \
    /../dep/zlib_c/inffast.c \
    /../dep/zlib_c/inflate.c \
    /../dep/zlib_c/inftrees.c \
    /../dep/zlib_c/trees.c \
    /../dep/zlib_c/uncompr.c \
    /../dep/zlib_c/zutil.c \
    /../dep/png_c/png.c \
    /../dep/png_c/pngerror.c \
    /../dep/png_c/pngget.c \
    /../dep/png_c/pngmem.c \
    /../dep/png_c/pngread.c \
    /../dep/png_c/pngpread.c \
    /../dep/png_c/pngrio.c \
    /../dep/png_c/pngrtran.c \
    /../dep/png_c/pngrutil.c \
    /../dep/png_c/pngset.c \
    /../dep/png_c/pngtrans.c \
    /../dep/png_c/pngwio.c \
    /../dep/png_c/pngwrite.c \
    /../dep/png_c/pngwtran.c \
    /../dep/png_c/pngwutil.c \
    /../dep/lua/lapi.c \
    /../dep/lua/lauxlib.c \
    /../dep/lua/lbaselib.c \
    /../dep/lua/lcode.c \
    /../dep/lua/ldblib.c \
    /../dep/lua/ldebug.c \
    /../dep/lua/ldo.c \
    /../dep/lua/ldump.c \
    /../dep/lua/lfunc.c \
    /../dep/lua/lgc.c \
    /../dep/lua/linit.c \
    /../dep/lua/liolib.c \
    /../dep/lua/llex.c \
    /../dep/lua/lmathlib.c \
    /../dep/lua/lmem.c \
    /../dep/lua/loadlib.c \
    /../dep/lua/lobject.c \
    /../dep/lua/lopcodes.c \
    /../dep/lua/loslib.c \
    /../dep/lua/lparser.c \
    /../dep/lua/lstate.c \
    /../dep/lua/lstring.c \
    /../dep/lua/lstrlib.c \
    /../dep/lua/ltable.c \
    /../dep/lua/ltablib.c \
    /../dep/lua/ltm.c \
    /../dep/lua/lundump.c \
    /../dep/lua/lvm.c \
    /../dep/lua/lzio.c \
    /../dep/lua/print.c \
    /../dep/lua/lua_tinker.cpp \
    req/myunzip.cpp \
    req/sqlite3.c \
    APILua.cpp \
    LuaManager.cpp \
    Box2D_Usage.cpp \
    PhantomAction.cpp \
    PhantomAPI.cpp \
    PhantomAStarPath.cpp \
    PhantomBase.cpp \
    PhantomBone.cpp \
    PhantomCamera.cpp \
    PhantomDB.cpp \
	PhantomDefine.cpp \
    PhantomDevice.cpp \
    PhantomEffect.cpp \
    PhantomFile.cpp \
    PhantomFloatAnimation.cpp \
    PhantomFont.cpp \
    PhantomHttp.cpp \
    PhantomMap2D.cpp \
    PhantomMapBase.cpp \
    PhantomManager.cpp \
    PhantomManagerRender.cpp \
	PhantomMath.cpp \
    PhantomSubMesh.cpp \
    PhantomMesh.cpp \
    PhantomMusic2D.cpp \
    PhantomMusicAndroid.cpp \
	PhantomOpenGL.cpp \
    PhantomOpenGL2_0.cpp \
    PhantomOS.cpp \
    PhantomScene3D.cpp \
    PhantomUIAnalyseText.cpp \
    PhantomUIBase.cpp \
    PhantomUIButton.cpp \
    PhantomUICheckBox.cpp \
    PhantomUIComboBox.cpp \
    PhantomUIControl.cpp \
    PhantomUIDialog.cpp \
    PhantomUIEffect.cpp \
    PhantomUIListBox.cpp \
    PhantomUIListCtrl.cpp \
    PhantomUIManager.cpp \
    PhantomUIProgress.cpp \
    PhantomUIScrollBar.cpp \
    PhantomUISlider.cpp \
	PhantomUITextBox.cpp \
    PhantomUIStatic.cpp \
    PhantomUIStaticFormat.cpp \
    PhantomUITextFormat.cpp \
    PhantomUnit3D.cpp \
    PhantomUnit3DElement.cpp \
    PhantomSocket.cpp \
    PhantomSocketAndroid.cpp \
    PhantomLanguage.cpp \
	PhantomGameBase.cpp \
	PhantomGameObject.cpp \
	PhantomGame.cpp \
	PhantomScene3DUnits.cpp \
	PhantomShadowMap.cpp \
    encrypt.cpp \
	/../GameAssist/Assist/MyGameState.cpp \
	/../GameAssist/Assist/MyGameInit.cpp \
	/../AllGames/base/GameManager.cpp \
	/../AllGames/base/NetService.cpp \
	/../AllGames/base/MD5.cpp \
	/../include/PhantomOSAndroid.cpp \
	/../include/PhantomImp.cpp
 



LOCAL_LDLIBS    := -llog -lGLESv2 -lEGL


include $(BUILD_SHARED_LIBRARY)
