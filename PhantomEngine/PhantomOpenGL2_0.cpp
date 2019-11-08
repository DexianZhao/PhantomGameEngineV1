//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 文件       :   PhantomOpenGL2_0.cpp
 幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
 
 Design Writer :   赵德贤 Dexian Zhao
 Email: yuzhou_995@hotmail.com
 
 
 Copyright 2009-2016 Zhao Dexian
 -------------------------------------------------------------------------------------------------
 
 
 -------------------------------------------------------------------------------------------------
 */
//////////////////////////////////////////////////////////////////////////////////////////////////////


#ifdef WIN32
#include <windows.h>
#include <atlimage.h>
extern HWND	GetDeviceWindow();
#endif
#include "PhantomOS.h"
#include "PhantomDevice.h"
#include "PhantomOpenGL.h"
#include "PhantomManager.h"

#ifdef ANDROID_OS
#define	GL_GLEXT_PROTOTYPES
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#elif defined(IPHONE_OS)
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#elif defined(WIN32_OS)
#include <windows.h>
#include <ES2/gl.h>
#include <ES2/glext.h>
#endif

#ifndef DIRECT_X_9
extern "C"{
    VOID    SetupGL2()
    {
#ifndef ANDROID_OS
        Phantom::Device_OGL::s_fnBindVertex = glBindVertexArrayOES;
        Phantom::Device_OGL::s_fnDeleteVertex = glDeleteVertexArraysOES;
        Phantom::Device_OGL::s_fnGenVertex = glGenVertexArraysOES;
#else

#endif
    }
};
namespace Phantom{
};

#endif

