//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com
 
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PhantomUITextFormat.h"
#include "PhantomUIManager.h"
#include "PhantomManager.h"

namespace Phantom{

	TextFormat::TextFormat(void)
	{
		m_lastMoveEventID		=	-1;
		m_bChanged				=	false;
		m_bRequestDraw			=	true;
		m_textAllocBuffer		=	1024;
		m_maxTextBuffer			=	(short_t*)malloc_s(m_textAllocBuffer * sizeof(short_t));
		m_formatBuffer			=	(unsigned short*)malloc_s(m_textAllocBuffer * sizeof(unsigned short));
		m_bUseTextOne			=	false;
		m_lineTextDraw			=	(LineTextDraw*)malloc_s(m_textAllocBuffer * sizeof(LineTextDraw));
		memset(m_formatBuffer, 0, m_textAllocBuffer * sizeof(unsigned short));
		memset(m_maxTextBuffer, 0, m_textAllocBuffer * sizeof(short_t));
		memset(m_lineTextDraw, 0, m_textAllocBuffer * sizeof(LineTextDraw));
		m_textLength			=	0;

		m_formatCount			=	1;
		m_formatAllocCount		=	4096;
		m_textFormats			=	(TextFormatData*)malloc_s(sizeof(TextFormatData) * m_formatAllocCount);
		memset(m_textFormats, 0, sizeof(TextFormatData) * m_formatAllocCount);
		m_textFormats[0].bUsed	=	true;
		m_pTextRender			=	0;
		m_lineDrawCount			=	0;
		m_bInvalidate			=	false;
		m_nShowBeginLine		=	0;
		m_maxLineCount			=	0;
		m_rectDraw.Set(0, 0, 0, 0);
		this->m_selBegin		=	0;
		this->m_selEnd			=	0;
		m_visibleBegin			=	0;
		m_visibleEnd			=	0;
		m_nShowLineCount		=	100;
		m_pTextFormatCB			=	0;
		m_eventID				=	1;
		m_nSelectColor			=	0xffffff00;
		m_nEventColor			=	0xff0000ff;
		m_pSelectEvent			=	0;
		m_lineSpacingHeight		=	0;
		m_nShadowWidth			=	0;
		m_nShadowColor			=	0xff000000;
		memset(&m_textDrawRegion, 0, sizeof(m_textDrawRegion));
		m_fTextAlpha			=	1.0f;
	}

	TextFormat::~TextFormat(void)
	{
		if(m_formatBuffer)
			free(m_formatBuffer);
		if(m_textFormats)
			free(m_textFormats);
		if(m_maxTextBuffer)
			free(m_maxTextBuffer);
		RemoveRedoBuffer();
		for(size_t i=0;i<this->m_undoBuffer.size();i++)
		{
			delete this->m_undoBuffer.at(i);
		}
		m_undoBuffer.clear();
	}

	VOID						TextFormat::EnableTextColor()
	{CPUTime(TextFormat);
		if(!m_bUseTextOne)
		{
			this->m_bUseTextOne	=	true;
			m_bRequestDraw		=	true;
		}
		if(m_maxTextOffset.size() < this->m_textLength)
		{
			int old		=	(int)m_maxTextOffset.size();
			m_maxTextOffset.resize(m_textLength);
			memset(&m_maxTextOffset.at(0), 0, sizeof(TextOne) * m_maxTextOffset.size());
			for(int i=old;i<m_textLength;i++)
				m_maxTextOffset.at(i).color	=	0xffffffff;
		}
	}
	VOID						TextFormat::SetTextColor(INT nIndex, const color4& c)
	{CPUTime(TextFormat);
		if(nIndex < 0 || nIndex >= m_textLength)
			return;
		EnableTextColor();
		m_maxTextOffset[nIndex].color	=	c;
		m_maxTextOffset[nIndex].bRc		=	false;
	}
	const color4*				TextFormat::GetTextColor(INT nIndex)
	{CPUTime(TextFormat);
		if(nIndex < 0 || nIndex >= m_maxTextOffset.size())
			return 0;
		static color4 c;
		c	=	m_maxTextOffset[nIndex].color;
		return &c;
	}
	VOID						TextFormat::SetTextOffset(INT nIndex, const Pixel& c)
	{CPUTime(TextFormat);
		if(nIndex < 0 || nIndex >= m_textLength)
			return;
		EnableTextColor();
		m_maxTextOffset[nIndex].offsetX	=	c.x;
		m_maxTextOffset[nIndex].offsetY	=	c.y;
	}
	const Pixel*				TextFormat::GetTextOffset(INT nIndex)
	{CPUTime(TextFormat);
		if(nIndex < 0 || nIndex >= m_maxTextOffset.size())
			return 0;
		static Pixel c;
		c.Set(m_maxTextOffset[nIndex].offsetX, m_maxTextOffset[nIndex].offsetY);
		m_maxTextOffset[nIndex].bRc		=	false;
		return &c;
	}

	VOID						TextFormat::RemoveRedoBuffer()
	{CPUTime(TextFormat);
		for(size_t i=0;i<this->m_redoBuffer.size();i++)
		{
			delete this->m_redoBuffer.at(i);
		}
		m_redoBuffer.clear();
	}

