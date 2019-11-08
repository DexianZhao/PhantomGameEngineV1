/*
 * This proprietary software may be used only as
 * authorised by a licensing agreement from ARM Limited
 * (C) COPYRIGHT 2009 - 2011 ARM Limited
 * ALL RIGHTS RESERVED
 * The entire notice above must be reproduced on all authorised
 * copies and copies may only be made to the extent permitted
 * by a licensing agreement from ARM Limited.
 */

#ifndef CUBE_H
#define CUBE_H
#ifndef WINVER
#define WINVER         0x0501
#endif
#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0501
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT   0x0600
#endif
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>

HWND create_window(int uiWidth, int uiHeight);
LRESULT CALLBACK process_window(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);
#else

#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>

#include "EGL/egl.h"

Window create_window(const char *title, int width, int height, Display* display, 
	EGLDisplay sEGLDisplay, EGLConfig FBConfig, Colormap *pColormap, 
	XVisualInfo **ppVisual);
#endif

typedef struct {
	float x, y, z;
} vec3;

#define GL_CHECK(x) \
        x; \
        { \
          GLenum glError = glGetError(); \
          if(glError != GL_NO_ERROR) { \
            fprintf(stderr, "glGetError() = %i (0x%.8x) at line %i\n", glError, glError, __LINE__); \
            exit(1); \
          } \
        }

#define EGL_CHECK(x) \
    x; \
    { \
        EGLint eglError = eglGetError(); \
        if(eglError != EGL_SUCCESS) { \
            fprintf(stderr, "eglGetError() = %i (0x%.8x) at line %i\n", eglError, eglError, __LINE__); \
            exit(1); \
        } \
    }

#endif /* CUBE_H */
