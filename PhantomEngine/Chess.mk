#	文件       :   PhantomEngine
#	软件名称   :   幻影游戏引擎, Phantom Game Engine, Phantom Engine
#	设计编写者 :   赵德贤(英文:cosmic-life)
#	MSN        :   yuzhou_995@hotmail.com
#	版权所有 赵德贤.  保留所有权利.
#	Copyright (C) 2005-2010 ZhaoDeXian - All Rights Reserved.
#	版权所有 2005-2010 赵德贤 - 保留所有权利.

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := libphantom3d
LOCAL_CFLAGS    := -Werror
LOCAL_CFLAGS    := -Wno-psabi

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../include \
    $(LOCAL_PATH)/../dep/png_c \
    $(LOCAL_PATH)/../dep/zip_c \
    $(LOCAL_PATH)/../dep/lua \
    $(LOCAL_PATH)/req \
    $(LOCAL_PATH)/../dep
     
LOCAL_SRC_FILES := \
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
    /../dep/box2d/collision/shapes/b2ChainShape.cpp \
    /../dep/box2d/collision/shapes/b2CircleShape.cpp \
    /../dep/box2d/collision/shapes/b2EdgeShape.cpp \
    /../dep/box2d/collision/shapes/b2PolygonShape.cpp \
    /../dep/box2d/collision/b2BroadPhase.cpp \
    /../dep/box2d/collision/b2CollideCircle.cpp \
    /../dep/box2d/collision/b2CollideEdge.cpp \
    /../dep/box2d/collision/b2CollidePolygon.cpp \
    /../dep/box2d/collision/b2Collision.cpp \
    /../dep/box2d/collision/b2Distance.cpp \
    /../dep/box2d/collision/b2DynamicTree.cpp \
    /../dep/box2d/collision/b2TimeOfImpact.cpp \
    /../dep/box2d/common/b2BlockAllocator.cpp \
    /../dep/box2d/common/b2Draw.cpp \
    /../dep/box2d/common/b2Math.cpp \
    /../dep/box2d/common/b2Settings.cpp \
    /../dep/box2d/common/b2StackAllocator.cpp \
    /../dep/box2d/common/b2Timer.cpp \
    /../dep/box2d/dynamics/contacts/b2ChainAndCircleContact.cpp \
    /../dep/box2d/dynamics/contacts/b2ChainAndPolygonContact.cpp \
    /../dep/box2d/dynamics/contacts/b2CircleContact.cpp \
    /../dep/box2d/dynamics/contacts/b2Contact.cpp \
    /../dep/box2d/dynamics/contacts/b2ContactSolver.cpp \
    /../dep/box2d/dynamics/contacts/b2EdgeAndCircleContact.cpp \
    /../dep/box2d/dynamics/contacts/b2EdgeAndPolygonContact.cpp \
    /../dep/box2d/dynamics/contacts/b2PolygonAndCircleContact.cpp \
    /../dep/box2d/dynamics/contacts/b2PolygonContact.cpp \
    /../dep/box2d/dynamics/joints/b2DistanceJoint.cpp \
    /../dep/box2d/dynamics/joints/b2FrictionJoint.cpp \
    /../dep/box2d/dynamics/joints/b2GearJoint.cpp \
    /../dep/box2d/dynamics/joints/b2Joint.cpp \
    /../dep/box2d/dynamics/joints/b2MouseJoint.cpp \
    /../dep/box2d/dynamics/joints/b2PrismaticJoint.cpp \
    /../dep/box2d/dynamics/joints/b2PulleyJoint.cpp \
    /../dep/box2d/dynamics/joints/b2RevoluteJoint.cpp \
    /../dep/box2d/dynamics/joints/b2RopeJoint.cpp \
    /../dep/box2d/dynamics/joints/b2WeldJoint.cpp \
    /../dep/box2d/dynamics/joints/b2WheelJoint.cpp \
    /../dep/box2d/dynamics/b2Body.cpp \
    /../dep/box2d/dynamics/b2ContactManager.cpp \
    /../dep/box2d/dynamics/b2Fixture.cpp \
    /../dep/box2d/dynamics/b2Island.cpp \
    /../dep/box2d/dynamics/b2World.cpp \
    /../dep/box2d/dynamics/b2WorldCallbacks.cpp \
    /../dep/box2d/rope/b2Rope.cpp \
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
    PhantomUIStatic.cpp \
    PhantomUIStaticFormat.cpp \
    PhantomUITextFormat.cpp \
    PhantomUnit3D.cpp \
    PhantomUnit3DElement.cpp \
    PhantomSocket.cpp \
    PhantomSocketAndroid.cpp \
    PhantomLanguage.cpp \
    encrypt.cpp \
    /../include/PhantomOSAndroid.cpp \
	/../include/PhantomImp.cpp \
	/../AllGames/HJ_Chess/BWEncrypt.cpp \
	/../AllGames/HJ_Chess/BWGameBase.cpp \
	/../AllGames/HJ_Chess/BWGameManager.cpp \
	/../AllGames/HJ_Chess/BWGameModule.cpp \
	/../AllGames/HJ_Chess/BWNetwork.cpp \
	/../AllGames/HJ_Chess/BWNetworkRoom.cpp \
	/../AllGames/HJ_Chess/BWChessAI.cpp \
	/../AllGames/HJ_Chess/BWChinaChess.cpp \
	/../AllGames/HJ_Chess/BWStateCanju.cpp \
	/../AllGames/HJ_Chess/ChessManager.cpp \
	/../AllGames/HJ_Chess/BWStateStart.cpp \
	/../AllGames/HJ_Chess/MyChessAI.cpp \
	/../AllGames/HJ_Chess/XQWLightAI.cpp \
	/../AllGames/HJ_Chess/ChessAndroid.cpp \
	/../AllGames/HJ_Chess/CreateGame.cpp



LOCAL_LDLIBS    := -llog -lGLESv2
 
     
include $(BUILD_SHARED_LIBRARY)