	INT							TextFormat::Insert(int nBegin, const short_t* str, int nCount, BOOL bAddToUndo, BOOL bAddToRedo)
	{CPUTime(TextFormat);
		m_bChanged	=	true;
		m_textLength		=	u_strlen(m_maxTextBuffer);
		m_bUseTextOne		=	false;
		if(nBegin < 0)nBegin = 0;
		if(nCount < 0)nCount = u_strlen(str);
		static short_t wbuf[1024];
		short_t* wbuffer	=	0;
		char	bcreat		=	false;
		if(nCount < 1024)
		{
			wbuffer			=	wbuf;
		}
		else
		{
			bcreat			=	true;
			wbuffer			=	new short_t[nCount + 1];
		}
		int nIndex	=	0;
		for(int i=0;i<nCount;i++)
		{
			if(str[i] == L'\r')
				continue;
			wbuffer[nIndex]	=	str[i];
			if(wbuffer[nIndex] == L'\t')
				wbuffer[nIndex]	=	L' ';
			nIndex++;
		}
		wbuffer[nIndex]	=	0;
		str				=	wbuffer;
		nCount			=	nIndex;
		if(nCount <= 0)
			return 0;
		//
		//short_t* wbufInsert	=	new char[nCount];
		//int nIndex	=	0;
		//for(int i=0;i<nCount;i++)
		//{
		//	if(str[i] == L'\r')
		//	if(wbufInsert
		//}
		//
		if(nBegin > m_textLength)
			nBegin	=	m_textLength;
		//
		if((nBegin + nCount) >= m_textAllocBuffer)
		{
			m_textAllocBuffer		+=	nCount + 4096;
			m_maxTextBuffer		=	(short_t*)realloc(m_maxTextBuffer, sizeof(short_t) * m_textAllocBuffer);
			memset(m_maxTextBuffer + m_textLength, 0, sizeof(short_t) * (m_textAllocBuffer - m_textLength - 1));
			m_formatBuffer		=	(unsigned short*)realloc(m_formatBuffer, sizeof(unsigned short) * m_textAllocBuffer);
			m_lineTextDraw		=	(LineTextDraw*)realloc(m_lineTextDraw, sizeof(LineTextDraw) * m_textAllocBuffer);
		}
		memmove(m_maxTextBuffer + nBegin + nCount, m_maxTextBuffer + nBegin, (m_textLength - nBegin + 1) * sizeof(short_t));
		memcpy(m_maxTextBuffer + nBegin, str, nCount * sizeof(short_t));
		//
		if(bAddToUndo || bAddToRedo)
		{
			UndoRedoData& d = *new UndoRedoData();
			d.begin		=	nBegin;
			d.count		=	nCount;
			d.type		=	d.TYPE_INSERT_TEXT;
			d.selBegin	=	m_selBegin;
			d.selEnd	=	m_selEnd;
			if(bAddToUndo)
			{
				this->m_undoBuffer.push_back( &d );
				if(!bAddToRedo)
					RemoveRedoBuffer();
			}
			else
				this->m_redoBuffer.push_back( &d );
		}
		//
		memmove(m_formatBuffer + nBegin + nCount, m_formatBuffer + nBegin, (m_textLength - nBegin + 1) * sizeof(short_t));
		//
		if(nBegin > 0)
		{
			m_formatBuffer[nBegin]	=	m_formatBuffer[nBegin - 1];
			if(this->m_textFormats[m_formatBuffer[nBegin]].nImageID > 0)
				m_formatBuffer[nBegin]	=	0;
		}
		else
			m_formatBuffer[nBegin]	=	0;
		for(int i=0;i<nCount;i++)
		{
			m_formatBuffer[i + nBegin] = m_formatBuffer[nBegin];
		}
		//
		m_textLength			+=	nCount;
		m_bUseTextOne			=	false;
		m_maxTextBuffer[m_textLength]	=	0;
		for(int i=0;i<m_textLength;i++)
			if(m_maxTextBuffer[i] == 0)
				m_maxTextBuffer[i] = L' ';
		//
		m_lineDrawCount	=	0;
		m_bInvalidate	=	true;
		m_textLength		=	u_strlen(m_maxTextBuffer);
		m_bUseTextOne		=	false;
		if(bcreat)
			delete[] wbuffer;
		//
		//delete[] wbufInsert;
		return nCount;
	}

	INT						TextFormat::Remove(int nBegin, int nCount, BOOL bAddToUndo, BOOL bAddToRedo)
	{CPUTime(TextFormat);
		m_bChanged	=	true;
		if(nBegin < 0 || nBegin > m_textLength)return nBegin;
		if(nCount < 0)nCount = m_textLength;
		if(nBegin + nCount > m_textLength)
			nCount	=	m_textLength - nBegin;
		if(nCount < 0)return nBegin;
		//
		if(bAddToUndo || bAddToRedo)
		{
			UndoRedoData& d = * new UndoRedoData();
			d.begin		=	nBegin;
			d.count		=	nCount;
			d.type		=	d.TYPE_DELETE_TEXT;
			d.selBegin	=	m_selBegin;
			d.selEnd	=	m_selEnd;
			if(nCount < RESERVED_BUFFER)
			{
				memcpy(d.d_buffer, m_maxTextBuffer + nBegin, sizeof(short_t) * nCount);
				memcpy(d.d_format, m_formatBuffer + nBegin, sizeof(unsigned short) * nCount);
				d.d_buffer[nCount]	=	0;
			}
			else
			{
				d.buffer	=	(short_t*)malloc_s((nCount + 1) * sizeof(short_t));
				memcpy(d.buffer, m_maxTextBuffer + nBegin, sizeof(short_t) * nCount);
				d.buffer[nCount]	=	0;

				d.format	=	(unsigned short*)malloc_s((nCount + 1) * sizeof(unsigned short));
				memcpy(d.format, m_formatBuffer + nBegin, sizeof(unsigned short) * nCount);
			}
			if(bAddToUndo)
			{
				this->m_undoBuffer.push_back( &d );
				if(!bAddToRedo)
					RemoveRedoBuffer();
			}
			else
				this->m_redoBuffer.push_back( &d );
		}
		//
		int length = (m_textLength - nBegin - nCount + 1);
		if(length > 0)
		{
			memmove(m_maxTextBuffer + nBegin, m_maxTextBuffer + nBegin + nCount, length * sizeof(short_t));
			memmove(m_formatBuffer + nBegin, m_formatBuffer + nBegin + nCount, length * sizeof(unsigned short));
		}
		//
		m_textLength	-=	nCount;
		m_bUseTextOne	=	false;
		if(m_textLength < 0)m_textLength = 0;
		m_maxTextBuffer[m_textLength]	=	0;
		//
		for(unsigned short i=1;i<m_formatCount;i++)
		{
			m_textFormats[i].bUsed	=	false;
		}
		for(int i=0;i<m_textLength;i++)
		{
			m_textFormats[m_formatBuffer[i]].bUsed	=	true;
		}
		for(unsigned short i=1;i<m_formatCount;i++)
		{
			if(!m_textFormats[i].bUsed)
			{
				//if(m_pTextRender)
				//	m_pTextRender->DeleteFont( m_textFormats[i].nFont );
			}
		}
		m_lineDrawCount	=	0;
		m_bInvalidate	=	true;
		return nBegin;
	}

