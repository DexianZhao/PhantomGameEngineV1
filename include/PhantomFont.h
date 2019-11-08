//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com
 
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __PHANTOM_FONT_H__
#define	__PHANTOM_FONT_H__

#include "PhantomBase.h"

namespace Phantom{
	class	Texture;
	class	Font;
	//字体读取器
	class	Font: public Base
	{
	public:
		Font();
		~Font();
		ImplementComValue();
		BOOL							LoadFont(const char* szFigFile);	//在项目路径下面的/fonts/目录中存放比如assets/fonts/szFaceName.fig
		BOOL							DrawTexts(const char* szUTF8, Rect& destRect, unsigned int nLength = 0, unsigned dtFormat = DrawTextAlign_LEFT | DrawTextAlign_TOP, const matrix4x4* world = 0);	//
		BOOL							DrawTexts(const short_t* szUnicode, Rect& destRect, unsigned int nLength = 0, unsigned dtFormat = DrawTextAlign_LEFT | DrawTextAlign_TOP, const matrix4x4* world = 0, const rect_f* scale = 0, BOOL bIsUI = false);	//
		//
		BOOL							DrawTextClip(const short_t* szUnicode, Rect& destRect, unsigned int nLength = 0, unsigned dtFormat = DrawTextAlign_LEFT | DrawTextAlign_TOP, const matrix4x4* world = 0, const Rect* clip = 0, BOOL bIsUI = false);	//
		//
		BOOL							DrawUnicode(short_t u, rect_f& destRcInOut, float& maxHeight, const Rect& destRc, BOOL bDraw, float& maxLineHeight, float2* topSpace = 0, BOOL bIsUI = false, const Rect* clipcRect = 0);	//
		//返回在象素内可渲染文本数量
		int								GetTextDrawCount(const short_t* szUnicode, int textCount, unsigned int nPixelWidth);
		Size							GetTextDrawSize(const short_t* szUnicode, int textCount);
		//
		inline	FileName&				GetFileName(){return m_fontName;}
		//
		inline	VOID					SetFontScale(float f){m_fFontScale = f;}
		inline	float					GetFontScale(){return m_fFontScale;}
		inline	float					GetFontSize(){return m_fFontSize;}
		VOID							SetFontSize(float f);
		inline	float					GetFontItalic(){return m_fFontItalic;}
		inline	VOID					SetFontItalic(float f){m_fFontItalic = f;}
		inline	const color4&			GetTextColor(){return m_textColor;}
		inline	VOID					SetTextColor(const color4& c){m_textColor = c;}
		int								m_fontSpaceW;
		inline	float					GetPixelSize(){return m_fPixelSize;}
		struct	FontTextInfo
		{
			short			x,y;
			unsigned char	w,h;
			unsigned char	rw,rh;
			char	offX,offY;
			unsigned short	unicode;
			unsigned char	channel;
			unsigned char	imageIndex;
		};
		FontTextInfo*					GetTextInfo(unsigned short u);
		inline	BOOL					IsFullRGB(){return m_bFullRGB;}
	protected:
		VOID							CalcRect(const short_t* szUnicode, Rect& destRect, unsigned int nLength);
		BOOL							InitDrawText(const Rect& rc, const color4* c = 0, const matrix4x4* world = 0, const rect_f* scale = 0);
		VOID							DrawUnicode(Texture* t, int channel, const rect_f& screen, const rect_f& texRc, BOOL bIsUI);	//渲染2D图片
		//
		float3							m_drawPos[4];
		unsigned int					m_drawColor[4];
		float2							m_drawUV[4];
		float3							m_drawPosDirL;
		float3							m_drawPosDirT;
		float3							m_drawPosBegin;
		rect_f							m_screenRect;
		//
		CDynamicArray<short_t>			m_tempBuffer;
		float							m_fFontSize;
		float							m_fFontItalic;
		color4							m_textColor;
		float							m_fFontScale;
	protected:
		static const int				MAX_FONT_IMAGES		=	10;	//最多10个图片组合成一个font，尽量使用1张
		//
		AutoPtr<Texture>				m_fontImgs[MAX_FONT_IMAGES];
		CDynamicArray<FontTextInfo>		m_texts;
		BOOL							m_bFullRGB;
		CDynamicArray<unsigned short>	m_charsetMap;
		FileName						m_fontName;
		float							m_fPixelSize;
		_shader_var						m_varTextChannel;
		friend class					RenderTextFig;
	};
	extern	BOOL	ClipRect(Rect& dest, Rect& tex, const Rect& clip);
};

#endif
