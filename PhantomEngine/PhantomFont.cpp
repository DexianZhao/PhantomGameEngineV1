//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com
 
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PhantomFont.h"
#include "PhantomManager.h"

namespace Phantom{

	Font::Font()
	{
		m_fPixelSize	=	16.0f;
		m_fFontSize		=	16.0f;
		m_fFontItalic	=	0;
		m_bFullRGB		=	false;
		m_textColor		=	color4(1,1,1,1);
		m_fFontScale	=	1.0f;
		m_fontSpaceW	=	0;
	}
	Font::~Font()
	{
	}
	BOOL							Font::LoadFont(const char* szFigFile)
	{CPUTime(Font);
		CFileHelperR r;
		if(!r.open(szFigFile))
		{
			LogInfo("error:load font %s failure\n", szFigFile);
			return false;
		}
		char head[4];
		r.read(head, 4);
		if(stricmp(head, "fig") != 0)
		{
			LogInfo("error:load font %s failure, not fig file\n", szFigFile);
			return false;
		}
		int pixelSize = 12;
		int version = 1;
		r >> version;
		r >> pixelSize;
		r >> m_bFullRGB;
		m_fPixelSize	=	(float)pixelSize;
		int nCount = 0;
		r >> nCount;
		if(nCount > 65536)//不超过65536个
		{
			LogInfo("error:load font %s failure, data error\n", szFigFile);
			return false;
		}
		unsigned short maxcode = 0;
		r >> maxcode;
		unsigned short other = 0;
		r >> other;
		r.seek(other, StreamBase::Seek_Cur);
		//
		this->m_texts.SetArrayCount(nCount);
		r.read(m_texts.GetPtr(), sizeof(FontTextInfo) * nCount);
		//
		LFileName fig = szFigFile;
		fig.setIsFile();
		//
		LFileName dir, file;
		fig.splitpath(&dir, &file, 0);
		dir	+=	file;
		for(int i=0;i<MAX_FONT_IMAGES;i++)
		{
			LFileName path;
			path = dir;
			if(i == 0)
				path += ".png";
			else
			{
				char buf[128];
				sprintf(buf, "%d.png", i);
				path	+=	buf;
			}
			//如果找到了就载入
			if(!fexist(path))
				break;
			TexturePtr ret;
			if(g_manager.LoadTexture(ret, path, 0, Phantom::TextureFilter_Point|0x00030000))
			{
				m_fontImgs[i]	=	ret.GetPtr();
			}
		}
		m_charsetMap.SetArrayCount(maxcode + 1);
		for(int i=0;i<m_charsetMap.size();i++)
			m_charsetMap[i] = -1;
		for(int i=0;i<nCount;i++)
		{
			assert(m_texts[i].unicode <= maxcode);
			m_charsetMap[m_texts[i].unicode] = (unsigned short)i;
		}
		this->m_fontName	=	file;
		return true;
	}
	BOOL							Font::DrawTexts(const char* szUTF8, Rect& destRect, unsigned int len, unsigned dtFormat, const matrix4x4* world)
	{CPUTime(Font);
		if(len == 0)
			len = strlen(szUTF8);
		if(len <= 0)return false;
		m_tempBuffer.SetArrayCount(len * 2, false);
		if(g_manager.GetIsUtf8())
		{
			int index	=	0;
			for(int i=0;i<len;i++)
			{
				int n = 0;
				m_tempBuffer[index++] = utf8ToUnicode((const unsigned char*)szUTF8 + i, &n);
				i	+=	(n - 1);
			}
			m_tempBuffer.SetArrayCount(index);
		}
		else
		{
			int l = Utf8ToW(szUTF8, m_tempBuffer.GetPtr(), len * 2);
			m_tempBuffer.SetArrayCount(l);
		}
		DrawTexts(m_tempBuffer.GetPtr(), destRect, m_tempBuffer.size(), dtFormat, world);
		return true;
	}
	BOOL							Font::DrawTextClip(const short_t* szU, Rect& destRect, unsigned int len, unsigned dtFormat, const matrix4x4* world, const Rect* clip, BOOL bIsUI)
	{CPUTime(Font);
		if(!InitDrawText(destRect, &m_textColor, world, 0))
			return false;
		if(len == 0)
			len = u_strlen(szU);
	//
		rect_f destIO((float)destRect.left, (float)destRect.top, (float)destRect.right, (float)destRect.bottom);
		rect_f rc = destIO;
		float maxLineHeight	=	0;
		float2 topSpace(destIO.bottom, 0);
		{
			unsigned int nLength	=	len;
			const short_t* szUnicode	=	szU;
			float maxHeight	=	0;
			while(*szUnicode && nLength > 0)
			{
				nLength--;
				if(!DrawUnicode(*szUnicode, rc, maxHeight, destRect, false, maxLineHeight, &topSpace, bIsUI, clip))
					break;
				szUnicode++;
			}
			if(dtFormat & DrawTextAlign_RIGHT)
				destIO.left	=	destIO.right - (rc.right - destIO.left);
			else if(dtFormat & DrawTextAlign_CENTER)
				destIO.left	+=	(destIO.GetWidth() - (rc.right - destIO.left)) / 2.0f;
			if(dtFormat & DrawTextAlign_BOTTOM)
				destIO.top	=	destIO.bottom - maxLineHeight;//(rc.bottom - destIO.top);
			else if(dtFormat & DrawTextAlign_VCENTER)
				destIO.top	+=	(destIO.GetHeight() - maxLineHeight) / 2.0f;//(rc.bottom - destIO.top)) / 2.0f;
			if(dtFormat & DrawTextAlign_CALCRECT)
				return true;
		}
		{
			unsigned int nLength	=	len;
			const short_t* szUnicode	=	szU;
			float maxHeight	=	0;
			while(*szUnicode && nLength > 0)
			{
				nLength--;
				if(!DrawUnicode(*szUnicode, destIO, maxHeight, destRect, true, maxLineHeight, &topSpace, bIsUI, clip))
					break;
				szUnicode++;
			}
		}
		return true;
	}
	BOOL							Font::DrawTexts(const short_t* szU, Rect& destRect, unsigned int len, unsigned dtFormat, const matrix4x4* world, const rect_f* scale, BOOL bIsUI)
	{CPUTime(Font);
		if(!InitDrawText(destRect, &m_textColor, world, scale))
			return false;
		if(len == 0)
			len		=	-1;
		//
		rect_f destIO((float)destRect.left, (float)destRect.top, (float)destRect.right, (float)destRect.bottom);
		rect_f rc = destIO;
		float maxLineHeight	=	0;
		float2 topSpace(destIO.bottom, 0);
		//if(dtFormat > 0)
		{
			unsigned int nLength	=	len;
			const short_t* szUnicode	=	szU;
			float maxHeight	=	0;
			while(*szUnicode && nLength > 0)
			{
				nLength--;
				if(!DrawUnicode(*szUnicode, rc, maxHeight, destRect, false, maxLineHeight, &topSpace, bIsUI))
					break;
				szUnicode++;
			}
			//maxLineHeight	=	topSpace.y - topSpace.x;
			if(dtFormat & DrawTextAlign_RIGHT)
				destIO.left	=	destIO.right - (rc.right - destIO.left);
			else if(dtFormat & DrawTextAlign_CENTER)
				destIO.left	+=	(destIO.GetWidth() - (rc.right - destIO.left)) / 2.0f;
			if(dtFormat & DrawTextAlign_BOTTOM)
				destIO.top	=	destIO.bottom - maxLineHeight;//(rc.bottom - destIO.top);
			else if(dtFormat & DrawTextAlign_VCENTER)
				destIO.top	+=	(destIO.GetHeight() - maxLineHeight) / 2.0f;//(rc.bottom - destIO.top)) / 2.0f;
			if(dtFormat & DrawTextAlign_CALCRECT)
				return true;
		}
		{
			unsigned int nLength	=	len;
			const short_t* szUnicode	=	szU;
			float maxHeight	=	0;
			while(*szUnicode && nLength > 0)
			{
				nLength--;
				if(!DrawUnicode(*szUnicode, destIO, maxHeight, destRect, true, maxLineHeight, &topSpace, bIsUI))
					break;
				szUnicode++;
			}
		}
		return true;
	}
	Font::FontTextInfo*		Font::GetTextInfo(unsigned short u)
	{CPUTime(Font);
		if(u >= m_charsetMap.size() || m_charsetMap[u] == 65535)
			return 0;
		FontTextInfo* info	=	&m_texts[m_charsetMap[u]];
		if(info->unicode != u)
			return 0;
		return info;
	}
	int								Font::GetTextDrawCount(const short_t* szUnicode, int textCount, unsigned int nPixelWidth)
	{CPUTime(Font);
		Rect destRect(0, 0, g_manager.GetBufferWidth(), g_manager.GetBufferHeight());
		rect_f destIO(0, 0, g_manager.GetBufferWidth(), g_manager.GetBufferHeight());
		unsigned int nLength		=	textCount;
		float maxHeight	=	0;
		int nCount	=	0;
		float maxLineHeight = 0;
		while(*szUnicode && nLength > 0)
		{
			nLength--;
			if(!DrawUnicode(*szUnicode, destIO, maxHeight, destRect, false, maxLineHeight))
				break;
			if(destIO.right >= nPixelWidth)
				return nCount;
			nCount++;
			szUnicode++;
		}
		return nCount;
	}
	VOID							Font::SetFontSize(float f)
	{CPUTime(Font);
		m_fFontSize = f;
		if(m_fFontSize < 0)
			m_fFontSize	=	-m_fFontSize;
		m_fFontScale = m_fFontSize / m_fPixelSize;
		int nScale = (int)(m_fFontScale * 10);
		if((nScale%2) > 0)
			nScale = (nScale / 2 + 1) * 2;
		else
			nScale = (nScale / 2) * 2;
		m_fFontScale = (float)nScale / 10.0f;
		
	}
	Size							Font::GetTextDrawSize(const short_t* szUnicode, int textCount)
	{CPUTime(Font);
		Rect destRect(0, 0, g_manager.GetBufferWidth(), g_manager.GetBufferHeight());
		rect_f destIO(0, 0, m_fFontSize, m_fFontSize);
		unsigned int nLength		=	textCount;
		float maxHeight	=	0;
		float maxLineHeight = 0;
		float2 topSpace(1000, 0);
		while(*szUnicode && nLength > 0)
		{
			nLength--;
			if(!DrawUnicode(*szUnicode, destIO, maxHeight, destRect, false, maxLineHeight, &topSpace))
			{
				maxLineHeight	=	m_fFontSize;
				destIO.right	=	maxLineHeight;
				break;
			}
			szUnicode++;
		}
		return Size(destIO.right, maxLineHeight);
	}
	BOOL	ClipRect(Rect& dest, Rect& tex, const Rect& clip)
	{
		if(dest.bottom < clip.top || dest.top > clip.bottom || dest.left > clip.right || dest.right < clip.left)
			return false;
		int dh = dest.GetHeight();
		int dw = dest.GetWidth();
		int th = tex.GetHeight();
		int tw = tex.GetWidth();
		if(dest.top < clip.top)
		{
			int ch = clip.top - dest.top;
			dest.top = clip.top;
			tex.top += ch * th / dh;
		}
		if(dest.bottom > clip.bottom)
		{
			int ch = dest.bottom - clip.bottom;
			dest.bottom = clip.bottom;
			tex.bottom -= ch * th / dh;
		}
		if(dest.left < clip.left)
		{
			int cw = clip.left - dest.left;
			dest.left = clip.left;
			tex.left += cw * tw / dw;
		}
		if(dest.right > clip.right)
		{
			int cw = dest.right - clip.right;
			dest.right = clip.right;
			tex.right -= cw * tw / dw;
		}
		return true;
	}
	BOOL							Font::DrawUnicode(short_t u, rect_f& dio, float& maxHeight, const Rect& destRc, BOOL bDraw, float& maxLineHeight, float2* topSpace, BOOL bIsUI, const Rect* clip)
	{CPUTime(Font);
		if(u == ' ')
		{
			u = '_';
			bDraw	=	false;
		}
		Font* font	=	this;
		FontTextInfo* info	=	GetTextInfo(u);
		if(!info || u < 128)
		{
			font	=	g_manager.GetDefaultENFont();
			if(!font)
				return false;
			info	=	font->GetTextInfo(u);
		}
		if(!info)
		{
			for(int i=0;i<g_manager.m_fonts.size();i++)
			{
				if(g_manager.m_fonts[i] == font)
					continue;
				font	=	g_manager.m_fonts[i];
				info	=	g_manager.m_fonts[i]->GetTextInfo(u);
				if(info)
					break;
			}
		}
		float screenWidth	=	(float)g_manager.GetBufferWidth();
		float screenHeight	=	(float)g_manager.GetBufferHeight();
		float scale			=	1;//(float)m_fFontSize / (float)info->rh;//m_fFontScale;
		if(bDraw)
			scale *= ((float)maxLineHeight / (float)font->m_fPixelSize);
		float width,height;
		float offX			=	0;
		float offY			=	0;
		float realW			=	0;
		float realH			=	0;
		if(info)
		{
			//scale		*=	(float)m_fFontSize / (float)info->h;//
			width		=	(float)info->w * scale;
			height		=	(float)info->h * scale;
			realW		=	(float)info->rw * scale;
			realH		=	(float)info->rh * scale;
			offX		=	(float)info->offX * scale;
			offY		=	(float)info->offY * scale;
		}
		else
		{
			realW		=	m_fFontSize;
			realH		=	m_fFontSize;
			width		=	m_fFontSize;
			height		=	m_fFontSize;
		}
		dio.right			=	dio.left + width;
		if(dio.right > destRc.right)
		{
			dio.left		=	destRc.left;
			dio.right		=	dio.left + width;
			dio.top			+=	maxLineHeight;//maxHeight;
			maxHeight		=	0;
		}
		float top			=	dio.top + offY;// + realH;
		dio.bottom			=	top + height;//realH;
		if(!bDraw)
		{
			maxLineHeight	=	getmax(maxLineHeight, height);
		}
		else
		{
			float fSaveB	=	dio.bottom - dio.top;
		}
		rect_f rc(dio.left, top , dio.right, dio.bottom);
		//dio.bottom			+=	height * 0.5f;
		if(dio.bottom > maxHeight)
			maxHeight		=	dio.bottom;
		//
		if((int)dio.top > destRc.bottom)
			return false;
		if(info)
		{
			Texture* t	=	font->m_fontImgs[info->imageIndex].t;
			float fTexWidth		=	(float)t->GetWidth();
			float fTexHeight	=	(float)t->GetHeight();
			if(bDraw)
			{
				if(clip){
					Rect rcd(rc.left, rc.top, rc.right, rc.bottom);
					Rect rct(info->x, info->y, info->x + info->w, info->y + info->h);
					if(ClipRect(rcd, rct, *clip))
						DrawUnicode(font->m_fontImgs[info->imageIndex].t, info->channel, rect_f(rcd.left, rcd.top, rcd.right, rcd.bottom), rect_f((float)rct.left / fTexWidth, 
						(float)rct.top / fTexHeight, (float)rct.right / fTexWidth, (float)rct.bottom / fTexHeight), bIsUI);
				}
				else
				{
					rect_f texr((float)(info->x) / fTexWidth, (float)(info->y) / fTexHeight, (float)(info->x + info->w) / fTexWidth, (float)(info->y + info->h) / fTexHeight);
					rc.right = rc.left + (float)info->w;
					rc.bottom = rc.top + (float)info->h;
					DrawUnicode(font->m_fontImgs[info->imageIndex].t, info->channel, rc, texr, bIsUI);
				}
			}
		}
		if(info && u > 256)
		{
			if((info->w + 2) == info->rw)
			{
				width += width * 0.2f;
			}
		}
		dio.left			+=	(width-m_fontSpaceW);
		return true;
	}
	BOOL			Font::InitDrawText(const Rect& rc, const color4* c, const matrix4x4* world, const rect_f* scale)
	{CPUTime(Font);
		//if(rc.right <= rc.left || rc.bottom <= rc.top)
		//	return false;
		//if(m_bFullRGB)
		//{
		//	if(!g_manager.SetRenderMode(RenderMode_TextFull))
		//		return false;
		//}
		//else if(!g_manager.SetRenderMode(RenderMode_TextChannel))
		//	return false;
		//m_screenRect.set(rc.left, rc.top, rc.right, rc.bottom);
		//float fov		=	tanf(g_manager.Get2DProjectionFov() / 2.0f) * 2.0f;
		//float fovY		=	fov;
		//float fovX		=	fovY * (float)g_manager.GetBufferWidth() / (float)g_manager.GetBufferHeight();
		////
		//m_drawPos[0].setxyz(m_screenRect.left, m_screenRect.top, 0);
		//m_drawPos[1].setxyz(m_screenRect.right, m_screenRect.top, 0);
		//m_drawPos[2].setxyz(m_screenRect.right, m_screenRect.bottom, 0);
		//m_drawPos[3].setxyz(m_screenRect.left, m_screenRect.bottom, 0);
		//for(int i=0;i<4;i++)
		//{
		//	float3& p	=	m_drawPos[i];
		//	if(world)
		//		p.transform(p, *world);
		//	else if(scale)
		//	{
		//		p.x	=	p.x * scale->left + scale->right;
		//		p.y	=	p.y * scale->top + scale->bottom;
		//	}
		//}
		//m_drawPosBegin	=	m_drawPos[0];
		//m_drawPosDirL	=	m_drawPos[1] - m_drawPos[0];
		//m_drawPosDirT	=	m_drawPos[3] - m_drawPos[0];
		//m_drawPosDirL.normalize();
		//m_drawPosDirT.normalize();
		////
		//g_manager.SetWorldMatrix2D(0);
		//unsigned int	temp	=	0xffffffff;
		//if(c)
		//	temp	=	*c;
		//for(int i=0;i<4;i++)
		//	m_drawColor[i]	=	temp;
		//g_manager->SetCullVisible(CullMode_CW);
		//g_manager->SetAlphaMode(AlphaMode_Blend);
		return true;
	}
	VOID			Font::DrawUnicode(Texture* tex, int channel, const rect_f& screen, const rect_f& texRc, BOOL bIsUI)	//渲染2D图片
	{CPUTime(Font);
		assert(channel < 3);
		static	float3	texChannel[3] = {float3(1,0,0), float3(0,1,0), float3(0,0,1)};
		float fWidth	=	m_screenRect.GetWidth();
		float fHeight	=	m_screenRect.GetHeight();
		float left		=	screen.left - m_screenRect.left;
		float top		=	screen.top - m_screenRect.top;
		float right		=	screen.right - m_screenRect.left;
		float bottom	=	screen.bottom - m_screenRect.top;
		float l2		=	left + m_fFontItalic * this->m_fFontSize;
		float r2		=	right + m_fFontItalic * this->m_fFontSize;
		m_drawPos[0]	=	m_drawPosBegin + m_drawPosDirL * l2 + m_drawPosDirT * top;
		m_drawPos[1]	=	m_drawPosBegin + m_drawPosDirL * r2 + m_drawPosDirT * top;
		m_drawPos[2]	=	m_drawPosBegin + m_drawPosDirL * right + m_drawPosDirT * bottom;
		m_drawPos[3]	=	m_drawPosBegin + m_drawPosDirL * left + m_drawPosDirT * bottom;
		float fov		=	tanf(g_manager.Get2DProjectionFov() / 2.0f) * 2.0f;
		float fovY		=	fov;
		float fovX		=	fovY * (float)g_manager.GetBufferWidth() / (float)g_manager.GetBufferHeight();
		for(int i=0;i<4;i++)
		{
			if(bIsUI)
			{
				m_drawPos[i].x	*=	g_manager.GetUIScale().x;
				m_drawPos[i].y	*=	g_manager.GetUIScale().y;
				m_drawPos[i].x	+=	g_manager.GetUIOffset().x;
				m_drawPos[i].y	+=	g_manager.GetUIOffset().y;
			}
			m_drawPos[i].x	=	((m_drawPos[i].x / (float)g_manager.GetBufferWidth()) - 0.5f) * fovX;
			m_drawPos[i].y	=	(0.5f - (m_drawPos[i].y / (float)g_manager.GetBufferHeight())) * fovY;
			m_drawPos[i].z	=	0;
		}
		//
		m_drawUV[0]	=	float2(texRc.left, texRc.top);
		m_drawUV[1]	=	float2(texRc.right, texRc.top);
		m_drawUV[2]	=	float2(texRc.right, texRc.bottom);
		m_drawUV[3]	=	float2(texRc.left, texRc.bottom);
		g_manager.GetCurrProgram()->SetTexture(TextureIndex_0, tex);
		if(!m_bFullRGB)
		{
			if(this->m_varTextChannel.is_findFail())
				this->m_varTextChannel.reset_handle();
			g_manager.GetCurrProgram()->SetFloat3(this->m_varTextChannel, texChannel[channel], "textChannel");
		}
		static unsigned short indexBuffer[] ={3,1,0,3,2,1, 0,1,3,1,2,3};
		int numIndices	=	6;
		g_manager->DrawIndexed(g_manager.GetCurrProgram(), DrawVInfos(4, m_drawPos, m_drawColor, m_drawUV), indexBuffer, numIndices, DrawMode_Triangles);
	}
};