	VOID						TextFormat::SetFormat(int nBegin, int nCount, DWORD dwColor, INT nFontIndex, INT nImageID, INT nShadowWidth, DWORD shadowColor, BOOL bAddToUndo, BOOL bAddToRedo, INT nEvent)
	{CPUTime(TextFormat);
		if(nBegin < 0)return;
		if(nBegin > m_textLength)return;
		if(nCount < 0)
			nCount		=	m_textLength;
		unsigned short	iFormat	=	AddFormat(dwColor, nFontIndex, nImageID, nShadowWidth, shadowColor, nEvent, FORMAT_TYPE_ALL);
		if(nCount > (m_textLength - nBegin))
			nCount	=	m_textLength - nBegin;
		//
		if(bAddToUndo || bAddToRedo)
		{
			UndoRedoData& d = * new UndoRedoData();
			d.begin		=	nBegin;
			d.count		=	nCount;
			d.type		=	d.TYPE_SET_FORMAT;
			d.selBegin	=	m_selBegin;
			d.selEnd	=	m_selEnd;
			if(nCount < RESERVED_BUFFER)
			{
				memcpy(d.d_format, m_formatBuffer + nBegin, sizeof(unsigned short) * nCount);
				d.d_buffer[nCount]	=	0;
			}
			else
			{
				d.format	=	(unsigned short*)malloc_s((nCount + 1) * sizeof(unsigned short));
				memcpy(d.format, m_formatBuffer + nBegin, sizeof(unsigned short) * nCount);
			}
			if(bAddToUndo)
			{
				this->m_undoBuffer.push_back( &d );
				if(!bAddToRedo)
					RemoveRedoBuffer();
			}
			else
				this->m_redoBuffer.push_back( &d );
		}
		//
		for(int i=0;i<nCount;i++)
		{
			m_formatBuffer[i + nBegin]	=	iFormat;
		}
		//
		m_lineDrawCount	=	0;
		this->m_bInvalidate	=	true;
	}

	VOID						TextFormat::SetFormatEvent(INT nBegin, INT nCount, const short_t* szEventText, int parameter)
	{CPUTime(TextFormat);
		m_eventID++;
		int nEventID		=	SetEventText( m_eventID, szEventText, parameter);
		if(nCount < 0)nCount = this->m_textLength;
		for(int i=0;i<nCount;i++)
		{
			int ib	=	nBegin + i;
			if(ib >= m_textLength)
				return;
			int iformat = m_formatBuffer[ib];
			if(iformat >= m_formatCount || iformat < 0)
				continue;
			TextFormatData& d		=	m_textFormats[iformat];
			m_formatBuffer[ib]		=	AddFormat(d.dwColor, d.nFont, d.nImageID, d.nShadowWidth, d.dwShadowColor, nEventID, FORMAT_TYPE_EVENT);
		}
		//
		m_lineDrawCount	=	0;
		this->m_bInvalidate	=	true;
	}

	VOID						TextFormat::SetFormatColor(INT nBegin, INT nCount, DWORD dwColor)
	{CPUTime(TextFormat);
		if(nCount < 0)nCount = this->m_textLength;
		for(int i=0;i<nCount;i++)
		{
			int ib	=	nBegin + i;
			if(ib >= m_textLength)
				return;
			int iformat = m_formatBuffer[ib];
			if(iformat >= m_formatCount || iformat < 0)
				continue;
			TextFormatData& d		=	m_textFormats[iformat];
			m_formatBuffer[ib]	=	AddFormat(dwColor, d.nFont, d.nImageID, d.nShadowWidth, d.dwShadowColor, d.nEvent, FORMAT_TYPE_COLOR);
		}
		//
		m_lineDrawCount	=	0;
		this->m_bInvalidate	=	true;

	}

	VOID						TextFormat::SetFormatFont(INT nBegin, INT nCount, INT nFontID)
	{CPUTime(TextFormat);
		if(nCount < 0)nCount = this->m_textLength;
		for(int i=0;i<nCount;i++)
		{
			int ib	=	nBegin + i;
			if(ib >= m_textLength)
				return;
			int iformat = m_formatBuffer[ib];
			if(iformat >= m_formatCount || iformat < 0)
				continue;
			TextFormatData& d		=	m_textFormats[iformat];
			m_formatBuffer[ib]		=	AddFormat(d.dwColor, nFontID, d.nImageID, d.nShadowWidth, d.dwShadowColor, d.nEvent, FORMAT_TYPE_FONT);
		}
		//
		m_lineDrawCount	=	0;
		this->m_bInvalidate	=	true;
	}

	unsigned short				TextFormat::AddFormat(DWORD dwColor, INT nFontIndex, INT nImageID, INT nShadowWidth, DWORD shadowColor, INT nEvent, INT nType)
	{CPUTime(TextFormat);
		int nUse	=	m_formatCount;
		for(unsigned short i=0;i<m_formatCount;i++)
		{
			TextFormatData& d	=	this->m_textFormats[i];
			if(!d.bUsed)
			{
				nUse	=	i;
				continue;
			}
			//if(nType == 0)
			//	return i;
			////
			//if(nType & FORMAT_TYPE_COLOR)
			//{
			//	if(d.dwColor != dwColor)
			//		continue;
			//}
			//if(nType & FORMAT_TYPE_IMAGE)
			//{
			//	if(d.nImageID != nImageID)
			//		continue;
			//}
			//if(nType & FORMAT_TYPE_EVENT)
			//{
			//	if(d.nEvent != nEvent)
			//		continue;
			//}
			//if(nType & FORMAT_TYPE_FONT)
			//{
			//	if(d.nFont != nFontIndex)
			//		continue;
			//}
			if(d.dwColor == dwColor && d.dwShadowColor == shadowColor && d.nShadowWidth == nShadowWidth && d.nImageID == nImageID && d.nFont == nFontIndex && d.nEvent == nEvent)
			{
				return i;
			}
		}
		if(m_formatCount > 60000)
			return 0;
		memset(&m_textFormats[nUse], 0,sizeof(TextFormatData));
		m_textFormats[nUse].dwColor			=	dwColor;
		m_textFormats[nUse].dwShadowColor	=	shadowColor;
		m_textFormats[nUse].bUsed			=	true;
		m_textFormats[nUse].nShadowWidth	=	nShadowWidth;
		m_textFormats[nUse].nImageID		=	nImageID;
		m_textFormats[nUse].nFont			=	nFontIndex;
		m_textFormats[nUse].nEvent			=	nEvent;
		if(nUse == m_formatCount)
			m_formatCount++;
		m_lineDrawCount	=	0;
		return nUse;
	}

	VOID						TextFormat::RemoveSelect()
	{CPUTime(TextFormat);
		m_bChanged	=	true;
		if(abs(m_selEnd - m_selBegin) > 0)
		{
			m_selBegin	=	m_selEnd	=	Remove(getmin(m_selBegin, m_selEnd), abs(m_selEnd - m_selBegin));
		}
		ResetSelect();
	}

