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
#include <EGL/egl.h>
#elif defined(IPHONE_OS)
#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>
#elif defined(WIN32_OS)
#include <windows.h>
#include <ES2/gl.h>
#include <ES2/glext.h>
#endif

namespace Phantom{
	BOOL	g_bSupportOES = true;
	BOOL	IsSupportOES(){
		return g_bSupportOES;
	}
#ifdef ANDROID_OS
	BOOL	InitOES(){
		g_bSupportOES = false;
		return false;
		if(Device_OGL::s_fnBindVertex)return true;
		Device_OGL::s_fnGenVertex = (PFNGLGENVERTEXARRAYSOESPROC) eglGetProcAddress("glGenVertexArrays");
		if(!Device_OGL::s_fnGenVertex)
		{
			g_bSupportOES = false;
			LogInfo("No Support OES");
			return false;
		}
		g_bSupportOES = true;
		LogInfo("Yes Support OES");
		Device_OGL::s_fnBindVertex = (PFNGLBINDVERTEXARRAYOESPROC) eglGetProcAddress("glBindVertexArray");
		Device_OGL::s_fnDeleteVertex = (PFNGLDELETEVERTEXARRAYSOESPROC) eglGetProcAddress("glDeleteVertexArrays");
		return false;
	}
#endif
};

#ifndef DIRECT_X_9


namespace Phantom{
    Phantom::DeviceRenderText_OGL*	g_currentDrawText = 0;
};

#define	OS_L_FRONT			GL_FRONT
#define	OS_L_BACK			GL_BACK
extern "C"
{
    #include "CInclude.h"
#ifdef IPHONE_OS
    //int				DrawTextIOS(const unsigned short* text, int textLength, int fontHeight, int fontStyle, const char* fontName, int*inoutW, int*inoutH, int*outImageW, int*outImageH, unsigned char** ret);
    int				DrawTextIOSA(const char* text, int textLength, int fontHeight, int fontStyle, const char* fontName, unsigned short*inoutW, unsigned short*inoutH, unsigned short*outImageW, unsigned short*outImageH, unsigned char** ret, int nAlignment);
#elif defined(ANDROID_OS)
    //void            DrawTextAndroid(const unsigned short* text, int textLength, int fontHeight, int fontStyle, const char* fontName);
    void			DrawTextAndroidA(const char* text, int textLength, int fontHeight, int fontStyle, const char* fontName, int nTextAlignment);
	void			OnDrawText(int* pixels, int width, int height, int textWidth, int textHeight){
		if(!Phantom::g_currentDrawText)
			return;
		Phantom::g_currentDrawText->OnResetAndroidDevice(pixels, width, height, textWidth, textHeight);
	}
#endif
};

namespace Phantom{
    BOOL                     Device_OGL::s_bIsGL3 = false;//if system support OpenGLES3
    inline BOOL              IsOpenGLES3(){return Device_OGL::s_bIsGL3;}
#ifndef ANDROID_OS
    PBindVertexArray         Device_OGL::s_fnBindVertex = glBindVertexArray;
    PDeleteVertexArrays      Device_OGL::s_fnDeleteVertex = glDeleteVertexArrays;
    PGenVertexArrays         Device_OGL::s_fnGenVertex = glGenVertexArrays;
#else
    PBindVertexArray         Device_OGL::s_fnBindVertex = 0;
    PDeleteVertexArrays      Device_OGL::s_fnDeleteVertex = 0;
    PGenVertexArrays         Device_OGL::s_fnGenVertex = 0;
#endif
    VOID		Device_OGL::ClearError(){while(glGetError() != GL_NO_ERROR){}}
    //Phantom::DeviceRenderText_OGL*	g_currentDrawText = 0;
    BOOL g_bSupportDepthShadowMap = false;
	DeviceShadowMap_OGL::DeviceShadowMap_OGL()
	{
		m_depthBuffer	=	0;
		m_frameBuffer	=	0;
		m_renderBuffer	=	0;
		m_textureBuffer	=	0;
		m_saveFrameBuffer	=	0;
	}
	DeviceShadowMap_OGL::~DeviceShadowMap_OGL()
	{
		OnLostDevice();
	}
	VOID			DeviceShadowMap_OGL::OnResetDevice()
	{CPUTime(DeviceShadowMap_OGL);
		OnLostDevice();
		int status;
		glGenFramebuffers(1, &m_frameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
		//
//		glGenRenderbuffers(1, &m_renderBuffer);
//		glBindRenderbuffer(GL_RENDERBUFFER, m_renderBuffer);
//		glRenderbufferStorage(GL_RENDERBUFFER, GL_R32F, m_imgBuffer.width, m_imgBuffer.height);
//		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_renderBuffer);
//		//
        glGenTextures(1, &m_textureBuffer);
        glBindTexture(GL_TEXTURE_2D, m_textureBuffer);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        g_bSupportDepthShadowMap = false;
        if(g_bSupportDepthShadowMap){
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_imgBuffer.width, m_imgBuffer.height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 0);
            unsigned int err = glGetError();
            if(err!=GL_NO_ERROR)
                g_bSupportDepthShadowMap=false;
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_textureBuffer, 0);
            status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if(status != GL_FRAMEBUFFER_COMPLETE)
            {
                g_bSupportDepthShadowMap = false;
                LogInfo("system->error:frame buffer fail(0x%X),0x%X", status, err);
            }
            else
                g_bSupportDepthShadowMap = true;
        }
        if(!g_bSupportDepthShadowMap){
            glGenRenderbuffers(1, &m_depthBuffer);
            glBindRenderbuffer(GL_RENDERBUFFER, m_depthBuffer);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, m_imgBuffer.width, m_imgBuffer.height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthBuffer);
            //
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_imgBuffer.width, m_imgBuffer.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
            unsigned int err = glGetError();
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textureBuffer, 0);
            status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if(status != GL_FRAMEBUFFER_COMPLETE)
                LogInfo("system->error:frame buffer fail(0x%X),0x%X", status, err);
        }
		//