	VOID						TextFormat::InsertImage(INT nIndex, INT nImageID)
	{CPUTime(TextFormat);
		m_bChanged	=	true;
		RemoveSelect();
		if(nIndex >= 0 && m_textLength > nIndex)
		{
			if(m_maxTextBuffer[nIndex] != L' ')
			{
				short_t buf[20];
				buf[0] = ' ';
				buf[1] = 0;
				this->Insert(nIndex, buf, 1);
			}
			SetFormat(nIndex, 1, 0, 0, nImageID, 0, 0);
		}
	}

	VOID						TextFormat::Render(VOID* lpParam)
	{CPUTime(TextFormat);
		if(this->m_bInvalidate)
		{
			this->Invalidate();
			m_bInvalidate	=	FALSE;
		}
		//
		char bRequestBuild	=	false;
		int nLineHeight		=	0;
		int nMaxTop			=	0;
		m_visibleLineCount	=	0;
		m_visibleBegin		=	-1;
		int	nLineIndex		=	m_nShowBeginLine;
		int alpha = (float)(((int)(m_fTextAlpha * 255.0f) & 0x000000FF) << 24);
		for(int i=0;i<m_lineDrawCount;i++)
		{
			LineTextDraw& ld	=	this->m_lineTextDraw[i];
			if(ld.lineIndex < m_nShowBeginLine)
			{
				nLineHeight		=	ld.rc.bottom;
				//
				continue;
			}
			else if(m_visibleBegin < 0)
				m_visibleBegin	=	ld.nBegin;
			char bSel		=	false;
			int selIndex	=	getmin(m_selBegin, m_selEnd);
			int selEnd		=	getmax(m_selBegin, m_selEnd);
			if(GetSelRange() > 0)
			{
				if(selIndex >= ld.nBegin && selIndex < (ld.nBegin + ld.nCount))
					bSel		=	true;
				else if(selIndex < ld.nBegin && selEnd >= ld.nBegin)
					bSel		=	true;
			}
			TextFormatData& d	=	m_textFormats[ld.formatIndex];
			if(d.nImageID > 0)
			{
				Rect rc			=	ld.rc;
				Size sz			=	m_pTextRender->GetImageSize(d.nImageID);
				rc.right		=	rc.left + sz.cx;
				rc.top			=	rc.bottom - sz.cy;
				if(bSel)
				{
					//m_pTextRender->DrawSelectRect( rc, m_nSelectColor, lpParam );
					m_pTextRender->DrawImage( d.nImageID, rc, true, lpParam, 0x00FFFFFF | alpha  );
				}
				else
					m_pTextRender->DrawImage( d.nImageID, rc, false, lpParam, 0x00FFFFFF | alpha  );
				continue;
			}
			if(bSel)
			{
				int begin	=	0;
				int end		=	0;
				if(selIndex > ld.nBegin)
					begin	=	selIndex - ld.nBegin;
				if((ld.nBegin + ld.nCount) < selEnd)
					end		=	ld.nCount;
				else
					end		=	(selEnd - ld.nBegin);
				//
				Rect rc		=	ld.rc;
				//
				Size sz;
				this->GetDrawSize(m_maxTextBuffer + ld.nBegin, begin, d.nFont, sz);
				rc.left		=	rc.left + sz.cx;
				this->GetDrawSize(m_maxTextBuffer + ld.nBegin + begin, (end - begin), d.nFont, sz);
				rc.right	=	rc.left + sz.cx;
				//
				//m_pTextRender->DrawSelectRect( rc, m_nSelectColor, lpParam );
			}
			if(ld.rc.bottom > this->m_rectDraw.bottom)
				break;
			m_visibleLineCount	=	ld.lineIndex - m_nShowBeginLine;
			if(m_visibleLineCount >= m_nShowLineCount)
				break;
			m_visibleEnd		=	ld.nBegin + ld.nCount;
			DWORD dwColor		=	d.dwColor;
			if(d.nEvent > 0)
			{
				if(m_pSelectEvent == &d)
				{
					dwColor		=	m_nSelectColor;
					//m_pTextRender->DrawSelectRect( ld.rc, m_nSelectColor );
				}
				else if(m_nEventColor != 0)
				{
					//m_pTextRender->DrawSelectRect( ld.rc, m_nEventColor );
				}
			}
			if(m_bUseTextOne && this->m_maxTextOffset.size() >= this->m_textLength)
			{
				Rect	lastRect;
				lastRect	=	ld.rc;
				color4 src(dwColor);
				src.a	=	m_fTextAlpha;
				for(int i=0;i<ld.nCount;i++)
				{
					int index	=	ld.nBegin + i;
					if(index >= 0 && index < m_textLength)
					{
						TextOne& one	=	m_maxTextOffset[index];
						if(!one.bRc)//m_bRequestDraw || (one.rc.left == one.rc.right || one.rc.bottom == one.rc.top))
						{
							Size sz;
							if(!GetDrawSize(m_maxTextBuffer + index, 1, d.nFont, sz))
								continue;
							lastRect.right	=	lastRect.left + sz.cx;
							one.rc			=	lastRect;
							one.bRc			=	true;
						}
						lastRect.left	=	one.rc.right;
						Rect rc	=	one.rc;
						rc.Offset(one.offsetX, one.offsetY);
						short_t textBuf[5];
						textBuf[0]	=	m_maxTextBuffer[index];
						textBuf[1]	=	0;
						color4 color(one.color);
						color	*=	src;
						if(color.a <= 0.001f)
							continue;
						//m_pTextRender->DrawFormatText(d.nFont, textBuf, 1, rc, color, DrawTextAlign_LEFT | DrawTextAlign_BOTTOM | DrawTextAlign_SINGLELINE, lpParam, m_nShadowWidth, m_nShadowColor);
					}
				}
			}
			//else
				//m_pTextRender->DrawFormatText(d.nFont, m_maxTextBuffer + ld.nBegin, ld.nCount, ld.rc, (dwColor & 0x00FFFFFF) | alpha, DrawTextAlign_LEFT | DrawTextAlign_BOTTOM | DrawTextAlign_SINGLELINE, lpParam, m_nShadowWidth, m_nShadowColor);
		}
		m_bRequestDraw	=	false;
	}

	INT							TextFormat::GetFormatData(FormatData* buffer, INT nDataRange)		//取得格式属性
	{CPUTime(TextFormat);
		//static unsigned short szMaxFormats[10000];
		{
			int formatCount	=	0;
			unsigned short formatBegin	=	this->m_formatBuffer[0];
			for(int i=1;i<this->m_textLength;i++)
			{
				if(formatBegin != this->m_formatBuffer[i])
				{
					if(formatBegin > 0)
						formatCount++;
					formatBegin	=	this->m_formatBuffer[i];
				}
			}
			if(nDataRange < formatCount)
				return formatCount;
		}
		//
		int formatCount	=	0;
		int formatIndex	=	0;
		unsigned short formatBegin	=	this->m_formatBuffer[0];
		memset(buffer, 0, sizeof(FormatData) * nDataRange);
		for(int i=1;i<=m_textLength;i++)
		{
			if(formatBegin != this->m_formatBuffer[i] || i == m_textLength)
			{
				if(formatBegin > 0)
				{
					FormatData& d	=	buffer[formatCount];
					TextFormatData& td	=	this->m_textFormats[formatBegin];
					INT nHeight, nBold, nUnderline;
					m_pTextRender->GetFontData(td.nFont, nHeight, nBold, nUnderline);
					formatCount++;
					//
					d.begin			=	formatIndex;
					d.end			=	i-1;
					d.color			=	td.dwColor;
					if(td.nFont > 0)
					{
						d.fontHeight	=	nHeight;
						d.fontState		=	(nBold ? d.fontType_Bold : 0);
					}
					if(td.nImageID > 0)
					{
						int imageType=0;
						d.imageID		=	td.nImageID;
						d.imageType		=	imageType;
					}
					d.shadowWidth		=	td.nShadowWidth;
					//
				}
				formatIndex	=	i;
				formatBegin	=	this->m_formatBuffer[i];
			}
		}
		return formatCount;
	}

	VOID						TextFormat::SetFormatData(const FormatData* buffer, INT nBufferRange)	//设置格式属性
	{CPUTime(TextFormat);
		memset(m_formatBuffer, 0, sizeof(unsigned short) * this->m_textLength);
		for(int i=0;i<nBufferRange;i++)
		{
			const FormatData& d	=	buffer[i];
			//
			INT nFont		=	0;
			INT nImage		=	0;
			if(d.fontHeight > 0)
				nFont		=	GetDlgManager()->AddFont(0, d.fontHeight, d.fontState & d.fontType_Bold, d.fontState & d.fontType_Underline);
			if(d.imageID > 0)
				nImage		=	d.imageID;
			for(int is=d.begin;is<=d.end;is++)
			{
				if(is > this->m_textLength)
					break;
				m_formatBuffer[is]	=	AddFormat(d.color, nFont, nImage, d.shadowWidth, 0, 0, FORMAT_TYPE_ALL);
			}
		}
	}

	VOID						TextFormat::Invalidate()
	{CPUTime(TextFormat);
		if(!this->m_pTextRender)return;
		int begin	=	0;
		int count	=	0;
		int format	=	m_formatBuffer[0];
		Rect rcRender	=	m_rectDraw;
		Rect rcLine		=	m_rectDraw;
		m_lineDrawCount	=	0;
		m_maxLineCount	=	0;
		for(int i=0;i<=m_textLength;i++)
		{
			if(m_formatBuffer[i] != format || i >= m_textLength || m_maxTextBuffer[i] == L'\n')
			{
				//if(m_maxTextBuffer[i] == L'\r')
				//m_maxTextBuffer[i] = L' ';//MessageBox(0, 0, 0, 0);
				count	=	i - begin;
				//if(count == 0)
				//{
				//	if(m_maxTextBuffer[i] == L'\n')
				//	{
				//		this->m_lineTextDraw[m_lineDrawCount].formatIndex	=	0;
				//		this->m_lineTextDraw[m_lineDrawCount].nBegin		=	begin;
				//		this->m_lineTextDraw[m_lineDrawCount].nCount		=	1;
				//		this->m_lineTextDraw[m_lineDrawCount].rc			=	rcRender;
				//		this->m_lineTextDraw[m_lineDrawCount].rc.bottom		=	this->m_lineTextDraw[m_lineDrawCount].rc.top + 12;
				//		this->m_lineTextDraw[m_lineDrawCount].lineIndex		=	m_maxLineCount;
				//		m_lineDrawCount++;
				//	}
				//	begin++;
				//	m_maxLineCount++;
				//	rcRender.top	+=	12;
				//	rcLine			=	rcRender;
				//	continue;
				//}
				if(m_maxTextBuffer[i] == L'\n')
				{
					//count--;
				}
				//
				char bNewLine	=	false;
				if(m_maxTextBuffer[i] == L'\n')
				{
					//m_maxTextBuffer[i]	=	L' ';
					bNewLine	=	true;
				}
				int nHeight	=	DrawFormatText(begin, count, rcRender, rcLine, format, m_maxLineCount, bNewLine);
				if(nHeight > 0)
				{
					rcRender.top	+=	nHeight;
					rcLine.top		=	rcRender.top;
				}
				if(rcRender.top > 5000)
					break;
				//
				format	=	m_formatBuffer[i];
				begin	=	i;
			}
			if(m_maxTextBuffer[i] == L'\n')
			{
				begin	++;
			}
		}
		m_maxLineCount++;
		int nLineHeight		=	0;
		int lineIndex		=	-1;
		int lastIndex		=	0;
		//
		static int	lineHeight[20000];
		memset(lineHeight, 0, sizeof(int) * (m_maxLineCount + 10));
		for(int i=0;i<m_lineDrawCount;i++)
		{
			LineTextDraw& ld			=	m_lineTextDraw[i];
			lineHeight[ld.lineIndex]	=	getmax(lineHeight[ld.lineIndex], ld.rc.bottom - ld.rc.top);
		}
		nLineHeight				=	0;
		int nBeginLineHeight	=	0;
		for(int i=0;i<=m_maxLineCount;i++)
		{
			int top				=	nLineHeight;
			nLineHeight			+=	lineHeight[i];
			if(m_nShowBeginLine > i)
				nBeginLineHeight	=	nLineHeight;
			lineHeight[i]		=	top + m_rectDraw.top;
		}
		m_textDrawRegion.Set(1000, 1000, -1000, -1000);
		for(int i=0;i<m_lineDrawCount;i++)
		{
			LineTextDraw& ld	=	this->m_lineTextDraw[i];
			ld.rc.top			=	lineHeight[ld.lineIndex] - nBeginLineHeight;
			ld.rc.bottom		=	lineHeight[ld.lineIndex + 1] - nBeginLineHeight;//ld.rc.top + lineHeight[i];
			//
			m_textDrawRegion.left	=	getmin(m_textDrawRegion.left, ld.rc.left);
			m_textDrawRegion.top	=	getmin(m_textDrawRegion.top, ld.rc.top);
			m_textDrawRegion.right	=	getmax(m_textDrawRegion.right, ld.rc.right);
			m_textDrawRegion.bottom	=	getmax(m_textDrawRegion.bottom, ld.rc.bottom);
		}
		if(m_lineDrawCount > 0)
		{
			this->m_lineTextDraw[m_lineDrawCount - 1].rc.right = rcRender.right;
		}
		//nLineHeight		=	0;
		//for(int i=0;i<m_lineDrawCount;i++)
		//{
		//	LineTextDraw& ld	=	this->m_lineTextDraw[i];
		//	if(ld.lineIndex < m_nShowBeginLine)
		//	{
		//		nLineHeight		=	max(ld.rc.bottom, nLineHeight);
		//		continue;
		//	}
		//	break;
		//}
		//for(int i=0;i<m_lineDrawCount;i++)
		//{
		//	LineTextDraw& ld	=	this->m_lineTextDraw[i];
		//	ld.rc.top			-=	nLineHeight;
		//	ld.rc.bottom		-=	nLineHeight;
		//}
	}