//		GLuint depthTexture;
//		glGenTextures(1, &depthTexture);
//		glBindTexture(GL_TEXTURE_2D, depthTexture);
//        glTexStorage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, m_imgBuffer.width, m_imgBuffer.height);
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, depthTexture);
//        glGenFramebuffers(1, &m_frameBuffer);
//        glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
//        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
//        GLenum drawBuffers[]={GL_NONE};
//        glDrawBuffers(1, drawBuffers);
//        //		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, m_imgBuffer.width, m_imgBuffer.height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 0);
////		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, depthTexture, 0);
//		status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
//		if(status != GL_FRAMEBUFFER_COMPLETE)
//			LogInfo("system->error:frame buffer fail(0x%X)", status);

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	VOID			DeviceShadowMap_OGL::OnLostDevice()
	{CPUTime(DeviceShadowMap_OGL);
		if(m_frameBuffer)
			glDeleteFramebuffers(1, &m_frameBuffer);
		if(m_textureBuffer)
			glDeleteTextures(1, &m_textureBuffer);
		if(m_depthBuffer)
			glDeleteRenderbuffers(1, &m_depthBuffer);
		if(m_renderBuffer)
			glDeleteRenderbuffers(1, &m_renderBuffer);
		m_frameBuffer	=	0;
		m_renderBuffer	=	0;
		m_depthBuffer	=	0;
	}
	BOOL			DeviceShadowMap_OGL::Begin()
	{CPUTime(DeviceShadowMap_OGL);
		if(m_frameBuffer){
			glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_saveFrameBuffer);
			// 2. RENDER TO OFFSCREEN RENDER TARGET
			glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
			glViewport(0, 0, m_imgBuffer.width, m_imgBuffer.height);
            glClearColor(1.0,1.0,1.0,1.0);
            glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		}
		return true;
	}
	VOID			DeviceShadowMap_OGL::End()
	{CPUTime(DeviceShadowMap_OGL);
		glBindFramebuffer(GL_FRAMEBUFFER, m_saveFrameBuffer);
		m_saveFrameBuffer = 0;
        g_manager.SetViewport(0);
	}

	BOOL	GlErrorOutput()
	{
		GLenum glError = glGetError();
		if(glError != GL_NO_ERROR)
		{
			//LogInfo("error: glGetError() = %i (0x%.8x)\n", glError, glError);
			return false;
		}
		return true;
	}
	Device_OGL*	g_ogl = 0;
	DeviceTexture_OGL::DeviceTexture_OGL()
	{
		m_texID	=	0;
		memset(m_samplerFilter, 0, sizeof(m_samplerFilter));
	}
	DeviceTexture_OGL::~DeviceTexture_OGL()
	{
		m_imgBuffer.FreePixels();
		if(g_manager.IsDeviceSetup())
			OnLostDevice();
	}
	VOID					DeviceTexture_OGL::Destroy()
	{CPUTime(DeviceTexture_OGL);
		Texture::Destroy();
		this->OnLostDevice();
	}
	VOID					DeviceTexture_OGL::OnLoadDevice()
	{CPUTime(DeviceTexture_OGL);
		m_imgBuffer.AutoResize(this->m_fileName.t);
		if(s_bSmallImage||(m_nFilter&0x00020000)>0)
			m_imgBuffer.ToSmallRGB();
		BASE_CHECK_NR(glGenTextures(1, &m_texID));
		BASE_CHECK_NR(glBindTexture(GL_TEXTURE_2D, m_texID));
		int pixelFormat	=	GL_RGB;
		if(m_imgBuffer.bpp == ImageBuffer::PixelFormat_RGBA || m_imgBuffer.bpp == ImageBuffer::PixelFormat_RGBA_4444)
			pixelFormat	=	GL_RGBA;
		int nBytes = GL_UNSIGNED_BYTE;
		if(m_imgBuffer.bpp == ImageBuffer::PixelFormat_RGBA_4444)
			nBytes = GL_UNSIGNED_SHORT_4_4_4_4;
		else if(m_imgBuffer.bpp == ImageBuffer::PixelFormat_RGB_565)
			nBytes = GL_UNSIGNED_SHORT_5_6_5;
        int nFilter = m_nFilter&0x0000ffff;
		if(nFilter==1){
			BASE_CHECK_NR(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
			BASE_CHECK_NR(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
		}
		else{//if(m_nFilter==1){
			BASE_CHECK_NR(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
			BASE_CHECK_NR(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		}
        if((m_nFilter&0x00010000)>0){
            BASE_CHECK_NR(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
            BASE_CHECK_NR(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        }
        else{
            BASE_CHECK_NR(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
            BASE_CHECK_NR(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
        }
		//
		BASE_CHECK_NR(glTexImage2D(GL_TEXTURE_2D, 0, pixelFormat, m_imgBuffer.width, m_imgBuffer.height, 0, pixelFormat, nBytes, m_imgBuffer.pixels));
		if(this->m_bGenMipmap)
		{
			glGenerateMipmap(GL_TEXTURE_2D);
			if(nFilter==1)
			{
				BASE_CHECK_NR(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR));
			}
			else
			{
				BASE_CHECK_NR(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
			}
		}
		m_imgBuffer.FreePixels(false);
		//设置会原来的设备
		glBindTexture(GL_TEXTURE_2D, 0);
        //LogInfo("system-> reset texture ok. %s", this->m_fileName.t);
		Texture::s_maxImageBuffer += m_imgBuffer.GetBufferSize();
		m_loadThread = false;
	}
	VOID					DeviceTexture_OGL::OnResetDevice()
	{CPUTime(DeviceTexture_OGL);
		OnLostDevice();
		if(!LoadTextureData())
		{
			return;
		}
		if(!this->m_imgBuffer.pixels)
			return;
		//
		if(!this->m_loadThread)
			OnLoadDevice();
	}
	VOID					DeviceTexture_OGL::OnLostDevice()
	{CPUTime(DeviceTexture_OGL);
		if(m_texID > 0)
		{
            //LogInfo("system-> ,lost texture %s", this->m_fileName.t);
			glBindTexture(GL_TEXTURE_2D, 0);
			glDeleteTextures(1, &m_texID);
			Texture::s_maxImageBuffer -= m_imgBuffer.GetBufferSize();
			//LogInfo("system-> ,lost ok");
		}
		m_texID	=	0;
	}
	VOID					DeviceTexture_OGL::SetSampler(SamplerState sampler, TextureFilter filter)
	{CPUTime(DeviceTexture_OGL);
		if(m_samplerFilter[sampler] == filter)
			return;
		BASE_CHECK_NR(glBindTexture(GL_TEXTURE_2D, m_texID));
		m_samplerFilter[sampler]	=	filter;
		switch(sampler)
		{
		case SAMP_MAGFILTER:
			{
				int texF	=	GL_NEAREST;
				if(filter == Phantom::TextureFilter_Point)
					texF	=	GL_NEAREST;
				else
					texF	=	GL_LINEAR;
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
			}
			break;
		case SAMP_MINFILTER:
		case SAMP_MIPFILTER:
			{
				int texF	=	GL_NEAREST_MIPMAP_NEAREST;
				if(m_samplerFilter[SAMP_MINFILTER] == TextureFilter_Point)
				{
					if(m_samplerFilter[SAMP_MIPFILTER] == TextureFilter_Point)
						texF	=	GL_NEAREST_MIPMAP_NEAREST;
					else
						texF	=	GL_NEAREST_MIPMAP_LINEAR;
				}
				else
				{
					if(m_samplerFilter[SAMP_MIPFILTER] == TextureFilter_Point)
						texF	=	GL_LINEAR_MIPMAP_NEAREST;
					else
						texF	=	GL_LINEAR_MIPMAP_LINEAR;
				}
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texF);
			}
			break;
		}
		glBindTexture(GL_TEXTURE_2D, 0);
	}


	DeviceRenderText_OGL::DeviceRenderText_OGL()
	{
		m_texID	=	0;
	}
	DeviceRenderText_OGL::~DeviceRenderText_OGL()
	{
		if(g_manager.IsDeviceSetup())
			OnLostDevice();
	}
	extern int	GetNearestSize(int n);
#ifdef ANDROID_OS
	unsigned char*						ToSmallRGBA_Android(unsigned char* pixels, int width, int height)
	{
		unsigned char* temp = (unsigned char*)malloc_s(width*height*sizeof(unsigned char)*2);
		for(int x=0;x<width;x++)
		{
			for(int y=0;y<height;y++)
			{
				unsigned char* tp = temp + (x + y * width) * 2;
				unsigned char* sp = pixels + (x + y * width) * 4;
				int r = ((int)sp[3])/16;
				int g = ((int)sp[2])/16;
				int b = ((int)sp[1])/16;
				int a = ((int)sp[0])/16;
				tp[0] = r + (g*16);
				tp[1] = b + (a*16);
			}
		}
		return temp;
	}
	VOID					DeviceRenderText_OGL::OnResetAndroidDevice(int* pixels, int width, int height, int textWidth, int textHeight)
	{CPUTime(DeviceRenderText_OGL);
		m_realWidth = width;
		m_realHeight = height;
		m_imgWidth = textWidth;
		m_imgHeight = textHeight;
        if(m_texID==0){
            BASE_CHECK_NR(glGenTextures(1, &m_texID));
            BASE_CHECK_NR(glBindTexture(GL_TEXTURE_2D, m_texID));
            BASE_CHECK_NR(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
            BASE_CHECK_NR(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
            BASE_CHECK_NR(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
            BASE_CHECK_NR(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        }
        else
        {
            BASE_CHECK_NR(glBindTexture(GL_TEXTURE_2D, m_texID));
        }
		unsigned char* bits = ToSmallRGBA_Android((unsigned char*)pixels, m_realWidth, m_realHeight);
        BASE_CHECK_NR(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_realWidth, m_realHeight, 0, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, bits));
		free(bits);
        glBindTexture(GL_TEXTURE_2D, 0);
	}
#endif
	VOID					DeviceRenderText_OGL::OnResetDevice()
	{CPUTime(DeviceRenderText_OGL);
		if(m_text.size() == 0)
		{
			m_imgWidth = 0;
			m_imgHeight = 0;
			return;
		}
		Texture::s_maxImageBuffer -= (m_realWidth*m_realHeight*2);
#ifdef IPHONE_OS
        unsigned char* ret = 0;
        //if(m_text.size()>0){
        //    if(!::DrawTextIOS(m_text.GetPtr(), m_text.size()*2, this->m_nFontSize, this->m_nFontStyle, 0,
        //                &this->m_realWidth, &this->m_realHeight, &this->m_imgWidth,&this->m_imgHeight, &ret))
        //        return;
        //}
        //else
        //{
		if(!::DrawTextIOSA(m_text.GetPtr(), m_text.size(), this->m_nFontSize*2, this->m_nFontStyle, this->m_name.t, &this->m_realWidth, &this->m_realHeight, &this->m_imgWidth, &this->m_imgHeight, &ret, this->m_nTextAlignment))
			return;
        //}
        if(m_texID==0){
            BASE_CHECK_NR(glGenTextures(1, &m_texID));
            BASE_CHECK_NR(glBindTexture(GL_TEXTURE_2D, m_texID));
            BASE_CHECK_NR(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
            BASE_CHECK_NR(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
            BASE_CHECK_NR(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
            BASE_CHECK_NR(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        }
        else
        {
            BASE_CHECK_NR(glBindTexture(GL_TEXTURE_2D, m_texID));
        }
		unsigned char* bits = ToSmallRGBA(ret, m_realWidth, m_realHeight);
        BASE_CHECK_NR(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_realWidth, m_realHeight, 0, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, bits));
		free_s(bits);
        glBindTexture(GL_TEXTURE_2D, 0);
        Phantom::free_s(ret);
#elif defined(ANDROID_OS)
		g_currentDrawText = this;
        //if(m_text.size()>0){
        //    DrawTextAndroid(m_text.GetPtr(), m_text.size()*2, this->m_nFontSize>0?m_nFontSize:-m_nFontSize, this->m_nFontStyle,0);
        //}
        //else{
		DrawTextAndroidA(m_text.GetPtr(), m_text.size(), (this->m_nFontSize>0?m_nFontSize:-m_nFontSize)*2, this->m_nFontStyle,m_name.t, m_nTextAlignment);
        //}
		g_currentDrawText = 0;
#endif
		Texture::s_maxImageBuffer += (m_realWidth*m_realHeight*2);
	}
	VOID					DeviceRenderText_OGL::OnLostDevice()
	{CPUTime(DeviceRenderText_OGL);
		if(m_texID > 0)
		{
			glBindTexture(GL_TEXTURE_2D, 0);
			glDeleteTextures(1, &m_texID);
			Texture::s_maxImageBuffer -= (m_realWidth*m_realHeight*2);
			m_realWidth = m_realHeight = 0;
		}
		m_texID	=	0;
		//safe_release(m_texturePtr);
	}

	BOOL	LoadGLShaderText(GLuint* shader, const char* szText, GLint shaderType)
	{
		GLint iStatus;
		*shader					=	BASE_CHECK_RET(glCreateShader(shaderType));
		const char* buf[1];
		buf[0]	=	szText;
		BASE_CHECK_RET(glShaderSource(*shader, 1, buf, NULL));
		BASE_CHECK_RET(glCompileShader(*shader));
		BASE_CHECK_RET(glGetShaderiv(*shader, GL_COMPILE_STATUS, &iStatus));
		if(iStatus != GL_TRUE) {
			GLint iLen;
			char *sDebugSource = NULL;
			char *sErrorLog = NULL;
			BASE_CHECK_RET(glGetShaderiv(*shader, GL_SHADER_SOURCE_LENGTH, &iLen));
			sDebugSource = (char*)malloc(iLen);
			BASE_CHECK_RET(glGetShaderSource(*shader, iLen, NULL, sDebugSource));
#ifdef WIN32
			char buf[GL_SHADER_SOURCE_LENGTH];
			sprintf(buf, "ERROR: \n%s\n", sDebugSource);
			MessageBox(0, buf, "shader failure", 0);
#else//if defined(IPHONE_OS)
            LogInfo("system->shader Error:");
            LogInfo(sDebugSource);
#endif
			GLint logLength;
			glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &logLength);
			if (logLength > 0)
			{
#ifndef WIN32_OS
				GLchar *log = (GLchar *)malloc(logLength);
				glGetShaderInfoLog(*shader, logLength, &logLength, log);
				LogInfo("system->Error Log:%s", log);
				free(log);
#endif
			}
			free(sDebugSource);
			glDeleteShader(*shader);
			*shader	=	0;
			return false;
		}
		return true;
	}

	BOOL	LoadGLShader(GLuint* shader, const char* szFile, GLint shaderType)
	{
		StreamBase*	buffer	=	open_s(szFile, true);
		BOOL ret	=	LoadGLShaderText(shader, buffer->getMemoryPtr(), shaderType);
		close_s(&buffer);
		return ret;
	}
	DeviceProgram_OGL::DeviceProgram_OGL(int mode):Program(mode)
	{
		m_bFail		=	false;
		m_bCreated	=	false;
		m_programID	=	0;
		m_vertID	=	0;
		m_pixelID	=	0;
        m_bOESAttr  =   0;
		//m_varPositions.SetName("inPosition", _shader_var::S_T_Attribuate);
		//m_varColors.SetName("inColor", _shader_var::S_T_Attribuate);
		//m_varNormals.SetName("inNormal", _shader_var::S_T_Attribuate);
		//m_varUVs.SetName("inTexcoord", _shader_var::S_T_Attribuate);
		//m_varUV2s.SetName("inTexcoord2", _shader_var::S_T_Attribuate);
		//m_varUV3s.SetName("inTexcoord3", _shader_var::S_T_Attribuate);
		//m_varSpecular.SetName("inColor1", _shader_var::S_T_Attribuate);
		//m_varUV4s.SetName("inTexcoord4", _shader_var::S_T_Attribuate);
		//m_varUV5s.SetName("inTexcoord5", _shader_var::S_T_Attribuate);
		/*m_varTex[TextureIndex_0].SetName("texture1", _shader_var_name::S_T_Uniform);
		m_varTex[TextureIndex_1].SetName("texture2", _shader_var_name::S_T_Uniform);
		m_varTex[TextureIndex_2].SetName("texture3", _shader_var_name::S_T_Uniform);
		m_varTex[TextureIndex_3].SetName("texture4", _shader_var_name::S_T_Uniform);
		m_varTex[TextureIndex_4].SetName("texture5", _shader_var_name::S_T_Uniform);*/
	}
	DeviceProgram_OGL::~DeviceProgram_OGL()
	{
		if(g_manager.IsDeviceSetup())
			OnLostDevice();
	}
    BOOL				DeviceProgram_OGL::SetPointer(_shader_var& var, int size, int type, const void* ptr, const char* szName){CPUTimeNone();return SetPointer(var, size, type, 0, 0, ptr, szName);}
    BOOL				DeviceProgram_OGL::SetPointer(_shader_var& var, const float3* ptr, const char* szName){CPUTimeNone();return SetPointer(var, 3, GL_FLOAT, ptr, szName); }
    BOOL				DeviceProgram_OGL::SetPointer(_shader_var& var, const color4* ptr, const char* szName){CPUTimeNone();return SetPointer(var, 4, GL_FLOAT, ptr, szName); }
    BOOL				DeviceProgram_OGL::SetPointer(_shader_var& var, const float2* ptr, const char* szName){CPUTimeNone();return SetPointer(var, 2, GL_FLOAT, ptr, szName); }
    BOOL				DeviceProgram_OGL::SetPointer(_shader_var_name& var, int size, int type, const void* ptr){CPUTimeNone();return SetPointer(var, size, type, GL_FALSE, 0, ptr);}
    BOOL				DeviceProgram_OGL::SetPointer(_shader_var_name& var, const float3* ptr){CPUTimeNone();return SetPointer(var, 3, GL_FLOAT, ptr); }
    BOOL				DeviceProgram_OGL::SetPointer(_shader_var_name& var, const color4* ptr){CPUTimeNone();return SetPointer(var, 4, GL_FLOAT, ptr); }
    BOOL				DeviceProgram_OGL::SetPointer(_shader_var_name& var, const float2* ptr){CPUTimeNone();return SetPointer(var, 2, GL_FLOAT, ptr); }
	VOID				DeviceProgram_OGL::OnCreateShader()
	{CPUTimeNone();
		Text vs,ps;
        BOOL bToShadowMap	=	IsProgram(Program::Type_ToShadowMap);
		BOOL bMaterial	=	IsProgram(Program::Type_Material);
		BOOL bUV		=	IsProgram(Program::Type_UV);
		BOOL bNormal	=	IsProgram(Program::Type_Normal);
		BOOL bLight		=	(IsProgram(Program::Type_Light) && bNormal);
		BOOL bColor		=	IsProgram(Program::Type_Color);
		BOOL bBlend		=	this->m_bBlendMode;
		BOOL bAlphaTest	=	IsProgram(Program::Type_AlphaTest)&&bUV;
		BOOL bContrast	=	IsProgram(Program::Type_Contrast);
		BOOL bStaticShadowMap	=	IsProgram(Program::Type_StaticShadowMap);
		BOOL bWithShadowMap		=	IsProgram(Program::Type_WithShadowMap);
		BOOL bUV2		=	bStaticShadowMap;
		BOOL bUseColor	=	(bColor||bUV||bMaterial);
		BOOL bFog		=	IsProgram(Program::Type_Fog)||IsProgram(Program::Type_Fog2D)||IsProgram(Program::Type_Fog2DGray)||IsProgram(Program::Type_Fog3DGray)
			||IsProgram(Program::Type_Fog3DSub)||IsProgram(Program::Type_Fog2DSub);
		bFog = false;
		BOOL bBrightness = IsProgram(Program::Type_Brightness);

        vs >> "attribute vec4	inPosition;";
		if(bNormal)
			vs >> "attribute vec3	inNormal;";
		if(bUV)
			vs >> "attribute vec2	inTexcoord;";
		if(bUV2)
			vs >> "attribute vec2	inTexcoord1;";
		if(bColor)
			vs >> "attribute vec4	inColor;";
        if(bBlend)
        {
            vs.add("uniform mat4	sBones[%d];\r\n", PROGRAM_BONE_COUNT);
            vs >> "attribute vec4	inColor1;";
            vs >> "attribute vec2	inTexcoord2;";
            vs >> "attribute vec2	inTexcoord3;";
        }
		//uniform
		vs >> "uniform mat4	sWorld;";
		vs >> "uniform mat4	sViewProj;";
		if(bToShadowMap){//Normal,inTexcoord之类的设置部分必须保留，即使不使用也不能删除否则在ios上会自动优化掉，所以会造成不存在这个attr导致顶点对齐出问题并崩溃
            //verying
            vs >> "precision highp float;";
            if(bUV)
                vs >> "varying vec2	passCoord;";
            if(!g_bSupportDepthShadowMap)
                vs >> "varying vec2 passDepth;";
            vs >> "void main(){";
            if(bBlend){
                vs >> "ivec4 fBI		=	ivec4(inColor1 * 255.0);";
                vs >> "mat4 m0 		= 	sBones[	fBI[0] ]	*	inTexcoord2.x;";
                vs >> "mat4 m1 		= 	sBones[	fBI[1] ]	*	inTexcoord2.y;";
                vs >> "mat4 m2 		= 	sBones[	fBI[2] ]	*	inTexcoord3.x;";
                vs >> "mat4 m3 		= 	sBones[	fBI[3] ]	*	inTexcoord3.y;";
                vs >> "vec4 fm1		=	vec4(m0[0][0] + m1[0][0] + m2[0][0] + m3[0][0] ,	m0[1][0] + m1[1][0] + m2[1][0] + m3[1][0] ,	m0[2][0] + m1[2][0] + m2[2][0] + m3[2][0] ,	m0[3][0] + m1[3][0] + m2[3][0] + m3[3][0] );";
                vs >> "vec4 fm2		=	vec4(m0[0][1] + m1[0][1] + m2[0][1] + m3[0][1] ,	m0[1][1] + m1[1][1] + m2[1][1] + m3[1][1] ,	m0[2][1] + m1[2][1] + m2[2][1] + m3[2][1] ,	m0[3][1] + m1[3][1] + m2[3][1] + m3[3][1] );";
                vs >> "vec4 fm3		=	vec4(m0[0][2] + m1[0][2] + m2[0][2] + m3[0][2] ,	m0[1][2] + m1[1][2] + m2[1][2] + m3[1][2] ,	m0[2][2] + m1[2][2] + m2[2][2] + m3[2][2] ,	m0[3][2] + m1[3][2] + m2[3][2] + m3[3][2] );";
                vs >> "vec4 pos		=	vec4(dot(inPosition.xyz,	fm1.xyz)	+	fm1.w,	dot(inPosition.xyz,	fm2.xyz)	+	fm2.w,	dot(inPosition.xyz,	fm3.xyz)	+	fm3.w, 1);";
                vs >> "pos				=	sWorld * pos;";
                if(bNormal)
                {
                    vs >> "vec3 Normal	=	vec3(dot(inNormal,	fm1.xyz),	dot(inNormal,	fm2.xyz),	dot(inNormal,	fm3.xyz));";
                    vs >> "Normal			=	normalize(mat3(sWorld) * Normal);";
                }
            }
            else
            {
                vs >> "vec4 pos		=	sWorld * inPosition;";
                if(bNormal)
                    vs >> "vec3 Normal	=	normalize(mat3(sWorld) * inNormal);";
            }
            if(bUV)
                vs >> "passCoord	=	inTexcoord;";
            vs >> "gl_Position	=	sViewProj * pos;";
            if(!g_bSupportDepthShadowMap)
                vs >> "passDepth	=	gl_Position.zw;";
            vs >> "}";
            //pixel vertices
            ps >> "precision highp float;";
            if(bAlphaTest)
            {
                ps >> "varying vec2			passCoord;";
                ps >> "uniform sampler2D	texture1;";
                ps >> "uniform	float	sAlphaConstrast;";
            }
            ps >> "uniform float m_shadowEpsilon;";
            if(!g_bSupportDepthShadowMap){
                ps >> "varying vec2 passDepth;";
                ps >> "vec4 pack(float depth){";
                ps >> "const vec4 bitSh = vec4(256.0*256.0*256.0,256.0*256.0,256.0,1.0);";
                ps >> "const vec4 bitMsk = vec4(0,1.0/256.0,1.0/256.0,1.0/256.0);";
                ps >> "vec4 comp = fract(depth*bitSh);";
                ps >> "comp -= comp.xxyz*bitMsk;";
                ps >> "return comp;";
                ps >> "}";
            }
            ps >> "void main(){";
            if(bAlphaTest){
                ps >> "if((texture2D(texture1, passCoord).a*sAlphaConstrast) < 0.99)discard;";
            }
            if(!g_bSupportDepthShadowMap){
            ps >> "float depth = clamp(passDepth.x / passDepth.y * m_shadowEpsilon,0.0,1.0);";//1.0;";//sShadowEpsilon;";
            ps >> "gl_FragColor = pack(depth);";//vec4(depth,depth,depth,depth);";
            }
            ps >> "}";
		}
		else{
			if(bWithShadowMap)
				vs >> "uniform mat4 sViewToLightProj;";
			if(bFog)
				vs >> "uniform vec3 sFogPos;";
			if(bLight)
				vs >> "uniform vec3	sLightDirInv;";
			if(bMaterial)
			{
				vs >> "uniform vec4	sDiffuse;";
				vs >> "uniform vec3	sEmissive;";
			}
			char buf[256];
			int maxLight = MAX_LIGHT_COUNT;
			if(bLight&&bUseColor){
				vs >> "uniform int		bLightDirect;";//是否存在全局方向光
				vs >> "uniform vec3		sLightDirect;";//[5];";//一个模型最多承受5个灯光的影响
				vs >> "uniform vec3		sLightDirectC;";
				vs >> "uniform vec3		sAmbient;";
				sprintf(buf, "uniform vec4	sLightPos[%d];", maxLight);//一个模型最多承受5个灯光的影响
				vs >> buf;
				sprintf(buf, "uniform vec4	sLightColor[%d];", maxLight);//一个模型最多承受5个灯光的影响
				vs >> buf;
				sprintf(buf, "uniform vec4	sLightRangeA[%d];", maxLight);//一个模型最多承受5个灯光的影响
				vs >> buf;
				vs >> "uniform int		sLightCount;";//灯光数量
			}
			if(bBrightness)
				vs >> "uniform float	sBrightness;";
			//
			//verying
			if(bUseColor)
				vs >> "varying vec4	passColor;";
			if(bUV)
				vs >> "varying vec2	passCoord;";
			if(bUV2)
				vs >> "varying vec2	passCoord1;";
			if(bFog)
				vs >> "varying vec3 passDir;";
			if(bWithShadowMap)
				vs >> "varying vec4 passShadowMap;";
			vs >> "void main(){";
			if(bBlend){
				vs >> "ivec4 fBI		=	ivec4(inColor1 * 255.0);";
				vs >> "mat4 m0 		= 	sBones[	fBI[0] ]	*	inTexcoord2.x;";
				vs >> "mat4 m1 		= 	sBones[	fBI[1] ]	*	inTexcoord2.y;";
				vs >> "mat4 m2 		= 	sBones[	fBI[2] ]	*	inTexcoord3.x;";
				vs >> "mat4 m3 		= 	sBones[	fBI[3] ]	*	inTexcoord3.y;";
				vs >> "vec4 fm1		=	vec4(m0[0][0] + m1[0][0] + m2[0][0] + m3[0][0] ,	m0[1][0] + m1[1][0] + m2[1][0] + m3[1][0] ,	m0[2][0] + m1[2][0] + m2[2][0] + m3[2][0] ,	m0[3][0] + m1[3][0] + m2[3][0] + m3[3][0] );";
				vs >> "vec4 fm2		=	vec4(m0[0][1] + m1[0][1] + m2[0][1] + m3[0][1] ,	m0[1][1] + m1[1][1] + m2[1][1] + m3[1][1] ,	m0[2][1] + m1[2][1] + m2[2][1] + m3[2][1] ,	m0[3][1] + m1[3][1] + m2[3][1] + m3[3][1] );";
				vs >> "vec4 fm3		=	vec4(m0[0][2] + m1[0][2] + m2[0][2] + m3[0][2] ,	m0[1][2] + m1[1][2] + m2[1][2] + m3[1][2] ,	m0[2][2] + m1[2][2] + m2[2][2] + m3[2][2] ,	m0[3][2] + m1[3][2] + m2[3][2] + m3[3][2] );";
				vs >> "vec4 pos		=	vec4(dot(inPosition.xyz,	fm1.xyz)	+	fm1.w,	dot(inPosition.xyz,	fm2.xyz)	+	fm2.w,	dot(inPosition.xyz,	fm3.xyz)	+	fm3.w, 1);";
				vs >> "pos				=	sWorld * pos;";
				if(bNormal)
				{
					vs >> "vec3 Normal	=	vec3(dot(inNormal,	fm1.xyz),	dot(inNormal,	fm2.xyz),	dot(inNormal,	fm3.xyz));";
					vs >> "Normal			=	normalize(mat3(sWorld) * Normal);";
				}
			}
			else
			{
				vs >> "vec4 pos		=	sWorld * inPosition;";
				if(bNormal)
					vs >> "vec3 Normal	=	normalize(mat3(sWorld) * inNormal);";
			}
			if(bFog)
				vs >> "passDir		=	pos.xyz - sFogPos;";
			if(bUV)
				vs >> "passCoord	=	inTexcoord;";
			if(bUV2)
				vs >> "passCoord1	=	inTexcoord1;";
			if(bUseColor){
				if(bMaterial)
					vs >> "passColor	=	sDiffuse;";
				else
					vs >> "passColor.rgba	=	vec4(1.0,1.0,1.0,1.0);";
				if(bColor)
					vs >> "passColor	*=	inColor;";
				if(bLight)
				{
					vs >> "vec3 flight = vec3(0.0,0.0,0.0);";
					//directional
					vs >> "if(bLightDirect>0){";
					vs >> "float fldir = clamp(dot(sLightDirect, Normal),0.0,1.0);";
					vs >> "flight += fldir*sLightDirectC+sAmbient.xyz;";
					vs >> "}";
					//for(int i=0;i<maxLight;i++){
					//	sprintf(buf, "if(sLightCount>%d){", i);
					//	vs >> buf;
					//	sprintf(buf, "%s light = sLightPos[%d].xyz - pos.xyz;", (i==0)?"vec3":"", i);
					//	vs >> buf;
					//	sprintf(buf, "flight	+=	dot( normalize(light), Normal) * (1.0-clamp((length(light)-sLightRangeA[%d].x)/sLightRangeA[%d].y,0.0,1.0)) * sLightColor[%d].xyz;",i,i,i);
					//	vs >> buf;
					//}
					//for(int i=0;i<maxLight;i++){
					//	vs >> "}";
					//}
					vs >> "passColor.rgb *= flight.xyz;";
				}
				if(bMaterial)
					vs >> "passColor.rgb += sEmissive.rgb;";
				if(bBrightness)
					vs >> "passColor.rgb	+=	sBrightness;";
			}
			vs >> "gl_Position	=	sViewProj * pos;";
			if(bWithShadowMap){
				vs >> "vec4 shadowPos = sViewToLightProj*pos;";
				vs >> "shadowPos.xyz  = shadowPos.xyz / shadowPos.w;";
                vs >> "passShadowMap  = shadowPos;";//vec3( 0.5 * shadowPos.x + 0.5, (0.5 * shadowPos.y + 0.5), shadowPos.z);";//vec3( 0.5 * shadowPos.x + 0.5, 1.0 - (0.5 * shadowPos.y + 0.5), shadowPos.z);";
			}
			vs >> "}";
			//pixel vertices
			ps >> "precision mediump float;";
			if(bUseColor)
				ps >> "varying vec4			passColor;";
			if(bUV)
			{
				ps >> "varying vec2			passCoord;";
				ps >> "uniform sampler2D	texture1;";
			}
			if(bUV2)
				ps >> "varying vec2	passCoord1;";
			if(bFog)
				ps >> "varying vec3	passDir;";
			if(bWithShadowMap)
				ps >> "varying vec4 passShadowMap;";
			if(bContrast){
				//ps >> "uniform	float	sBrightness;";
				ps >> "uniform	float	sContrast;";
			}
			if(bAlphaTest)
				ps >> "uniform	float	sAlphaConstrast;";
			if(bStaticShadowMap||bWithShadowMap){//TextureIndex_3
				ps >> "uniform	vec2	shadowTextureSize;";
				ps >> "uniform	float	shadowDensity;";
			}
			if(bStaticShadowMap){
				ps >> "uniform sampler2D	texture4;";
			}
			if(bWithShadowMap){
				ps >> "uniform sampler2D	texture6;";
			}
			if(bFog)
			{
				ps >> "uniform vec3 sFogColor;";
				ps >> "uniform vec2 sFogRange;";//near,far
			}
            if(bWithShadowMap&&!g_bSupportDepthShadowMap){
                ps >> "float unpack(vec4 colour){";
                ps >> "const vec4 bitShifts = vec4(1.0/(256.0*256.0*256.0),1.0/(256.0*256.0),1.0/256.0,1);";
                ps >> "return dot(colour,bitShifts);";
                ps >> "}";
            }
			ps >> "void main(){";
			if(bUseColor){
				ps >> "vec4 ret = passColor;";
				if(bUV)
					ps >> "ret	*= texture2D(texture1, passCoord);";
			}
			else if(bUV)
				ps >> "ret	= texture2D(texture1, passCoord);";
			if(bAlphaTest){
				ps >> "ret.a *= sAlphaConstrast;";
				ps >> "if(ret.a < 0.99)discard;";
			}
			if(bStaticShadowMap)
			{
				if(!(IsProgram(Type_ShadowUsePCF3)||IsProgram(Type_ShadowUsePCF5)))//if(g_manager.m_shadowPCF==Phantom::PCF_0)
					ps >> "ret.rgb	*= texture2D(texture4, passCoord1).r * shadowDensity+(1.0-shadowDensity);";
				else{
					ps >> "float shadow = 0.0;";
					int size = 1;
					if(IsProgram(Type_ShadowUsePCF3))//g_manager.m_shadowPCF==Phantom::PCF_3x3)
						size = 1;
					else if(IsProgram(Type_ShadowUsePCF5))//g_manager.m_shadowPCF==Phantom::PCF_5x5)
						size = 2;
					//else if(g_manager.m_shadowPCF==Phantom::PCF_7x7)
					//	size = 3;
					ps.add("for(int y=-%d;y<=%d;y+=1){\r\n",size,size);
					ps.add("for(int x=-%d;x<=%d;x+=1){\r\n",size,size);
					ps >> "shadow	+=	texture2D(texture4, passCoord1 + vec2(x, y) * shadowTextureSize ).r;";
					ps >> "}}";
					ps.add("ret.rgb *= shadow / %.01f * shadowDensity+(1.0-shadowDensity);\r\n", (float)((size*2+1)*(size*2+1)));
				}
			}
			if(bWithShadowMap){
                ps >> "vec3 shadowPos = passShadowMap.xyz/passShadowMap.w;";//
				if(!(IsProgram(Type_ShadowUsePCF3)||IsProgram(Type_ShadowUsePCF5))){//||!g_bSupportDepthShadowMap){//g_manager.m_shadowPCF==Phantom::PCF_0||!IsProgram(Type_ShadowUsePCF)||!g_bSupportDepthShadowMap){
					ps >> "float shadow	=	1.0;";//
					ps >> "float shadowZ	=	shadowPos.z;";
                    if(g_bSupportDepthShadowMap){
                        ps >> "float textureShadow = texture2D( texture6, vec2( 0.5 * shadowPos.x + 0.5, (0.5 * shadowPos.y + 0.5)) ).z;";
                    }
                    else{
                        ps >> "vec4 cc	=	texture2D( texture6, vec2( 0.5 * shadowPos.x + 0.5, (0.5 * shadowPos.y + 0.5)) );";
                        ps >> "float textureShadow = unpack(cc);";//cc.r+0.0001;";
                    }
                    ps >> "if(shadowZ > textureShadow)shadow = 1.0-shadowDensity;";
					ps >> "ret.rgb	*= shadow;";// * shadowDensity+(1-shadowDensity);";
				}
				else{
					int size = 1;
					if(IsProgram(Type_ShadowUsePCF3))//g_manager.m_shadowPCF==Phantom::PCF_3x3)
						size = 1;
					else if(IsProgram(Type_ShadowUsePCF5))//g_manager.m_shadowPCF==Phantom::PCF_5x5)
						size = 2;
					//else if(g_manager.m_shadowPCF==Phantom::PCF_7x7)
						//size = 3;
					float pcfcount = (float)((size*2+1)*(size*2+1));
					ps.add("float shadow = %.01f;\r\n", pcfcount);
					ps.add("for(float y=-%d.0;y<=%d.0;y+=1.0){\r\n",size,size);
					ps.add("for(float x=-%d.0;x<=%d.0;x+=1.0){\r\n",size,size);
                    if(g_bSupportDepthShadowMap){
                        ps >> "float textureShadow = texture2D( texture6, vec2( 0.5 * shadowPos.x + 0.5, (0.5 * shadowPos.y + 0.5))+ vec2(x, y) * shadowTextureSize ).z;";
                    }
                    else{
                        ps >> "vec4 cc	=	texture2D( texture6, vec2( 0.5 * shadowPos.x + 0.5, (0.5 * shadowPos.y + 0.5))+ vec2(x, y) * shadowTextureSize );";
                        ps >> "float textureShadow = unpack(cc);";//cc.r+0.0001;";
                    }
					ps >> "if(passShadowMap.z > textureShadow)shadow-=1.0;";
					ps >> "}}";
					ps.add("ret.rgb *= shadow / %.01f * shadowDensity+(1.0-shadowDensity);\r\n", pcfcount);
				}
			}
			if(bFog){
				if(IsProgram(Program::Type_Fog2D))
					ps >> "ret.rgb = lerp(ret.rgb, sFogColor.xyz, clamp((length(passDir.xy)-sFogRange.x)/sFogRange.y, 0.0, 1.0));";
				else if(IsProgram(Program::Type_Fog2DGray))
					ps >> "ret.rgb = lerp(ret.rgb, sFogColor.xyz*(ret.r*0.11 + ret.g * 0.59 + ret.b * 0.30), clamp((length(passDir.xy)-sFogRange.x)/sFogRange.y, 0.0, 1.0));";
				else if(IsProgram(Program::Type_Fog3DGray))
					ps >> "ret.rgb = lerp(ret.rgb, sFogColor.xyz*(ret.r*0.11 + ret.g * 0.59 + ret.b * 0.30), clamp((length(passDir)-sFogRange.x)/sFogRange.y, 0.0, 1.0));";
				else if(IsProgram(Program::Type_Fog3DSub))
					ps >> "ret.rgb -= sFogColor.xyz * clamp((length(passDir)-sFogRange.x)/sFogRange.y, 0.0, 1.0);";
				else if(IsProgram(Program::Type_Fog2DSub))
					ps >> "ret.rgb -= sFogColor.xyz * clamp((length(passDir.xy)-sFogRange.x)/sFogRange.y, 0.0, 1.0);";
				else
					ps >> "ret.rgb = lerp(ret.rgb, sFogColor.xyz, clamp((length(passDir)-sFogRange.x)/sFogRange.y, 0.0, 1.0));";
			}
			if(bContrast){
				ps >> "ret.rgb = ((ret.rgb - 0.5) * sContrast) + 0.5;";
				//ps >> "ret.rgb += sBrightness;";
			}
			ps >> "gl_FragColor	=	ret;";
			ps >> "}";
		}
		m_vsps_opengles_v.resize(vs.size()+1);
		strcpy(m_vsps_opengles_v.GetPtr(), vs.str());
		m_vsps_opengles_p.resize(ps.size()+1);
		strcpy(m_vsps_opengles_p.GetPtr(), ps.str());
	}
	VOID				DeviceProgram_OGL::OnResetDevice()	//根据渲染模式创建shader
	{CPUTime(DeviceProgram_OGL);
		Program::OnResetDevice();
		if(m_vsps_opengles_v.size() == 0)
			return;
		OnLostDevice();
        LogInfo("system->shader_init(%s)", this->m_name.t);
		m_bFail		=	true;
		m_resetID++;
		if(!LoadGLShaderText(&m_vertID, this->m_vsps_opengles_v.GetPtr(), GL_VERTEX_SHADER))
        {
            LogInfo("system->error: v shader init %s", this->m_name.t);
            return;
        }
		if(!LoadGLShaderText(&m_pixelID, this->m_vsps_opengles_p.GetPtr(), GL_FRAGMENT_SHADER))
        {
            LogInfo("system->error: p shader init %s", this->m_name.t);
            return;
        }
		m_programID = BASE_CHECK_NR(glCreateProgram());
		BASE_CHECK_NR(glAttachShader(m_programID, m_vertID));
		BASE_CHECK_NR(glAttachShader(m_programID, m_pixelID));
		BASE_CHECK_NR(glLinkProgram(m_programID));
		m_bFail		=	false;
		m_bCreated	=	true;
		LogInfo("system->shader init ok");
#ifndef _DEBUG
		m_vsps_directx9.clear();
		m_vsps_opengles_v.clear();
		m_vsps_opengles_p.clear();
#endif
	}
	VOID				DeviceProgram_OGL::SetTextTexture(TextureIndex index, RenderText* tex)
	{CPUTimeNone();
		if(tex)
		{
			switch(index)
			{
			case TextureIndex_0:
				glActiveTexture(GL_TEXTURE0);
				break;
			case TextureIndex_1:
				glActiveTexture(GL_TEXTURE1);
				break;
			case TextureIndex_2:
				glActiveTexture(GL_TEXTURE2);
				break;
			case TextureIndex_3:
				glActiveTexture(GL_TEXTURE3);
				break;
			case TextureIndex_4:
				glActiveTexture(GL_TEXTURE4);
				break;
			case TextureIndex_5:
				glActiveTexture(GL_TEXTURE5);
				break;
			}
		}
		if(tex)
			glBindTexture(GL_TEXTURE_2D, static_cast<DeviceRenderText_OGL*>(tex)->m_texID);
		else
			glBindTexture(GL_TEXTURE_2D, 0);
		static const char* varTex[6] = {"texture1","texture2","texture3","texture4","texture5","texture6"};
		SetInt(m_varTex[index], index, varTex[index]);
	}
	VOID				DeviceProgram_OGL::SetTexture(TextureIndex index, Texture* tex)
	{CPUTimeNone();
		m_varTextPtr[index]	=	tex;
		if(tex)
		{
			switch(index)
			{
			case TextureIndex_0:
				glActiveTexture(GL_TEXTURE0);
				break;
			case TextureIndex_1:
				glActiveTexture(GL_TEXTURE1);
				break;
			case TextureIndex_2:
				glActiveTexture(GL_TEXTURE2);
				break;
			case TextureIndex_3:
				glActiveTexture(GL_TEXTURE3);
				break;
			case TextureIndex_4:
				glActiveTexture(GL_TEXTURE4);
				break;
			case TextureIndex_5:
				glActiveTexture(GL_TEXTURE5);
				break;
			}
		}
		if(tex)
		{
			if(tex->GetType() == 0)
				glBindTexture(GL_TEXTURE_2D, static_cast<DeviceTexture_OGL*>(tex)->m_texID);
			else
				glBindTexture(GL_TEXTURE_2D, static_cast<DeviceShadowMap_OGL*>(tex)->m_textureBuffer);
		}
		else
			glBindTexture(GL_TEXTURE_2D, 0);
		static const char* varTex[6] = {"texture1","texture2","texture3","texture4","texture5","texture6"};
		SetInt(m_varTex[index], index, varTex[index]);
	}
	BOOL					DeviceProgram_OGL::SetFloat(_shader_var& var, float f, const char* szName)
	{CPUTimeNone();
		if(!FindVariant(var, szName))
			return false;
		BASE_CHECK_RET(glUniform1f(var.handle, f));
		return true;
	}
	BOOL					DeviceProgram_OGL::SetFloat2(_shader_var& var, const Phantom::float2& v, const char* szName)
	{CPUTimeNone();
		if(!FindVariant(var, szName))
			return false;
		BASE_CHECK_RET(glUniform2f(var.handle, v.x, v.y));
		return true;
	}
	BOOL					DeviceProgram_OGL::SetFloat3(_shader_var& var, const Phantom::float3& v, const char* szName)
	{CPUTimeNone();
		if(!FindVariant(var, szName))
			return false;
		BASE_CHECK_RET(glUniform3f(var.handle, v.x, v.y, v.z));
		return true;
	}
	BOOL					DeviceProgram_OGL::SetFloat4(_shader_var& var, const Phantom::float4& v, const char* szName)
	{CPUTimeNone();
		if(!FindVariant(var, szName))
			return false;
		BASE_CHECK_RET(glUniform4f(var.handle, v.x, v.y, v.z, v.w));
		return true;
	}
	BOOL					DeviceProgram_OGL::SetFloatPtr(_shader_var& var, const float* f, int nCount, const char* szName)
	{CPUTimeNone();
		if(!FindVariant(var, szName))
			return false;
		BASE_CHECK_RET(glUniform1fv(var.handle, nCount, f));
		return true;
	}
	BOOL					DeviceProgram_OGL::SetMatrixPtr(_shader_var& var, const Phantom::matrix4x4* v, int nCount, const char* szName)
	{CPUTimeNone();
		if(!FindVariant(var, szName))
			return false;
		BASE_CHECK_RET(glUniformMatrix4fv(var.handle, nCount, GL_FALSE, (float*)v));
		return true;
	}
	BOOL					DeviceProgram_OGL::SetFloat2Ptr(_shader_var& var, const Phantom::float2* v, int nCount, const char* szName)
	{CPUTimeNone();
		if(!FindVariant(var, szName))
			return false;
		BASE_CHECK_RET(glUniform2fv(var.handle, nCount, (float*)v));
		return true;
	}
	BOOL					DeviceProgram_OGL::SetFloat3Ptr(_shader_var& var, const Phantom::float3* v, int nCount, const char* szName)
	{CPUTimeNone();
		if(!FindVariant(var, szName))
			return false;
		BASE_CHECK_RET(glUniform3fv(var.handle, nCount, (float*)v));
		return true;
	}
	BOOL					DeviceProgram_OGL::SetFloat4Ptr(_shader_var& var, const Phantom::float4* v, int nCount, const char* szName)
	{CPUTimeNone();
		if(!FindVariant(var, szName))
			return false;
		BASE_CHECK_RET(glUniform4fv(var.handle, nCount, (float*)v));
		return true;
	}

	BOOL			DeviceProgram_OGL::FindAttrib(_shader_var& out, const char* szVariantName)//查找attribute shader元素
	{CPUTimeNone();
		if(m_programID <= 0 || out.is_findFail())
			return false;
		if(out.programID == m_programID && out.programID > 0 && out.is_findOK() && out.resetID == m_resetID)
			return true;
		out.resetID		=	m_resetID;
		out.programID	=	m_programID;
		while(glGetError() != GL_NO_ERROR){}
		int nIndex	=	glGetAttribLocation(m_programID, szVariantName);
		GLenum glError = glGetError();
		if(glError == GL_NO_ERROR && nIndex >= 0)
		{
			out.set_handle(nIndex);
			return true;
		}
		else
			out.set_null();
		return false;
	}
	BOOL			DeviceProgram_OGL::FindVariant(_shader_var& out, const char* szVariantName)//查找uniform shader元素
	{CPUTimeNone();
		if(m_programID <= 0 || out.is_findFail())
			return false;
		if(out.programID == m_programID && out.programID > 0 && out.is_findOK() && out.resetID == m_resetID)
			return true;
		out.resetID		=	m_resetID;
		out.programID	=	m_programID;
		while(glGetError() != GL_NO_ERROR){}
		int nIndex	=	glGetUniformLocation(m_programID, szVariantName);
		GLenum glError = glGetError();
		if(glError == GL_NO_ERROR && nIndex >= 0)
		{
			out.set_handle(nIndex);
			return true;
		}
		else
			out.set_null();
		return false;
	}
	BOOL			DeviceProgram_OGL::Active()
	{CPUTimeNone();
		BASE_CHECK_RET(glUseProgram(GetProgramID()));
		return true;
	}
	BOOL			DeviceProgram_OGL::SetPointerEnable(_shader_var& var, BOOL bEnable, const char* szName)
	{CPUTimeNone();
		if(!FindAttrib(var, szName))
			return false;
		if(bEnable)
		{
			BASE_CHECK_RET(glEnableVertexAttribArray(var.handle));
		}
		else
		{
			BASE_CHECK_RET(glDisableVertexAttribArray(var.handle));
		}

		return true;
	}
	BOOL			DeviceProgram_OGL::SetPointer(_shader_var& var, int size, int type, int normalized, int stride, const void* ptr, const char* szName)
	{CPUTimeNone();
		//if(var.st != _shader_var::S_T_Attribuate)
		//	return false;
		if(!FindAttrib(var, szName))
			return false;
		BASE_CHECK_RET(glEnableVertexAttribArray(var.handle));
		BASE_CHECK_RET(glVertexAttribPointer(var.handle, size, type, normalized, stride, ptr));
		return true;
	}
	BOOL			DeviceProgram_OGL::SetPointer(_shader_var_name& var, int size, int type, int normalized, int stride, const void* ptr)
	{CPUTimeNone();
		//if(var.st != _shader_var::S_T_Attribuate)
		//	return false;
		if(!FindAttrib(var, var.name))
			return false;
		BASE_CHECK_RET(glEnableVertexAttribArray(var.handle));
		BASE_CHECK_RET(glVertexAttribPointer(var.handle, size, type, normalized, stride, ptr));
		return true;
	}
	BOOL			DeviceProgram_OGL::SetInt(_shader_var& var, int f, const char* szName)
	{CPUTimeNone();
		if(!FindVariant(var, szName))
			return false;
		BASE_CHECK_RET(glUniform1i(var.handle, f));
		return true;
	}
	BOOL			DeviceProgram_OGL::SetInt(_shader_var_name& var, int i)
	{CPUTimeNone();
		//if(var.st != _shader_var_name::S_T_Uniform)
		//	return false;
		if(!FindVariant(var, var.name))
			return false;
		BASE_CHECK_RET(glUniform1i(var.handle, i));
		return true;
	}
	BOOL			DeviceProgram_OGL::SetMatrix(_shader_var& var, const Phantom::matrix4x4* mat, const char* szName)
	{CPUTimeNone();
		if(!FindVariant(var, szName))
			return false;
		BASE_CHECK_RET(glUniformMatrix4fv(var.handle, 1, GL_FALSE, mat->f));
		return true;
	}
	BOOL			DeviceProgram_OGL::SetMatrix(_shader_var_name& var, const Phantom::matrix4x4* mat)
	{CPUTimeNone();
		//if(var.st != _shader_var_name::S_T_Uniform)
		//	return false;
		if(!FindVariant(var, var.name))
			return false;
		BASE_CHECK_RET(glUniformMatrix4fv(var.handle, 1, GL_FALSE, mat->f));
		return true;
	}

	void			DeviceProgram_OGL::OnLostDevice()
	{CPUTimeNone();
		for(int i=0;i<TextureIndex_Max;i++)
			m_varTextPtr[i]	=	0;
		if(m_pixelID > 0)
			glDeleteShader(m_pixelID);
		m_pixelID	=	0;
		if(m_vertID > 0)
			glDeleteShader(m_vertID);
		m_vertID	=	0;
		//
		m_bFail		=	false;
		m_bCreated	=	false;
		if(m_programID > 0)
			glDeleteProgram(m_programID);
		m_programID	=	0;
	}

	DeviceMesh_OGL::DeviceMesh_OGL()
	{
		m_nStrideSize	=	0;	//顶点宽度
		m_nNumIndices	=	0;
		m_vertexBuffer	=	-1;
        m_meshOES       =   -1;
	}
	DeviceMesh_OGL::~DeviceMesh_OGL()
	{
		if(g_manager.IsDeviceSetup())
			OnLostDevice();
		for(int i=0;i<m_meshAttribs.size();i++)
		{
			if(m_meshAttribs[i])
				delete m_meshAttribs[i];
		}
		m_meshAttribs.clear();
	}
	VOID					DeviceMesh_OGL::Destroy()
	{CPUTime(DeviceMesh_OGL);
		DeviceMesh::Destroy();
		this->OnLostDevice();
	}
	VOID							DeviceMesh_OGL::WriteIndexBuffer(const unsigned short* indexPtr, int indexCount, unsigned char* attrib)
	{CPUTime(DeviceMesh_OGL);
		DeviceMesh::WriteIndexBuffer(indexPtr, indexCount, attrib);
		for(int i=0;i<m_meshAttribs.size();i++)
		{
			if(!m_meshAttribs[i])continue;
			if(m_meshAttribs[i]->indexBuffer >= 0)
				glDeleteBuffers(1, (GLuint*)&m_meshAttribs[i]->indexBuffer);
			delete m_meshAttribs[i];
		}
		this->m_nNumIndices		=	indexCount;
		m_meshAttribs.clear();
		//
		if(!attrib)
		{
			OGLMeshAttribute* attr	=	new OGLMeshAttribute();
			m_meshAttribs.push_back(attr);
			attr->faces.SetArrayCount(indexCount / 3);
			memcpy(attr->faces.GetPtr(), indexPtr, sizeof(Triangle) * attr->faces.size());
			attr->numFaces = attr->faces.size();
		}
		else
		{
			int faceCount		=	indexCount / 3;
			Triangle* triSrc	=	(Triangle*)indexPtr;
			for(int i=0;i<faceCount;i++)
			{
				int index	=	attrib[i];
				if(m_meshAttribs.size() <= index)
					m_meshAttribs.SetArrayCount(index + 1);
				if(!m_meshAttribs[index])
					m_meshAttribs[index]	=	new OGLMeshAttribute();
				OGLMeshAttribute* attr	=	m_meshAttribs[index];
				attr->faces.push_back(triSrc[i]);
			}
			for(int i=0;i<m_meshAttribs.size();i++)
			{
				if(m_meshAttribs[i])
					m_meshAttribs[i]->numFaces = m_meshAttribs[i]->faces.size();
			}
		}
	}
	BOOL	AddElement(CDynamicArray<OGLElement,short,0>& rets, int& offset, int type, int usage, int usageIndex)
	{
		OGLElement el;
		//
		el.offset		=	offset;
		el.usage		=	usage;
		el.usageIndex	=	usageIndex;
		switch(type)
		{
		case VertexElement::VE_TYPE_FLOAT1:
			el.type			=	GL_FLOAT;
			el.typeCount	=	1;
			offset	+=	sizeof(float);
			break;
		case VertexElement::VE_TYPE_FLOAT2:
			el.type			=	GL_FLOAT;
			el.typeCount	=	2;
			offset	+=	sizeof(float) * 2;
			break;
		case VertexElement::VE_TYPE_FLOAT3:
			el.type			=	GL_FLOAT;
			el.typeCount	=	3;
			offset	+=	sizeof(float) * 3;
			break;
		case VertexElement::VE_TYPE_FLOAT4:
			el.type			=	GL_FLOAT;
			el.typeCount	=	4;
			offset	+=	sizeof(float) * 4;
			break;
		case VertexElement::VE_TYPE_D3DCOLOR:
			el.type			=	GL_UNSIGNED_BYTE;
			el.typeCount	=	4;
			el.normalized	=	true;
			offset			+=	sizeof(unsigned int);
			break;
		case VertexElement::VE_TYPE_UBYTE4:
			el.type			=	GL_UNSIGNED_BYTE;
			el.typeCount	=	4;
			el.normalized	=	false;
			offset	+=	sizeof(char) * 4;
			break;
		case VertexElement::VE_TYPE_SHORT2:
			el.type			=	GL_SHORT;
			el.typeCount	=	2;
			offset	+=	sizeof(short) * 2;
			break;
		case VertexElement::VE_TYPE_SHORT4:
			el.type			=	GL_SHORT;
			el.typeCount	=	4;
			offset	+=	sizeof(short) * 4;
			break;
		default:
			return false;
		}
		rets.push_back(el);
		return true;
	}
	BOOL	CreateElements(CDynamicArray<OGLElement,short,0>& rets, VertexElement* elements, int elementCount)
	{
		rets.SetArrayCount(0);
		int offset			=	0;
		for(int i=0;i<elementCount;i++)
		{
			if(!AddElement(rets, offset, elements[i].type, elements[i].usage, elements[i].usageIndex))
				return true;
		}
		//if(bHasTBN)
		//{
		//	AddElement(rets, offset, VertexElement::VE_TYPE_FLOAT3, VertexElement::VE_USAGE_TANGENT, 0);
		//	AddElement(rets, offset, VertexElement::VE_TYPE_FLOAT3, VertexElement::VE_USAGE_BINORMAL, 0);
		//}
		return true;
	}
	VOID	FillElemenetByFVF(CDynamicArray<OGLElement,short,0>& rets, INT nFVF)
	{
		rets.clear();
		int	eleIndex	=	0;
		int offset		=	0;
		if(nFVF & FVF_XYZ)
			AddElement(rets, offset, VertexElement::VE_TYPE_FLOAT3, VertexElement::VE_USAGE_POSITION, 0);
		if(nFVF & FVF_NORMAL)
			AddElement(rets, offset, VertexElement::VE_TYPE_FLOAT3, VertexElement::VE_USAGE_NORMAL, 0);
		if(nFVF & FVF_DIFFUSE)
			AddElement(rets, offset, VertexElement::VE_TYPE_D3DCOLOR, VertexElement::VE_USAGE_COLOR, 0);
		//
		BOOL t1 = false;
		BOOL t2 = false;
		BOOL t3 = false;
		BOOL t4 = false;
		BOOL t5 = false;
		if((nFVF & FVF_TEX5) == FVF_TEX5)
			t1 = t2 = t3 = t4=t5=true;
		else if((nFVF & FVF_TEX4) == FVF_TEX4)
			t1 = t2 = t3 = t4=true;
		else if((nFVF & FVF_TEX3) == FVF_TEX3)
			t1 = t2 = t3 = true;
		else if((nFVF & FVF_TEX2) == FVF_TEX2)
			t1 = t2 = true;
		else if((nFVF & FVF_TEX1) == FVF_TEX1)
			t1	=	true;
		if(t1)
			AddElement(rets, offset, VertexElement::VE_TYPE_FLOAT2, VertexElement::VE_USAGE_TEXCOORD, 0);
		if(t2)
			AddElement(rets, offset, VertexElement::VE_TYPE_FLOAT2, VertexElement::VE_USAGE_TEXCOORD, 1);
		if(t3)
			AddElement(rets, offset, VertexElement::VE_TYPE_FLOAT2, VertexElement::VE_USAGE_TEXCOORD, 2);
		if(t4)
			AddElement(rets, offset, VertexElement::VE_TYPE_FLOAT2, VertexElement::VE_USAGE_TEXCOORD, 3);
		if(t5)
			AddElement(rets, offset, VertexElement::VE_TYPE_FLOAT2, VertexElement::VE_USAGE_TEXCOORD, 4);
	}
	VOID					DeviceMesh_OGL::Create(INT nNumVertices, INT nNumFaces, VertexElement* elements, int elementCount)
	{CPUTime(DeviceMesh_OGL);
		Destroy();
		SetVertexDecl(elements, elementCount);
		m_nStrideSize	=	GetElementWidth();
		m_nNumIndices	=	nNumFaces * 3;
		m_nNumVertices	=	nNumVertices;
		if(m_nNumVertices > 0)
			m_vertices.SetArrayCount(m_nNumVertices * m_nStrideSize);
		if(m_nNumVertices > 0)
			m_indices.SetArrayCount(m_nNumIndices * sizeof(WORD));
		//FillElemenetByFVF(m_elementAttrs, m_nFVF);
	}
	VOID					DeviceMesh_OGL::SetVertexDecl(VertexElement* elements, int elementCount)
	{CPUTime(DeviceMesh_OGL);
		DeviceMesh::SetVertexDecl(elements, elementCount);
		CreateElements(m_elementAttrs, elements, elementCount);
	}
	
	VOID					DeviceMesh_OGL::UpdateIndexBuffer(const unsigned short* indexPtr, int indexCount)
	{CPUTime(DeviceMesh_OGL);
		if(m_meshAttribs.size() <= 0)return;
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_meshAttribs[0]->indexBuffer);
		m_meshAttribs[0]->numFaces = (indexCount / 3);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned short), (const void*)indexPtr, GL_STATIC_DRAW);
	}
	VOID					DeviceMesh_OGL::OnResetDevice()
	{CPUTime(DeviceMesh_OGL);
#ifdef ANDROID_OS
		InitOES();
#endif
		OnLostDevice();
		if(m_vertices.size() <= 0 || m_meshAttribs.size() <= 0)
			return;
		if(!IsSupportOES()){
			glGenBuffers(1, (GLuint*)&m_vertexBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
			glBufferData(GL_ARRAY_BUFFER, this->m_nNumVertices * this->m_nStrideSize, m_vertices.GetPtr(), GL_STATIC_DRAW);
			for(int i=0;i<m_meshAttribs.size();i++)
			{
				OGLMeshAttribute* attr	=	m_meshAttribs[i];
				if(!attr)continue;
				glGenBuffers(1, (GLuint*)&attr->indexBuffer);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, attr->indexBuffer);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, attr->faces.size() * sizeof(Triangle), (const void*)attr->faces.GetPtr(), GL_STATIC_DRAW);
			}
		}
	}
	VOID					DeviceMesh_OGL::OnLostDevice()
	{CPUTime(DeviceMesh_OGL);
        m_vertexBuffer	=	-1;
        for(int i=0;i<m_meshAttribs.size();i++)
        {
            OGLMeshAttribute* attr	=	m_meshAttribs[i];
			if(!attr)continue;
            if(attr->indexBuffer >= 0)
                ::glDeleteBuffers(1, (GLuint*)&attr->indexBuffer);
            attr->indexBuffer	=	-1;
        }
		if(m_vertexBuffer >= 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			::glDeleteBuffers(1, (GLuint*)&m_vertexBuffer);
		}
		if(IsSupportOES()){
			if(m_meshOES>=0)
			{
				(Device_OGL::s_fnDeleteVertex)(1, (GLuint*)&m_meshOES);
				m_meshOES = -1;
			}
		}
	}
	BOOL					DeviceMesh_OGL::RenderMesh(Program* prog, Texture* tex, const Phantom::matrix4x4* mat)
	{CPUTime(DeviceMesh_OGL);
		if(!IsSupportOES()){
			if(m_vertexBuffer < 0)
				return false;
		}
		if(mat)
			g_manager.SetWorldMatrix(mat);
		prog->SetTexture(TextureIndex_0, tex);
		RenderSubset(prog, 0);
		return false;
	}
    VOID                    DeviceMesh_OGL::SetupOES(DeviceProgram_OGL* prog)
    {CPUTime(DeviceMesh_OGL);
		if(!IsSupportOES())
			return;
        if(m_meshOES<0){
            (Device_OGL::s_fnGenVertex)(1, (GLuint*)&m_meshOES);
            (Device_OGL::s_fnBindVertex)(m_meshOES);
			glGenBuffers(1, (GLuint*)&m_vertexBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
			glBufferData(GL_ARRAY_BUFFER, this->m_nNumVertices * this->m_nStrideSize, m_vertices.GetPtr(), GL_STATIC_DRAW);
            for(int i=0;i<m_meshAttribs.size();i++){
                OGLMeshAttribute* attr	=	m_meshAttribs[i];
				if(!attr)continue;
                glGenBuffers(1, (GLuint*)&attr->indexBuffer);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, attr->indexBuffer);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, attr->faces.size() * sizeof(Triangle), (const void*)attr->faces.GetPtr(), GL_STATIC_DRAW);
            }
			//
			for(int i=0;i<m_elementAttrs.size();i++)
			{
        		OGLElement& el		=	m_elementAttrs[i];
        		const char* szUsage	=	g_ogl->m_usageName[el.usage][el.usageIndex].t;
        		prog->SetPointer(el.var, el.typeCount, el.type, el.normalized, m_nStrideSize, (GLvoid*)el.offset, szUsage);
			}
            prog->m_bOESAttr=true;
			(Device_OGL::s_fnBindVertex)(0);
			LogInfo("SetupOES:OK");
        }
    }
	BOOL					DeviceMesh_OGL::RenderSubset(Program* p, DWORD dwAttrib)
	{CPUTime(DeviceMesh_OGL);
		if(!p || !p->IsCreated())return false;
		if(!IsSupportOES()){
			if(m_vertexBuffer < 0 || dwAttrib >= m_meshAttribs.size())
				return false;
			//
			OGLMeshAttribute* attr	=	m_meshAttribs[dwAttrib];
			if(!attr)return false;
			if(attr->indexBuffer < 0 || attr->numFaces <= 0)
				return false;
			DeviceProgram_OGL* prog	=	static_cast<DeviceProgram_OGL*>(p);
			//prog->SetInt(prog->m_varAlphaTest, (g_ogl->GetAlphaMode() == AlphaMode_Test));
			glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, attr->indexBuffer);
			//
			for(int i=0;i<m_elementAttrs.size();i++)
			{
				OGLElement& el		=	m_elementAttrs[i];
				const char* szUsage	=	g_ogl->m_usageName[el.usage][el.usageIndex].t;
				prog->SetPointer(el.var, el.typeCount, el.type, el.normalized, m_nStrideSize, (GLvoid*)el.offset, szUsage);
			}
			int numFaces	=	attr->numFaces;//faces.size();
			GetDevicePtr()->AddRenderTriangle(numFaces);
			glDrawElements(GL_TRIANGLES, numFaces * 3, GL_UNSIGNED_SHORT, 0);
			if(g_ogl->GetCullMode() == Phantom::CullMode_Double)
			{
				GetDevicePtr()->AddRenderTriangle(numFaces);
				glCullFace(OS_L_BACK);
				glDrawElements(GL_TRIANGLES, numFaces * 3, GL_UNSIGNED_SHORT, 0);
				glCullFace(OS_L_FRONT);
			}
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			for(int i=0;i<m_elementAttrs.size();i++)
			{
				OGLElement& el		=	m_elementAttrs[i];
				const char* szUsage	=	g_ogl->m_usageName[el.usage][el.usageIndex].t;
				prog->SetPointerEnable(el.var, false, szUsage);
			}
		}
		else
		{
			if(this->m_nNumVertices == 0)
				return false;
			DeviceProgram_OGL* prog	=	static_cast<DeviceProgram_OGL*>(p);
			//prog->SetInt(prog->m_varAlphaTest, (g_ogl->GetAlphaMode() == AlphaMode_Test));
			if(m_meshOES<0){
				SetupOES(prog);
			}
			(Device_OGL::s_fnBindVertex)(this->m_meshOES);
	//        if(!prog->m_bOESAttr){
	//            for(int i=0;i<m_elementAttrs.size();i++)
	//            {
	//                OGLElement& el		=	m_elementAttrs[i];
	//                const char* szUsage	=	g_ogl->m_usageName[el.usage][el.usageIndex].t;
	//                prog->SetPointer(el.var, el.typeCount, el.type, el.normalized, m_nStrideSize, (GLvoid*)el.offset, szUsage);
	//            }
	//            prog->m_bOESAttr=true;
	//        }
			if(dwAttrib < m_meshAttribs.size())
			{
				OGLMeshAttribute* attr	=	m_meshAttribs[dwAttrib];
				if(attr){
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, attr->indexBuffer);
				int numFaces	=	attr->numFaces;//faces.size();
				GetDevicePtr()->AddRenderTriangle(numFaces);
				glDrawElements(GL_TRIANGLES, numFaces * 3, GL_UNSIGNED_SHORT, 0);
				}
			}
			else
			{
				glDrawArrays(GL_TRIANGLES, 0, m_nNumVertices);
				GetDevicePtr()->AddRenderTriangle(m_nNumVertices/3);
			}
			if(g_ogl->GetCullMode() == Phantom::CullMode_Double)
			{
				glCullFace(OS_L_BACK);
				if(dwAttrib < m_meshAttribs.size())
				{
					OGLMeshAttribute* attr	=	m_meshAttribs[dwAttrib];
					if(attr){
					int numFaces	=	attr->numFaces;//faces.size();
					GetDevicePtr()->AddRenderTriangle(numFaces);
					glDrawElements(GL_TRIANGLES, numFaces * 3, GL_UNSIGNED_SHORT, 0);
					}
				}
				else
				{
					glDrawArrays(GL_TRIANGLES, 0, m_nNumVertices);
					GetDevicePtr()->AddRenderTriangle(m_nNumVertices/3);
				}
				glCullFace(OS_L_FRONT);
			}
			(Device_OGL::s_fnBindVertex)(0);
		}
		return true;
	}
	VOID					DeviceMesh_OGL::UpdateVertexBuffer(const void* vertexPtr, int vertexCount)
    {CPUTime(DeviceMesh_OGL);
        if(m_vertexBuffer>=0){
            glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
			if(vertexPtr)
				glBufferData(GL_ARRAY_BUFFER, vertexCount * this->m_nStrideSize, vertexPtr, GL_STATIC_DRAW);
			else
				glBufferData(GL_ARRAY_BUFFER, this->m_nNumVertices * this->m_nStrideSize, m_vertices.GetPtr(), GL_STATIC_DRAW);
        }
    }
	BOOL					DeviceMesh_OGL::RenderIndexed(Program* p, int numTriangles)
    {CPUTime(DeviceMesh_OGL);
		if(this->m_nNumVertices == 0)
			return false;
        DeviceProgram_OGL* prog	=	static_cast<DeviceProgram_OGL*>(p);
		//prog->SetInt(prog->m_varAlphaTest, (g_ogl->GetAlphaMode() == AlphaMode_Test));
		if(IsSupportOES()){
			if(m_meshOES<0)
				SetupOES(prog);
			(Device_OGL::s_fnBindVertex)(this->m_meshOES);
		}
		else{
			glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
			for(int i=0;i<m_elementAttrs.size();i++)
			{
				OGLElement& el		=	m_elementAttrs[i];
				const char* szUsage	=	g_ogl->m_usageName[el.usage][el.usageIndex].t;
				prog->SetPointer(el.var, el.typeCount, el.type, el.normalized, m_nStrideSize, (GLvoid*)el.offset, szUsage);
			}
		}
        if(0 < m_meshAttribs.size())
        {
            OGLMeshAttribute* attr	=	m_meshAttribs[0];
			if(!attr)return false;
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, attr->indexBuffer);
            int numFaces	=	numTriangles;//attr->numFaces;//faces.size();
            GetDevicePtr()->AddRenderTriangle(numFaces);
            glDrawElements(GL_TRIANGLES, numFaces * 3, GL_UNSIGNED_SHORT, 0);
        }
        else
        {
            glDrawArrays(GL_TRIANGLES, 0, m_nNumVertices);
            GetDevicePtr()->AddRenderTriangle(m_nNumVertices/3);
        }
		if(g_ogl->GetCullMode() == Phantom::CullMode_Double)
		{
            glCullFace(OS_L_BACK);
            if(0 < m_meshAttribs.size())
            {
                OGLMeshAttribute* attr	=	m_meshAttribs[0];
				if(!attr)return false;
                int numFaces	=	numTriangles;//attr->numFaces;//faces.size();
                GetDevicePtr()->AddRenderTriangle(numFaces);
                glDrawElements(GL_TRIANGLES, numFaces * 3, GL_UNSIGNED_SHORT, 0);
            }
            else
            {
                glDrawArrays(GL_TRIANGLES, 0, m_nNumVertices);
                GetDevicePtr()->AddRenderTriangle(m_nNumVertices/3);
            }
            
			glCullFace(OS_L_FRONT);
		}
		if(IsSupportOES())
			(Device_OGL::s_fnBindVertex)(0);
		else{
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			for(int i=0;i<m_elementAttrs.size();i++)
			{
				OGLElement& el		=	m_elementAttrs[i];
				const char* szUsage	=	g_ogl->m_usageName[el.usage][el.usageIndex].t;
				prog->SetPointerEnable(el.var, false, szUsage);
			}
		}
		return true;
    }
	bool IsExtensionSupported(const char* szTargetExtension)
	{
		const unsigned char *pszExtensions = NULL;
		const unsigned char *pszStart;
		unsigned char *pszWhere, *pszTerminator;
		// Extension names should not have spaces
		pszWhere = (unsigned char *) strchr( szTargetExtension, ' ' );
		if( pszWhere || *szTargetExtension == '/0' )
			return false;
		// Get Extensions String
		pszExtensions = glGetString( GL_EXTENSIONS );
		// Search The Extensions String For An Exact Copy
		pszStart = pszExtensions;
		for(;;)
		{
			pszWhere = (unsigned char *) strstr( (const char *) pszStart, szTargetExtension );
			if( !pszWhere )
				break;
			pszTerminator = pszWhere + strlen( szTargetExtension );
			if( pszWhere == pszStart || *( pszWhere - 1 ) == ' ' )
				if( *pszTerminator == ' ' || *pszTerminator == '/0' )
					return true;
			pszStart = pszTerminator;
		}
		return false;
	}
	VOID		Device_OGL::AppCreate()
	{CPUTime(Device_OGL);
		//LogInfo("GL_EXTENSIONS:%s", glGetString( GL_EXTENSIONS ));
		//const char* vao = "GL_OES_vertex_array_object";
		//if(IsExtensionSupported ( vao ) != 0)
		//{
		//	LogInfo(" VAO supported!" );
		//}
		//else
		//{
		//	LogInfo(" VAO not supported" );
		//}
	}
	Device_OGL::Device_OGL()
	{
		g_ogl	=	this;
		memset(m_CullMode, 0, sizeof(m_CullMode));
		memset(m_BlendMode, 0, sizeof(m_BlendMode));
		memset(m_SamplerState, 0, sizeof(m_SamplerState));
		memset(m_DrawMode, 0, sizeof(m_DrawMode));
		memset(m_samplerFilter, 0, sizeof(m_samplerFilter));
		//
		m_DrawMode[DrawMode_Points]					=	GL_POINTS;
		m_DrawMode[DrawMode_Lines]					=	GL_LINES;
		m_DrawMode[DrawMode_LineStrip]				=	GL_LINE_STRIP;
		m_DrawMode[DrawMode_Triangles]				=	GL_TRIANGLES;
		m_DrawMode[DrawMode_TriangleStrip]			=	GL_TRIANGLE_STRIP;
		m_DrawMode[DrawMode_TriangleFan]			=	GL_TRIANGLE_FAN;
		m_CullMode[CullMode_Double]						=	CullMode_Double;
		m_CullMode[CullMode_CW]							=	OS_L_BACK;
		m_CullMode[CullMode_CCW]						=	OS_L_FRONT;
		m_BlendMode[Phantom::BlendMode_ZERO]			=	GL_ZERO;
		m_BlendMode[Phantom::BlendMode_ONE]				=	GL_ONE;
		m_BlendMode[Phantom::BlendMode_SRCCOLOR]		=	GL_SRC_COLOR;
		m_BlendMode[Phantom::BlendMode_INVSRCCOLOR]		=	GL_ONE_MINUS_SRC_COLOR;
		m_BlendMode[Phantom::BlendMode_SRCALPHA]		=	GL_SRC_ALPHA;
		m_BlendMode[Phantom::BlendMode_INVSRCALPHA]		=	GL_ONE_MINUS_SRC_ALPHA;
		m_BlendMode[Phantom::BlendMode_DESTALPHA]		=	GL_DST_ALPHA;
		m_BlendMode[Phantom::BlendMode_INVDESTALPHA]	=	GL_ONE_MINUS_DST_ALPHA;
		m_BlendMode[Phantom::BlendMode_DESTCOLOR]		=	GL_DST_COLOR;
		m_BlendMode[Phantom::BlendMode_INVDESTCOLOR]	=	GL_ONE_MINUS_DST_COLOR;
		m_BlendMode[Phantom::BlendMode_SRCALPHASAT]		=	GL_SRC_ALPHA_SATURATE;
		m_BlendMode[Phantom::BlendMode_BOTHSRCALPHA]	=	GL_ONE;
		m_BlendMode[Phantom::BlendMode_BOTHINVSRCALPHA]	=	GL_ONE;
		//
		SetUsageName(VertexElement::VE_USAGE_POSITION, "inPosition");
		SetUsageName(VertexElement::VE_USAGE_BLENDWEIGHT, "inBlendWeight");
		SetUsageName(VertexElement::VE_USAGE_BLENDINDICES, "inBlendIndices");
		SetUsageName(VertexElement::VE_USAGE_NORMAL, "inNormal");
		SetUsageName(VertexElement::VE_USAGE_PSIZE, "inPSize");
		SetUsageName(VertexElement::VE_USAGE_TEXCOORD, "inTexcoord");
		SetUsageName(VertexElement::VE_USAGE_TANGENT, "inTangent");
		SetUsageName(VertexElement::VE_USAGE_BINORMAL, "inBinormal");
		SetUsageName(VertexElement::VE_USAGE_TESSFACTOR, "inTessFactor");
		SetUsageName(VertexElement::VE_USAGE_POSITIONT, "inPositionT");
		SetUsageName(VertexElement::VE_USAGE_COLOR, "inColor");
		SetUsageName(VertexElement::VE_USAGE_FOG, "inFog");
		SetUsageName(VertexElement::VE_USAGE_DEPTH, "inDepth");
		SetUsageName(VertexElement::VE_USAGE_SAMPLE, "inSample");
	}
	//VOID					DynamicMesh::DrawOnly()	//渲染
	//{
	//	if(m_nNumVertices <= 0 || this->m_nFVF <= 0)
	//		return;
	//	BASE_CHECK_NR(glBindBuffer(GL_ARRAY_BUFFER, 0));
	//	BASE_CHECK_NR(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	//	//
	//	Program* p	=	g_manager.GetCurrProgram();
	//	if(!p)
	//		return;
	//	DeviceProgram_OGL* prog	=	static_cast<DeviceProgram_OGL*>(p);
	//	prog->SetPointer(prog->m_varPositions, this->m_posV.GetPtr() );
	//	prog->SetPointer(prog->m_varNormals, this->m_normalV.GetPtr() );
	//	prog->SetPointer(prog->m_varColors, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, m_diffuseV.GetPtr());
	//	prog->SetPointer(prog->m_varSpecular, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, m_specularV.GetPtr());
	//	prog->SetPointer(prog->m_varUVs, m_uvV.GetPtr());
	//	prog->SetPointer(prog->m_varUV2s, m_uv2V.GetPtr());
	//	prog->SetPointer(prog->m_varUV3s, m_uv3V.GetPtr());
	//	prog->SetPointer(prog->m_varUV4s, m_uv4V.GetPtr());
	//	prog->SetPointer(prog->m_varUV5s, m_uv5V.GetPtr());
	//	//
	//	int draw	=	g_ogl->m_DrawMode[m_drawMode];
	//	if(this->m_triangles.size() > 0)
	//	{
	//		if(m_attributes.size() == 0)
	//		{
	//			//glDrawElements(draw, m_triangles.size() * 3, GL_UNSIGNED_SHORT, m_triangles.GetPtr());
	//			GetDevicePtr()->AddRenderTriangle(m_triangles.size());
	//		}
	//		else
	//		{
	//			for(int i=0;i<m_attributes.size();i++)
	//			{
	//				MeshAttribute& m = m_attributes[i];
	//				g_manager.GetCurrProgram()->SetTexture(TextureIndex_0, m.texPtr);
	//				//glDrawElements(draw, m.count * 3, GL_UNSIGNED_SHORT, m_trianglesRenders.GetPtr() + m.beginIndex);
	//				GetDevicePtr()->AddRenderTriangle(m_triangles.size());
	//			}
	//		}
	//	}
	//	else
	//	{
	//		if(m_attributes.size() == 0)
	//		{
	//			//glDrawArrays(draw, 0, m_nNumVertices);
	//			GetDevicePtr()->AddRenderTriangle(m_nNumVertices/3);
	//		}
	//		else
	//		{
	//			for(int i=0;i<m_attributes.size();i++)
	//			{
	//				MeshAttribute& m = m_attributes[i];
	//				g_manager.GetCurrProgram()->SetTexture(TextureIndex_0, m.texPtr);
	//				//glDrawArrays(draw, m.beginIndex*3, m.count*3);
	//				GetDevicePtr()->AddRenderTriangle(m.count);
	//			}
	//		}

	//		
	//		GetDevicePtr()->AddRenderTriangle(m_nNumVertices / 3);
	//	}
	//	if(g_ogl->GetCullMode() == Phantom::CullMode_Double)
	//	{
	//		
	//		glCullFace(OS_L_BACK);
	//		if(this->m_triangles.size() > 0)
	//		{
	//			if(m_attributes.size() == 0)
	//			{
	//				//glDrawElements(draw, m_triangles.size() * 3, GL_UNSIGNED_SHORT, m_triangles.GetPtr());
	//				GetDevicePtr()->AddRenderTriangle(m_triangles.size());
	//			}
	//			else
	//			{
	//				for(int i=0;i<m_attributes.size();i++)
	//				{
	//					MeshAttribute& m = m_attributes[i];
	//					g_manager.GetCurrProgram()->SetTexture(TextureIndex_0, m.texPtr);
	//					//glDrawElements(draw, m.count * 3, GL_UNSIGNED_SHORT, m_trianglesRenders.GetPtr() + m.beginIndex);
	//					GetDevicePtr()->AddRenderTriangle(m_triangles.size());
	//				}
	//			}
	//		}
	//		else
	//		{
	//			//glDrawArrays(draw, 0, m_nNumVertices);
	//			GetDevicePtr()->AddRenderTriangle(m_nNumVertices / 3);
	//		}
	//		glCullFace(OS_L_FRONT);
	//	}
	//}
	Device_OGL::~Device_OGL()
	{
	}
	VOID	Device_OGL::SetUsageName(int usage, const char* szName)
	{CPUTime(Device_OGL);
		m_usageName[usage][0]	=	szName;
		for(int i=1;i<MAX_USAGE_INDEX;i++)
		{
			sprintf(m_usageName[usage][i].t, "%s%d", szName, i);
		}
	}
	VOID							Device_OGL::Clear(UINT flags)
	{CPUTime(Device_OGL);
		int c	=	0;
		if(flags & CLEAR_TARGET)
		{
			c	|=	GL_COLOR_BUFFER_BIT;
			BASE_CHECK_NR(glClearColor(this->m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a));
		}
		if(flags & CLEAR_ZBUFFER)
		{
			c	|=	GL_DEPTH_BUFFER_BIT;
			BASE_CHECK_NR(glClearDepthf(clamp01(m_fDepth)));
		}
		if(flags & CLEAR_STENCIL)
			c	|=	GL_STENCIL_BUFFER_BIT;
		BASE_CHECK_NR(glClear(c));
		memset(m_samplerFilter, 0, sizeof(m_samplerFilter));
	}
	VOID					Device_OGL::SetCullMode(CullMode mode)
	{CPUTime(Device_OGL);
		if(m_nCullMode == mode)
			return;
		m_nCullMode	=	mode;
		if(m_nCullMode == CullMode_Double)
		{
			BASE_CHECK_NR(glCullFace(OS_L_FRONT));
		}
		else
		{
			BASE_CHECK_NR(glCullFace(g_ogl->m_CullMode[m_nCullMode]));
		}
	}
	VOID					Device_OGL::SetBlendMode(BlendMode src, BlendMode dest)
	{CPUTime(Device_OGL);
		if(m_nSrcBlend == src && m_nDestBlend == dest)
			return;
		m_nSrcBlend		=	src;
		m_nDestBlend	=	dest;
		BASE_CHECK_NR(glBlendFunc(g_ogl->m_BlendMode[m_nSrcBlend], g_ogl->m_BlendMode[m_nDestBlend]));
	}
	VOID					Device_OGL::SetViewport(int x, int y, int w, int h)
	{CPUTime(Device_OGL);
		glViewport(x, y,  w, h);
	}
	VOID					Device_OGL::ResetAllDefault()
	{CPUTime(Device_OGL);
		Device::ResetAllDefault();
		//BASE_CHECK_NR(glEnable(GL_TEXTURE_2D));
		BASE_CHECK_NR(glEnable(GL_CULL_FACE));
		BASE_CHECK_NR(glCullFace(GL_FRONT));
		BASE_CHECK_NR(glEnable(GL_BLEND));
		BASE_CHECK_NR(glDepthFunc(GL_LEQUAL));
		BASE_CHECK_NR(glDepthMask( true ));
		SetEnableZ(m_bEnableZ);
	}
	VOID					Device_OGL::SetZWriteEnable(BOOL b)
	{CPUTime(Device_OGL);
		//SetEnableZ(b);
		if(m_bZWriteEnable == b)
			return;
		m_bZWriteEnable	=	b;
		if(m_bZWriteEnable)
			glDepthMask(GL_TRUE);
		else
			glDepthMask(GL_FALSE);
	}
	VOID					Device_OGL::SetEnableZ(BOOL b)
	{CPUTime(Device_OGL);
		if(m_bEnableZ == b)
			return;
		m_bEnableZ	=	b;
		if(m_bEnableZ)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);
	}
	VOID					Device_OGL::SetAlphaMode(AlphaMode mode)
	{CPUTime(Device_OGL);
		if(this->m_nAlphaMode == mode)
			return;
		m_nAlphaMode	=	mode;
		if(m_nAlphaMode > AlphaMode_None)
			glEnable(GL_BLEND);
		else
		{
			glDisable(GL_BLEND);
		}
		if(m_nAlphaMode == Phantom::AlphaMode_Blend)
			SetBlendMode(Phantom::BlendMode_SRCALPHA, Phantom::BlendMode_INVSRCALPHA);
	}
	VOID							Device_OGL::SetDrawInfo(const DrawVInfos& info, DeviceProgram_OGL* prog)
	{CPUTime(Device_OGL);
		BASE_CHECK_NR(glBindBuffer(GL_ARRAY_BUFFER, 0));
		BASE_CHECK_NR(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
		//
		prog->SetPointer(prog->m_varPositions, info.posList, "inPosition");
		prog->SetPointer(prog->m_varColors, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, info.colorList, "inColor");
		prog->SetPointer(prog->m_varUVs, info.uvList, "inTexcoord");
		prog->SetPointer(prog->m_varNormals, info.normalList, "inNormal");
	}
	VOID							Device_OGL::SetSampler(SamplerState sampler, TextureFilter filter)
	{CPUTime(Device_OGL);
		if(m_samplerFilter[sampler] == filter)
			return;
		m_samplerFilter[sampler]	=	filter;
		switch(sampler)
		{
		case SAMP_MAGFILTER:
			{
				int texF	=	GL_NEAREST;
				if(filter == Phantom::TextureFilter_Point)
					texF	=	GL_NEAREST;
				else
					texF	=	GL_LINEAR;
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
			}
			break;
		case SAMP_MINFILTER:
		case SAMP_MIPFILTER:
			{
				int texF	=	GL_NEAREST_MIPMAP_NEAREST;
				if(m_samplerFilter[SAMP_MINFILTER] == TextureFilter_Point)
				{
					if(m_samplerFilter[SAMP_MIPFILTER] == TextureFilter_Point)
						texF	=	GL_NEAREST_MIPMAP_NEAREST;
					else
						texF	=	GL_NEAREST_MIPMAP_LINEAR;
				}
				else
				{
					if(m_samplerFilter[SAMP_MIPFILTER] == TextureFilter_Point)
						texF	=	GL_LINEAR_MIPMAP_NEAREST;
					else
						texF	=	GL_LINEAR_MIPMAP_LINEAR;
				}
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texF);
			}
			break;
		}
	}
	VOID							Device_OGL::DrawIndexed(Program* prog, const DrawVInfos& info, const unsigned short* tList, int numIndices, DrawMode mode)
	{CPUTime(Device_OGL);
		DeviceProgram_OGL* p = static_cast<DeviceProgram_OGL*>(prog);
		SetDrawInfo(info, p);
		AddRenderTriangle(numIndices / 3);
		int draw	=	g_ogl->m_DrawMode[mode];
		glDrawElements(draw, numIndices, GL_UNSIGNED_SHORT, tList);
		if(g_ogl->GetCullMode() == Phantom::CullMode_Double)
		{
			GetDevicePtr()->AddRenderTriangle(numIndices / 3);
			glCullFace(OS_L_BACK);
			glDrawElements(draw, numIndices, GL_UNSIGNED_SHORT, tList);
			glCullFace(OS_L_FRONT);
		}
	}
	VOID							Device_OGL::DrawNoIndexed(Program* prog, const DrawVInfos& info, int numVertices, DrawMode mode)
	{CPUTime(Device_OGL);
		SetDrawInfo(info, static_cast<DeviceProgram_OGL*>(prog));
		AddRenderTriangle(numVertices / 3);
		//glDrawArrays(g_ogl->m_DrawMode[mode], 0, numVertices);
		if(g_ogl->GetCullMode() == Phantom::CullMode_Double)
		{
			GetDevicePtr()->AddRenderTriangle(numVertices / 3);
			glCullFace(OS_L_BACK);
			//glDrawArrays(g_ogl->m_DrawMode[mode], 0, numVertices);
			glCullFace(OS_L_FRONT);
		}
	}
    VOID                            Device_OGL::SetUIClipRect(const Rect* rect)
    {CPUTime(Device_OGL);
        if(rect)
        {
            glEnable(GL_SCISSOR_TEST);
            glScissor(rect->left, rect->top, rect->GetWidth(), rect->GetHeight());
        }
        else
        {
            glDisable(GL_SCISSOR_TEST);
        }
    }
	classT<Texture, classMgr<Texture> >*			Device_OGL::CreateTexture()
	{
		return new DeviceTexture_OGL();
	}
	Program*			Device_OGL::CreateProgram(int mode){CPUTime(Device_OGL);return new DeviceProgram_OGL(mode);}
	classT<DeviceMesh, classMgr<DeviceMesh> >*				Device_OGL::CreateMesh()
	{
		return new DeviceMesh_OGL();
	}
	//extern	Device*	CreateDeviceV1();
	Device*			CreateDevice()
	{
		//if(UseOpenGLV1())
		//	return CreateDeviceV1();
		return new Device_OGL();
	}
	RenderText*				Device_OGL::CreateRenderText()
	{CPUTime(Device_OGL);
		return new DeviceRenderText_OGL();
	}
	RenderTarget*			Device_OGL::CreateShadowMap()
	{CPUTime(Device_OGL);
		return new DeviceShadowMap_OGL();
	}
};

#endif