	VOID						TextFormat::BeginShowLine(INT nIndex)
	{CPUTime(TextFormat);
		m_nShowBeginLine	=	nIndex;
		this->Invalidate();
	}

	INT							TextFormat::DrawFormatText(INT nBegin, INT nCount, const Rect& rcRender, Rect& rcLine, unsigned int formatIndex, int& lineIndex, char bNewLine)
	{CPUTime(TextFormat);
		TextFormatData& d	=	m_textFormats[formatIndex];
		INT nDraw			=	0;
		INT nHeight			=	0;
		INT nEndHeight		=	0;
		LineTextDraw* pNewLine	=	0;
		while(nDraw < nCount)
		{
			if(rcLine.top > 5000)
				break;
			const short_t* szDrawBegin	=	m_maxTextBuffer + nBegin + nDraw;
			int		nDrawCount			=	nCount - nDraw;
			int		nNowDraw			=	GetDrawTextLength(szDrawBegin, nDrawCount, d.nFont, rcLine.right - rcLine.left);
			//
			//Rect drawRC					=	rcLine;
			//m_pTextRender->CalcText(d.nFont, szDrawBegin, max(nNowDraw, 1), drawRC, DrawTextAlign_LEFT | DrawTextAlign_TOP | DrawTextAlign_SINGLELINE);
			Size sz;
			if(d.nImageID == 0)
				GetDrawSize(szDrawBegin, getmax(nNowDraw, 1), d.nFont, sz);
			else
			{
				sz			=	m_pTextRender->GetImageSize(d.nImageID);
				nNowDraw	=	1;
			}
			//
			if(nNowDraw == 0 || ((d.nImageID != 0) && (rcLine.left + sz.cx) > rcLine.right))
			{
				rcLine.left				=	rcRender.left;
				rcLine.top				+=	sz.cy;
				nHeight					+=	sz.cy;
				lineIndex				++;
				nHeight					+=	m_lineSpacingHeight;
				if(pNewLine)
				{
					pNewLine->rc.right	=	rcRender.right;// + 500;
				}
				continue;
			}
			//
			Rect drawRC					=	rcLine;
			drawRC.right				=	drawRC.left + sz.cx;
			drawRC.bottom				=	drawRC.top + sz.cy + m_lineSpacingHeight;
			this->m_lineTextDraw[m_lineDrawCount].formatIndex	=	formatIndex;
			this->m_lineTextDraw[m_lineDrawCount].nBegin		=	nBegin + nDraw;
			this->m_lineTextDraw[m_lineDrawCount].nCount		=	nNowDraw;
			this->m_lineTextDraw[m_lineDrawCount].rc			=	drawRC;
			this->m_lineTextDraw[m_lineDrawCount].lineIndex		=	lineIndex;
			pNewLine					=	&m_lineTextDraw[m_lineDrawCount];
			//
			nDraw			+=	nNowDraw;
			rcLine.left		=	drawRC.right;
			if(bNewLine)
			{
				nEndHeight		=	(drawRC.bottom - drawRC.top);
			}
			m_lineDrawCount++;
		}
		if(nCount == 0)
		{
			const short_t* szDrawBegin	=	m_maxTextBuffer + nBegin;
			Rect drawRC					=	rcLine;
			Size sz;
			GetDrawSize(szDrawBegin, 1, d.nFont, sz);
			drawRC.right				=	drawRC.left + 0;//sz.cx;
			drawRC.bottom				=	drawRC.top + sz.cy + m_lineSpacingHeight;
			this->m_lineTextDraw[m_lineDrawCount].formatIndex	=	formatIndex;
			this->m_lineTextDraw[m_lineDrawCount].nBegin		=	nBegin;
			this->m_lineTextDraw[m_lineDrawCount].nCount		=	nCount;
			this->m_lineTextDraw[m_lineDrawCount].rc			=	drawRC;
			this->m_lineTextDraw[m_lineDrawCount].lineIndex		=	lineIndex;
			pNewLine		=	&m_lineTextDraw[m_lineDrawCount];
			rcLine.left		=	drawRC.right;
			if(bNewLine)
			{
				nEndHeight	=	(drawRC.bottom - drawRC.top);
			}
			m_lineDrawCount++;
		}
		if(bNewLine)
		{
			rcLine.left				=	rcRender.left;
			lineIndex++;
			if(pNewLine)
				pNewLine->rc.right	=	rcRender.right;// + 500;
		}
		return (nHeight + nEndHeight);
	}

	INT							TextFormat::GetDrawTextLength(const short_t* szText, int numText, int iFont, int nPixelWidth)
	{CPUTime(TextFormat);
		FontNode2D* fontNode	=	GetDlgManager()->GetFontNode(iFont);
		if(fontNode && fontNode->fontPtr.t)
		{
			fontNode->fontPtr->SetFontSize(fontNode->nFontHeight);
			return fontNode->fontPtr.t->GetTextDrawCount(szText, numText, nPixelWidth);
		}
		return 0;
	}

	BOOL						TextFormat::GetDrawSize(const short_t* szText, int numText, int iFont, Size& sz)
	{CPUTime(TextFormat);
		FontNode2D* fontNode	=	GetDlgManager()->GetFontNode(iFont);
		if(fontNode && fontNode->fontPtr.t)
		{
			fontNode->fontPtr->SetFontSize(fontNode->nFontHeight);
			sz = fontNode->fontPtr.t->GetTextDrawSize(szText, numText);
			return true;
		}
		return FALSE;
	}

	INT							TextFormat::GetFirstChar(Pixel pt)
	{CPUTime(TextFormat);
		for(int i=0;i<this->m_lineDrawCount;i++)
		{
			LineTextDraw& ld	=	this->m_lineTextDraw[i];
			if(pt.y >= ld.rc.top && pt.y < ld.rc.bottom)
				return ld.nBegin;
		}
		return -1;
	}

	INT							TextFormat::GetLastChar(Pixel pt)
	{CPUTime(TextFormat);
		int nRet	=	-1;
		for(int i=0;i<this->m_lineDrawCount;i++)
		{
			LineTextDraw& ld	=	this->m_lineTextDraw[i];
			if(pt.y >= ld.rc.top && pt.y < ld.rc.bottom)
			{
				nRet	=	ld.nBegin + ld.nCount;
			}
			else if(nRet >= 0)
				return nRet;
		}
		return nRet;
	}

	INT							TextFormat::GetCharFromPoint(Pixel pt)
	{CPUTime(TextFormat);
		for(int i=0;i<this->m_lineDrawCount;i++)
		{
			LineTextDraw& ld	=	this->m_lineTextDraw[i];
			if(ld.rc.top > pt.y || ld.rc.bottom < pt.y)
				continue;
			if(ld.rc.left > pt.x || ld.rc.right < pt.x)
				continue;
			if(ld.rc.isPtIn(pt.x, pt.y))
			{
				int width	=	pt.x - ld.rc.left;
				return ld.nBegin + GetDrawTextLength(m_maxTextBuffer + ld.nBegin, ld.nCount, this->m_textFormats[ld.formatIndex].nFont, width);
			}
		}
		return -1;
	}

	INT							TextFormat::GetCharLineIndex(INT nCharIndex)	//字符串出现的行
	{CPUTime(TextFormat);
		for(int i=0;i<this->m_lineDrawCount;i++)
		{
			LineTextDraw& ld	=	this->m_lineTextDraw[i];
			if(ld.nBegin > nCharIndex || (ld.nBegin + ld.nCount) < nCharIndex)
				continue;
			return ld.lineIndex;
		}
		return -1;
	}

	INT							TextFormat::GetCharFromLineX(INT nLineIndex, INT pixelX)	//返回pixel位置的第几个字符串
	{CPUTime(TextFormat);
		if(nLineIndex < 0)
			return 0;
		else if(nLineIndex > this->m_maxLineCount)
			return this->m_textLength;
		for(int i=0;i<this->m_lineDrawCount;i++)
		{
			LineTextDraw& ld	=	this->m_lineTextDraw[i];
			if(ld.lineIndex != nLineIndex)
				continue;
			if(pixelX >= ld.rc.left && pixelX <= ld.rc.right)
			{
				int width	=	pixelX - ld.rc.left;
				return ld.nBegin + GetDrawTextLength(m_maxTextBuffer + ld.nBegin, ld.nCount, this->m_textFormats[ld.formatIndex].nFont, width);
			}
		}
		return m_textLength;
	}

	Pixel						TextFormat::GetPointFromChar(INT nCharIndex, INT* nHeight)
	{CPUTime(TextFormat);
		Pixel pt;
		pt.x	=	pt.y	=	0;
		for(int i=0;i<this->m_lineDrawCount;i++)
		{
			LineTextDraw& ld	=	this->m_lineTextDraw[i];
			if(nCharIndex < ld.nBegin  || nCharIndex > (ld.nBegin + ld.nCount))
			{
				pt.x	=	ld.rc.left;
				pt.y	=	ld.rc.top + 1;
				continue;
			}
			int nCount	=	nCharIndex - ld.nBegin;
			Size sz;
			if(m_textFormats[ld.formatIndex].nImageID > 0)
				sz		=	m_pTextRender->GetImageSize(m_textFormats[ld.formatIndex].nImageID);
			else
				GetDrawSize(m_maxTextBuffer + ld.nBegin, nCount, this->m_textFormats[ld.formatIndex].nFont, sz);
			pt.x	=	ld.rc.left + sz.cx;
			if(nHeight)
				*nHeight	=	ld.rc.bottom - ld.rc.top - 1;
			pt.y	=	ld.rc.top + 1;// - sz.cy;
			return pt;
		}
		return pt;
	}

	VOID						TextFormat::SetSel(int nIndex, int nCount)
	{CPUTime(TextFormat);
		SetSelRange(nIndex, nIndex + nCount);
	}

	VOID						TextFormat::ResetSelect()
	{CPUTime(TextFormat);
		if(m_selBegin > m_textLength)
			m_selBegin	=	m_textLength;
		else if(m_selBegin < 0)
			m_selBegin	=	0;
		if(m_selEnd > m_textLength)
			m_selEnd	=	m_textLength;
		else if(m_selEnd < 0)
			m_selEnd	=	0;
		//
	}

	VOID						TextFormat::SetDrawRect(const Rect& rc)
	{CPUTime(TextFormat);
		BOOL bInvalidate	=	(rc.GetWidth() != m_rectDraw.GetWidth() || rc.GetHeight() != m_rectDraw.GetHeight());
		this->m_rectDraw.Set(0, 0, rc.GetWidth(), rc.GetHeight());
		if(bInvalidate)
			this->Invalidate();
	}

	INT						TextFormat::SetEventText(INT nEvent, const short_t* text, int parameter)
	{CPUTime(TextFormat);
		FormatEventText t;
		t.nEvent		=	nEvent;
		t.text			=	text;
		t.parameter		=	parameter;
		m_events.push_back( t );
		return nEvent;
	}

	FormatEventText*				TextFormat::GetEventText(INT nEvent)
	{CPUTime(TextFormat);
		for(size_t i=0;i<m_events.size();i++)
		{
			if(m_events.at(i).nEvent == nEvent)
				return &m_events.at(i);
		}
		return 0;
	}

	VOID						TextFormat::CleanEvent()
	{CPUTime(TextFormat);
		m_events.clear();
	}

	TextFormatData*				TextFormat::GetClickEvent(INT x, INT y)
	{CPUTime(TextFormat);
		if(!this->m_pTextFormatCB)return NULL;
		int nLineHeight		=	0;
		int nMaxTop			=	0;
		int visibleLineCount	=	0;
		int visibleBegin		=	-1;
		int	nLineIndex		=	m_nShowBeginLine;
		for(int i=0;i<m_lineDrawCount;i++)
		{
			LineTextDraw& ld	=	this->m_lineTextDraw[i];
			if(ld.lineIndex < m_nShowBeginLine)
			{
				nLineHeight		=	ld.rc.bottom;
				continue;
			}
			else if(visibleBegin < 0)
				visibleBegin	=	ld.nBegin;
			TextFormatData& d	=	m_textFormats[ld.formatIndex];
			if(d.nEvent <= 0)
				continue;
			//if(d.nImageID > 0)
			//{
			//	continue;
			//}
			if(ld.rc.bottom > this->m_rectDraw.bottom)
				break;
			visibleLineCount	=	ld.lineIndex - m_nShowBeginLine;
			if(visibleLineCount >= m_nShowLineCount)
				break;
			if(x >= ld.rc.left && x <= ld.rc.right && y >= ld.rc.top && y <= ld.rc.bottom)
			{
				return &d;
			}
		}
		return NULL;
	}

	BOOL						TextFormat::OnButtonUp(INT x, INT y)
	{CPUTime(TextFormat);
		if(!this->m_pTextFormatCB)return NULL;
		TextFormatData* pData	=	GetClickEvent(x, y);
		if(pData)
		{
			FormatEventText*	pSelect = 0;
			for(int ie=0;ie<(int)m_events.size();ie++)
			{
				if(m_events.at(ie).nEvent == pData->nEvent)
				{
					pSelect		=	&m_events.at(ie);
					break;
				}
			}
			if(pSelect)
			{
				m_pTextFormatCB->OnEventClick( this, pSelect->text, pSelect->parameter );
				return TRUE;
			}
		}
		return FALSE;
	}

	BOOL						TextFormat::OnMouseMove(INT x, INT y)
	{CPUTime(TextFormat);
		m_pSelectEvent			=	0;
		TextFormatData* pData	=	GetClickEvent(x, y);
		if(pData)
		{
			m_pSelectEvent		=	pData;
			//
			if(!this->m_pTextFormatCB)return FALSE;
			FormatEventText*	pSelect = 0;
			for(int ie=0;ie<(int)m_events.size();ie++)
			{
				if(m_events.at(ie).nEvent == pData->nEvent)
				{
					pSelect		=	&m_events.at(ie);
					break;
				}
			}
			if(pSelect)
			{
				m_lastMoveEventID	=	pSelect->nEvent;
				m_pTextFormatCB->OnEventMoving( this, pSelect->text, pSelect->parameter );
				return TRUE;
			}
			else if(m_lastMoveEventID >= 0)
			{
				m_lastMoveEventID	=	-1;
			}
		}
		return FALSE;
	}

	VOID						TextFormat::SetSelRange(int nBegin, int nEnd)
	{CPUTime(TextFormat);
		INT nLine		=	this->GetCharLineIndex(nBegin);
		if(nLine > (m_nShowBeginLine + m_visibleLineCount))
		{
			m_nShowBeginLine	=	nLine - m_visibleLineCount;
			this->Invalidate();
		}
		else if(nLine < m_nShowBeginLine)
		{
			m_nShowBeginLine	=	nLine;
			this->Invalidate();
		}
		//
		m_selBegin		=	nBegin;
		m_selEnd		=	nEnd;
		if(m_selEnd != m_selBegin)
		{
			if(m_selEnd < m_visibleBegin)
				m_selEnd	=	m_visibleBegin;
			else if(m_selEnd > m_visibleEnd)
				m_selEnd	=	m_visibleEnd;
		}
		ResetSelect();
	}

	VOID						TextFormat::RemoveLine(int nFirstLine, int nRemoveCount)	//删除几行
	{CPUTime(TextFormat);
		m_bChanged	=	true;
		int nBeginLine	=	-1;
		int nEndLine	=	-1;
		int nLastLine	=	nFirstLine + nRemoveCount - 1;
		for(int i=0;i<this->m_lineDrawCount;i++)
		{
			LineTextDraw& ld	=	this->m_lineTextDraw[i];
			if(ld.lineIndex == nFirstLine)
			{
				if(nBeginLine < 0)
					nBeginLine	=	ld.nBegin;
			}
			if(ld.lineIndex == nLastLine)
			{
				nEndLine	=	ld.nBegin + ld.nCount;
			}
		}
		if(nBeginLine < 0 || nEndLine < 0)
			return;
		RemoveText(getmin(nBeginLine, nEndLine),  abs(nEndLine - nBeginLine));
		ResetSelect();
		this->Invalidate();
	}

	VOID						TextFormat::InsertFormatText(const short_t* str, int nCount, DWORD dwColor, INT nFontIndex, INT nImageID, INT nShadowWidth, DWORD shadowColor)
	{CPUTime(TextFormat);
		RemoveSelect();
		int sel			=	m_selBegin;
		nCount			=	Insert(m_selBegin, str, nCount);
		//
		SetFormat(sel, nCount, dwColor, nFontIndex, nImageID, nShadowWidth, shadowColor, 0);
		//
		m_selBegin		+=	nCount;
		m_selEnd		=	m_selBegin;
		this->Invalidate();
	}

	VOID						TextFormat::InsertText(const short_t* str, int nCount)
	{CPUTime(TextFormat);
		m_bChanged	=	true;
		RemoveSelect();
		m_selBegin		+=	Insert(m_selBegin, str, nCount);
		m_selEnd		=	m_selBegin;
		this->Invalidate();
	}

	VOID						TextFormat::RemoveAll()
	{CPUTime(TextFormat);
		m_lastMoveEventID	=	-1;
		m_bChanged	=	true;
		this->Remove(0, -1, false, false);
		this->m_formatCount	=	0;
		m_events.clear();
		RemoveRedoBuffer();
		for(size_t i=0;i<this->m_undoBuffer.size();i++)
		{
			delete this->m_undoBuffer.at(i);
		}
		m_undoBuffer.clear();
		m_pSelectEvent		=	0;
	}

	VOID						TextFormat::RemoveText(int nBegin, int nCount)
	{CPUTime(TextFormat);
		m_bChanged	=	true;
		if(nBegin < 0 || nBegin >= m_textLength)
			return;
		Remove(nBegin, nCount);
		m_selBegin	=	nBegin;
		m_selEnd	=	nBegin;
		this->Invalidate();
	}


	VOID						TextFormat::InsertChar(short_t ch)
	{CPUTime(TextFormat);
		m_bChanged	=	true;
		RemoveSelect();
		//
		short_t buf[5];
		buf[0]			=	ch;
		buf[1]			=	0;
		m_selBegin		+=	Insert(m_selBegin, buf);
		m_selEnd		=	m_selBegin;
		this->Invalidate();
	}

};
