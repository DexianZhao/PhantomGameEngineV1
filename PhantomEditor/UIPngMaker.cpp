/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
// UIPngMaker.cpp : 实现文件
//

#include "stdafx.h"
#include "UIPngMaker.h"
#include "afxdialogex.h"
#include "TgaLoader.h"
#include "../PhantomEngine/encrypt.h"
#include "ViewImageManager.h"

// CUIPngMaker 对话框
//extern	BOOL	GetAuthGUID(AuthGUID* guidRet);

IMPLEMENT_DYNAMIC(CUIPngMaker, CDialogBase)
	
CUIPngMaker::CUIPngMaker(CWnd* pParent)
:CDialogBase(CUIPngMaker::IDD, pParent)
{
}
CUIPngMaker::~CUIPngMaker()
{
}

void	ToUtf8(char* ret, const char* buf)
{
	int len = strlen(buf);
	wchar_t* wbuf = new wchar_t[len*2+1];
	len = ::MultiByteToWideChar(CP_ACP, 0, buf, len, wbuf, len*2+1);
	wbuf[len] = 0;
	len = ::WideCharToMultiByte(CP_UTF8, 0, wbuf, len, ret, strlen(buf)*3, 0, 0);
	ret[len] = 0;
	delete[] wbuf;
}

void CUIPngMaker::DoDataExchange(CDataExchange* pDX)
{
	CDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, ID_FILES_LIST, m_lbAddFile);
	DDX_Control(pDX, IDC_OUTPUT_NAME, m_edOutputName);
	DDX_Control(pDX, IDC_SIZE, m_cbSize);
	DDX_Control(pDX, IDC_EDIT1, m_edScale);
	DDX_Control(pDX, IDC_EDIT2, m_edReserved);
	DDX_Control(pDX, IDC_COMBO1, m_cbDirectory);
	DDX_Control(pDX, IDC_COMBO2, m_cbAlign);
	DDX_Control(pDX, IDC_CHECK1, m_chkAlpha);
}

BEGIN_MESSAGE_MAP(CUIPngMaker, CDialogBase)
	ON_BN_CLICKED(IDC_BROWSER, &CUIPngMaker::OnBnClickedBrowser)
	ON_BN_CLICKED(IDC_REMOVE, &CUIPngMaker::OnBnClickedRemove)
	ON_BN_CLICKED(IDC_CLEAR2, &CUIPngMaker::OnBnClickedClear2)
	ON_BN_CLICKED(IDC_BUTTON1, &CUIPngMaker::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CUIPngMaker::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUILD_ALL, &CUIPngMaker::OnBnClickedBuildAll)
	ON_BN_CLICKED(IDOK, &CUIPngMaker::OnBnClickedOk)
	ON_BN_CLICKED(1005, &CUIPngMaker::OnBnClicked1005)
	ON_BN_CLICKED(1004, &CUIPngMaker::OnBnClicked1004)
	ON_BN_CLICKED(1011, &CUIPngMaker::OnBnClicked1008)
	ON_BN_CLICKED(IDC_BUILD_ALL2, &CUIPngMaker::OnBnClickedBuildAll2)
	ON_BN_CLICKED(IDC_CHECK1, &CUIPngMaker::OnBnClickedCheck1)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CUIPngMaker::OnCbnSelchangeCombo1)
	ON_BN_CLICKED(IDC_BROWSER2, &CUIPngMaker::OnBnClickedBrowser2)
	ON_BN_CLICKED(IDC_BUTTON100, &CUIPngMaker::OnBnClickedButton100)
END_MESSAGE_MAP()


// CUIPngMaker 消息处理程序

BOOL CUIPngMaker::OnInitDialog()
{
	CDialogBase::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。
	m_chkAlpha.SetCheck(1);
	this->m_cbSize.AddString(L"128");
	this->m_cbSize.AddString(L"256");
	this->m_cbSize.AddString(L"512");
	this->m_cbSize.AddString(L"1024");
	this->m_cbSize.AddString(L"2048");
	m_cbDirectory.AddString(L"0");
	m_cbDirectory.AddString(L"1");
	m_cbDirectory.AddString(L"2");
	m_cbDirectory.AddString(L"3");
	m_cbSize.SetCurSel(3);
	m_cbDirectory.SetCurSel(0);
	m_edScale.SetWindowTextW(L"1.0");
	m_edReserved.SetWindowTextW(L"1");
	//
	//m_cbAlign.AddString(L"使用图片原始设大小");
	m_cbAlign.AddString(L"宽度开始计算");
	m_cbAlign.AddString(L"高度开始计算");
	m_cbAlign.SetCurSel(0);
/*
	m_cbAlign.AddString(L"左右【中】，上下【中】,128x128");
	m_cbAlign.AddString(L"左右【中】，下,128x128");
	m_cbAlign.AddString(L"左右【中】，上,128x128");
	m_cbAlign.AddString(L"左右【中】，上下【中】,256x256");
	m_cbAlign.AddString(L"左右【中】，下,256x256");
	m_cbAlign.AddString(L"左右【中】，上,256x256");
	m_cbAlign.AddString(L"左右【中】，上下【中】,512x512");
	m_cbAlign.AddString(L"左右【中】，下,512x512");
	m_cbAlign.AddString(L"左右【中】，上,512x512");*/
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}
const wchar_t*	GetExecutePath(){
	static wchar_t path[256];
	wchar_t drive[256];
	_wsplitpath(_wpgmptr, path, drive, 0, 0);
	wcscat(path, drive);
	return path;
}
void CUIPngMaker::OnBnClickedBrowser()
{
	CFileDialog dlg(TRUE, 0, 0, OFN_EXPLORER|OFN_ENABLESIZING|OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_ALLOWMULTISELECT, L"Alpha Png Files|*.png;*.jpg;*.bmp|TGA files|*.tga|*.*|*.*||", this);
	std::wstring temp;
	temp.resize(1024000);
	dlg.m_pOFN->lpstrFile	=	(wchar_t*)temp.c_str();
	dlg.m_pOFN->lpstrFile[0]	=	0;
	dlg.m_pOFN->nMaxFile	=	temp.size();
	if(dlg.DoModal() != IDOK)
		return;
	SetProjectPath();
	POSITION pos	=	dlg.GetStartPosition();
	const wchar_t* exepath = GetExecutePath();
	while(pos)
	{
		CString strFile	=	dlg.GetNextPathName(pos);
		const wchar_t* szfile = (const wchar_t*)strFile + wcslen(exepath);
		BOOL bFind	=	false;
		for(int i=0;i<m_files.size();i++)
		{
			if(wcsicmp(m_files[i].c_str(), strFile) == 0)
			{
				bFind	=	true;
				break;
			}
		}
		if(!bFind)
		{
			if(m_files.size() == 0)
			{
				wchar_t buf[256], dir[256];
				_wsplitpath(szfile, buf, dir, 0, 0);
				wcscat(buf, dir);
				wcscat(buf, L"img");
				if(m_edOutputName.GetWindowTextLengthW() == 0)
					this->m_edOutputName.SetWindowTextW(buf);
			}
			m_files.push_back((const wchar_t*)szfile);
			//m_lbAddFile.AddString(strFile);
		}
	}
	OnBnClickedCheck1();
}


void CUIPngMaker::OnBnClickedRemove()
{
	for(int i=0;i<m_lbAddFile.GetCount();i++)
	{
		if(m_lbAddFile.GetSel(i))
		{
			m_files.erase(m_files.begin() + i);
			m_lbAddFile.DeleteString(i);
			i--;
		}
	}
}


void CUIPngMaker::OnBnClickedClear2()
{
	m_files.clear();
	m_lbAddFile.ResetContent();
}


void CUIPngMaker::OnBnClickedButton1()
{
	wchar_t szPath[256], szFile[256], szDir[256];
	if(fexist(A(m_loadFile).buf))
	{
		_wsplitpath(m_loadFile, szPath, szDir, szFile, 0);
	}
	else
	{
		CFileDialog dlg(false, 0, 0, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, L"UI配置文件|*.pngui|*.*|*.*||", this);
		if(dlg.DoModal() != IDOK)
			return;
		_wsplitpath(dlg.GetPathName(), szPath, szDir, szFile, 0);
	}
	SetProjectPath();
	wcscat(szPath, szDir);
	wcscat(szPath, szFile);
	wcscat(szPath, L".pngui");
	CString strOutput;
	this->m_edOutputName.GetWindowTextW(strOutput);
	//
	FILE* file	=	::_wfopen(szPath, L"wb");
	int version = 5;
	fwrite(&version, sizeof(int), 1, file);
	INT length	=	strOutput.GetLength() + 1;
	fwrite(&length, sizeof(int), 1, file);
	fwrite((const wchar_t*)strOutput, length * sizeof(wchar_t), 1, file);
	int nCount	=	this->m_files.size();
	fwrite(&nCount, sizeof(int), 1, file);
	for(int i=0;i<nCount;i++)
	{
		int len	=	m_files[i].size() + 1;
		fwrite(&len, sizeof(int), 1, file);
		fwrite((const wchar_t*)m_files[i].c_str(), len * sizeof(wchar_t), 1, file);
	}
	int sel	=	m_cbSize.GetCurSel();
	fwrite(&sel, sizeof(int), 1, file);
	int ncb = m_cbDirectory.GetCurSel();
	fwrite(&ncb, sizeof(int), 1, file);
	CString str;
	m_edScale.GetWindowTextW(str);
	float fScale = _wtof(str);
	fwrite(&fScale, sizeof(float), 1, file);
	int nAlign = m_cbAlign.GetCurSel();
	fwrite(&nAlign, sizeof(int), 1, file);
	int nAlpha = 0;
	int nReserved = 0;
	CString szReserved;
	nAlpha = this->m_chkAlpha.GetCheck();
	this->m_edReserved.GetWindowTextW(szReserved);
	nReserved = _wtoi(szReserved);
	fwrite(&nAlpha, sizeof(int), 1, file);
	fwrite(&nReserved, sizeof(int), 1, file);
	fclose(file);
}


void CUIPngMaker::OnBnClickedButton2()
{
	CFileDialog dlg(true, 0, 0, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, L"UI配置文件|*.pngui|*.*|*.*||", this);
	if(dlg.DoModal() != IDOK)
		return;
	SetProjectPath();
	//
	m_loadFile = dlg.GetPathName();
	m_files.clear();
	this->m_lbAddFile.ResetContent();
	//
	FILE* file	=	::_wfopen(dlg.GetPathName(), L"rb");
	int version = 1;
	fread(&version, sizeof(int), 1, file);
	INT length;
	wchar_t buf[1024];
	fread(&length, sizeof(int), 1, file);
	fread(buf, length * sizeof(wchar_t), 1, file);
	this->m_edOutputName.SetWindowTextW(buf);
	int nCount = 0;
	fread(&nCount, sizeof(int), 1, file);
	for(int i=0;i<nCount;i++)
	{
		int len;
		fread(&len, sizeof(int), 1, file);
		fread(buf, len * sizeof(wchar_t), 1, file);
		m_lbAddFile.AddString(buf);
		m_files.push_back(buf);
	}
	int sel;//	=	m_cbSize.GetCurSel();
	fread(&sel, sizeof(int), 1, file);
	m_cbSize.SetCurSel(sel);
	if(version > 1)
	{
		int ncb = 0;
		fread(&ncb, sizeof(int), 1, file);
		m_cbDirectory.SetCurSel(ncb);
	}
	if(version > 2)
	{
		float fScale = 1;
		fread(&fScale, sizeof(float), 1, file);
		wchar_t buf[128];
		swprintf(buf, L"%.02f", fScale);
		m_edScale.SetWindowTextW(buf);
	}
	else
		m_edScale.SetWindowTextW(L"1.0");
	if(version > 3)
	{
		int nAlign = 0;
		fread(&nAlign, sizeof(int), 1, file);
		m_cbAlign.SetCurSel(nAlign);
	}
	if(version >= 5)
	{
		int nAlpha;
		int nReserved;
		fread(&nAlpha, sizeof(int), 1, file);
		fread(&nReserved, sizeof(int), 1, file);
		m_chkAlpha.SetCheck(nAlpha);
		wchar_t buf[128];
		swprintf(buf, L"%d", nReserved);
		m_edReserved.SetWindowTextW(buf);
	}
	fclose(file);
}

struct ImageRect
{
	CImage*	img;
	CRect	rect;
	CRect	imgRect;
	CPoint	offset;
	wchar_t fileName[128];
};
struct ImageFile
{
	CImage	img;
	wchar_t fileName[128];
	CRect	imgRect;
	CPoint	offset;
	VOID	Optimize(BOOL bAlpha);
};

VOID	ImageFile::Optimize(BOOL bAlpha)
{
	int nWidth	=	this->img.GetWidth();
	int nHeight	=	this->img.GetHeight();
	imgRect.SetRect(0, 0, nWidth, nHeight);
	if(img.GetBPP() < 32 || !bAlpha)
		return;
	for(int x=0;x<nWidth;x++)
	{
		BOOL bZero	=	true;
		for(int y=0;y<nHeight;y++)
		{
			BYTE* sRGBA = (BYTE*)img.GetPixelAddress(x, y);
			int alpha = sRGBA[3];
			if(alpha > 0)
			{
				bZero	=	false;
				break;
			}
		}
		if(bZero)
			imgRect.left	=	x;
		else
			break;
	}
	for(int x=nWidth - 1;x >= 0;x--)
	{
		BOOL bZero	=	true;
		for(int y=0;y<nHeight;y++)
		{
			BYTE* sRGBA = (BYTE*)img.GetPixelAddress(x, y);
			int alpha = sRGBA[3];
			if(alpha > 0)
			{
				bZero	=	false;
				break;
			}
		}
		if(bZero)
			imgRect.right	=	x;
		else
			break;
	}
	for(int y=0;y<nHeight;y++)
	{
		BOOL bZero	=	true;
		for(int x=0;x<nWidth;x++)
		{
			BYTE* sRGBA = (BYTE*)img.GetPixelAddress(x, y);
			int alpha = sRGBA[3];
			if(alpha > 0)
			{
				bZero	=	false;
				break;
			}
		}
		if(bZero)
			imgRect.top	=	y;
		else
			break;
	}
	for(int y=nHeight - 1;y >= 0;y--)
	{
		BOOL bZero	=	true;
		for(int x=0;x<nWidth;x++)
		{
			BYTE* sRGBA = (BYTE*)img.GetPixelAddress(x, y);
			int alpha = sRGBA[3];
			if(alpha > 0)
			{
				bZero	=	false;
				break;
			}
		}
		if(bZero)
			imgRect.bottom	=	y;
		else
			break;
	}
	//imgRect.left-=2;
	//imgRect.top-=2;
	//imgRect.right+=2;
	//imgRect.bottom+=2;
	//if(imgRect.left<0)imgRect.left=0;
	//if(imgRect.top<0)imgRect.top=0;
	//if(imgRect.right>img.GetWidth())
	//	imgRect.right=img.GetWidth();
	//if(imgRect.bottom>img.GetHeight())
	//	imgRect.bottom=img.GetHeight();
	if(imgRect.right <= imgRect.left)
		imgRect.right = imgRect.left + 1;
	if(imgRect.bottom <= imgRect.top)
		imgRect.bottom = imgRect.top + 1;
}

int	sortImage(const void* v1, const void* v2)
{
	ImageFile* img1 = *(ImageFile**)v1;
	ImageFile* img2 = *(ImageFile**)v2;
	int w1	=	img1->imgRect.Width();//GetWidth();
	int h1	=	img1->imgRect.Height();//GetHeight();
	int h2	=	img2->imgRect.Height();//GetHeight();
	int w2	=	img2->imgRect.Width();//GetWidth();
	//
	if(w1 < w2 && h1 < h2)
		return 1;
	if(w1 < w2)
		return 1;
	//int n1	=	w1 * h1;
	//int n2	=	w2 * h2;
	//if(n1 < n2)
	//	return -1;
	return -1;
}

int	sortImageNew(const void* v1, const void* v2)
{
	ImageFile* img1 = *(ImageFile**)v1;
	ImageFile* img2 = *(ImageFile**)v2;
	int w1	=	img1->imgRect.Width();//GetWidth();
	int h1	=	img1->imgRect.Height();//GetHeight();
	int h2	=	img2->imgRect.Height();//GetHeight();
	int w2	=	img2->imgRect.Width();//GetWidth();
	int m1 = w1*h1;
	int m2 = w2*h2;
	if(m1 > m2)
		return -1;
	else if(m1 < m2)
		return 1;
	return 0;
}

int	sortImageH(const void* v1, const void* v2)
{
	ImageFile* img1 = *(ImageFile**)v1;
	ImageFile* img2 = *(ImageFile**)v2;
	int w1	=	img1->imgRect.Width();//GetWidth();
	int h1	=	img1->imgRect.Height();//GetHeight();
	int h2	=	img2->imgRect.Height();//GetHeight();
	int w2	=	img2->imgRect.Width();//GetWidth();
	//
	if(w1 < w2 && h1 < h2)
		return 1;
	if(h1 < h2)
		return 1;
	//int n1	=	w1 * h1;
	//int n2	=	w2 * h2;
	//if(n1 < n2)
	//	return -1;
	return -1;
}
struct LineImage
{
	int							maxHeight;
	std::vector<ImageRect>		images;
	LineImage()
	{
		maxHeight	=	0;
	}
};
struct	OutpuImage
{
	CImage					img;
	std::vector<ImageRect>	rects;
};
struct toA
{
	char* buf;
	toA(const wchar_t* w)
	{
		int len = (int)wcslen(w) + 1;
		buf = new char[len * 2];
		len = ::WideCharToMultiByte(CP_ACP, 0, w, len - 1, buf, len * 2, 0, 0);
		buf[len] = 0;
	}
	inline	operator const char*(){return buf;}
	~toA(){if(buf)delete[] buf;}
};

BOOL	GetImageSaveData(CDynamicArray<char>& rets, CImage& img)
{
	HGLOBAL hGlobal = GlobalAlloc(GMEM_FIXED, 1024*1024*10);
	LARGE_INTEGER l;
	l.QuadPart = 0;
	IStream* pStream = 0;
	//
	if(SUCCEEDED(::CreateStreamOnHGlobal(hGlobal, false, &pStream)))
	{
		pStream->Seek(l, SEEK_SET, 0);
		if(FAILED(img.Save(pStream, Gdiplus::ImageFormatPNG)))
		{
			GlobalFree(hGlobal);
			return false;
		}
		ULARGE_INTEGER newPos;
		l.HighPart = 0;
		l.LowPart = 0;
		pStream->Seek(l, SEEK_CUR, &newPos);
		int fileLength = newPos.LowPart;
		rets.SetArrayCount(fileLength);
		pStream->Seek(l, SEEK_SET, 0);
		ULONG length = 0;
		pStream->Read(rets.GetPtr(), fileLength, &length);
		assert(length == fileLength);
		pStream->Release();
	}
	GlobalFree(hGlobal);
	return true;
}

//判断一个矩形是否可以进入这个所有矩形区域内,rects是已经被插入的所有矩形，如果不能再插入了就返回false
BOOL	GetExistRectW(std::vector<CRect>& rects, CRect& ret, CSize& into, int width, int height)
{
	char* ch = new char[width * height];
	memset(ch, 0, width * height);
	for(int i=0;i<rects.size();i++)
	{
		CRect rc = rects[i];
		for(int x=rc.left;x<rc.right;x++)
		{
			for(int y=rc.top;y<rc.bottom;y++)
			{
				if(x <0 || x>=width || y<0||y>=height)
					continue;
				ch[width * y + x] = 1;
			}
		}
	}
	for(int x=0;x<width;x++)
	{
		for(int y=0;y<height;y++)
		{
			BOOL bFind	=	true;
			for(int xx=0;xx<into.cx;xx++)
			{
				int x1 = x + xx;
				if(x1 >= width)
				{
					bFind = false;
					break;
				}
				for(int yy=0;yy<into.cy;yy++)
				{
					int y1 = y + yy;
					if(y1 >= height)
					{
						bFind = false;
						break;
					}
					if(ch[width * y1 + x1])
					{
						bFind = false;
						break;
					}
				}
				if(!bFind)
					break;
			}
			if(bFind)
			{
				CRect rc(x,y,x+into.cx, y+into.cy);
				ret = rc;
				rects.push_back(rc);
				delete ch;
				return true;
			}
		}
	}
	delete ch;
	return false;
}

CDynamicArray<char>	g_freePixels;
//判断一个矩形是否可以进入这个所有矩形区域内,rects是已经被插入的所有矩形，如果不能再插入了就返回false
BOOL	GetFreeRect(CRect& ret, CSize& into, int width, int height)
{
	if(g_freePixels.size() == 0)
	{
		g_freePixels.SetArrayCount(width*height);
	}
	char* ch = g_freePixels.GetPtr();
	for(int x=0;x<width;x++)
	{
		for(int y=0;y<height;y++)
		{
			BOOL bFind	=	true;
			for(int xx=0;xx<into.cx;xx++)
			{
				int x1 = x + xx;
				if(x1 >= width)
				{
					bFind = false;
					break;
				}
				for(int yy=0;yy<into.cy;yy++)
				{
					int y1 = y + yy;
					if(y1 >= height)
					{
						bFind = false;
						break;
					}
					if(ch[width * y1 + x1])
					{
						bFind = false;
						break;
					}
				}
				if(!bFind)
					break;
			}
			if(bFind)
			{
				ret.SetRect(x,y,x+into.cx, y+into.cy);
				for(int x=ret.left;x<ret.right;x++)
				{
					for(int y=ret.top;y<ret.bottom;y++)
					{
						if(x <0 || x>=width || y<0||y>=height)
							continue;
						ch[width * y + x] = 1;
					}
				}
				return true;
			}
		}
	}

	return false;
}

VOID	ScaleImage(CImage& dest, CImage& src, float fScale, int nAlign)
{
	//m_cbAlign.AddString(L"左右【中】，上下【中】,256x256");
	//m_cbAlign.AddString(L"左右【中】，下,256x256");
	//m_cbAlign.AddString(L"左右【中】，上,256x256");
	//m_cbAlign.AddString(L"左右【中】，上下【中】,512x512");
	//m_cbAlign.AddString(L"左右【中】，下,512x512");
	//m_cbAlign.AddString(L"左右【中】，上,512x512");
	int newWidth = (float)src.GetWidth() * fScale;
	int newHeight = (float)src.GetHeight() * fScale;

	int imageW = newWidth;
	int imageH = newHeight;
	switch(nAlign)
	{
	case 1:
	case 2:
	case 3:
		imageW = imageH = 128;
		break;
	case 4:
	case 5:
	case 6:
		imageW = imageH = 256;
		break;
	case 7:
	case 8:
	case 9:
		imageW = imageH = 512;
		break;
	}
	dest.Create(imageW, imageH, src.GetBPP(), src.GetBPP() == 32 ? CImage::createAlphaChannel : 0);
	float fS1 = 1.0f / fScale;
	for(int x=0;x<imageW;x++)
	{
		for(int y=0;y<imageH;y++)
		{
			BYTE* dRGB = (BYTE*)dest.GetPixelAddress(x, y);
			for(int i=0;i<3;i++)
				dRGB[i] = 0;
			if(src.GetBPP() == 32)
				dRGB[3] = 0;
		}
	}
	for(int x=0;x<newWidth;x++)
	{
		for(int y=0;y<newHeight;y++)
		{
			int dx = x;
			int dy = y;
			switch(nAlign)
			{
			case 1:
				dx = (64 - newWidth / 2 + x);
				dy = (64 - newHeight / 2 + y);
				break;
			case 2:
				dx = (64 - newWidth / 2 + x);
				dy = (128 - newHeight + y);
				break;
			case 3:
				dx = (64 - newWidth / 2 + x);
				dy = y;
				break;
			case 4:
				dx = (128 - newWidth / 2 + x);
				dy = (128 - newHeight / 2 + y);
				break;
			case 5:
				dx = (128 - newWidth / 2 + x);
				dy = (256 - newHeight + y);
				break;
			case 6:
				dx = (128 - newWidth / 2 + x);
				dy = y;
				break;
			case 7:
				dx = (256 - newWidth / 2 + x);
				dy = (256 - newHeight / 2 + y);
				break;
			case 8:
				dx = (256 - newWidth / 2 + x);
				dy = (512 - newHeight + y);
				break;
			case 9:
				dx = (256 - newWidth / 2 + x);
				dy = y;
				break;
			}
			if(dx < 0 || dy < 0 || dx >= imageW || dy >= imageH)
				continue;
			int xx = fS1 * (float)x;
			int yy = fS1 * (float)y;
			BYTE* sRGB = (BYTE*)src.GetPixelAddress(xx, yy);
			BYTE* dRGB = (BYTE*)dest.GetPixelAddress(dx, dy);
			for(int i=0;i<3;i++)
				dRGB[i] = sRGB[i];
			if(src.GetBPP() == 32)
				dRGB[3] = sRGB[3];
		}
	}
}

//VOID	GUIDToGenuine(Phantom::GenuineGUID& ret, AuthGUID& guid)
//{
//	memcpy(&ret, &guid, sizeof(AuthGUID));
//}

void CUIPngMaker::OnBnClickedBuildAll()
{
	CString strOutputName;
	this->m_edOutputName.GetWindowTextW(strOutputName);
	if(strOutputName.GetLength() <= 1)
	{
		MessageBox(L"写入输出文件名，和相对路径");
		return;
	}
	wchar_t szPath[512];
	wcscpy(szPath, W(GetProjectPath()).buf);
	wcscat(szPath, strOutputName);
	//
	std::vector<ImageFile*>		images;
	std::vector<OutpuImage*>	outputs;
	std::vector<OutpuImage*>	outputsH;
	INT nCount	=	this->m_files.size();//.GetCount();
	CString szScale;
	m_edScale.GetWindowTextW(szScale);
	float fScale = _wtof(szScale);
	if(fScale < 0.1)fScale = 0.1;
	else if(fScale > 1)fScale = 1;
	int nAlign = m_cbAlign.GetCurSel();
	const wchar_t* exepath = GetExecutePath();
	for(int i=0;i<nCount;i++)
	{
		CString strFile;
		this->m_lbAddFile.GetText(i, strFile);
		wchar_t szFilePath[512];
		wcscpy(szFilePath, exepath);
		wcscat(szFilePath, m_files[i].c_str());
		wchar_t szExt[128];
		_wsplitpath(strFile, 0, 0, 0, szExt);
		ImageFile* img = new ImageFile();
		img->offset.SetPoint(0, 0);
		BOOL bLoadOK	=	false;
		CImage temp;
		if(wcsicmp(szExt, L".tga") == 0)
		{
			Phantom::StreamBase* ss	=	open_s(A(szFilePath), true);
			TextureTga tex;
			if(ss && LoadTGA(&tex, ss))
			{
				bLoadOK	=	true;
				tex.ConvertToImage(temp);//img->img);
			}
			else
				bLoadOK	=	false;
			if(ss)
				close_s(&ss);
		}
		else
		{
			bLoadOK	=	SUCCEEDED(temp.Load(szFilePath));
		}
		if(bLoadOK)
			ScaleImage(img->img, temp, fScale, 0);//nAlign);
		if(!bLoadOK)//FAILED(img->img.Load(strFile)))
		{
			MessageBox(szFilePath, L"can't open png file");
			delete img;
		}
		else if(img->img.GetBPP() < 24)
		{
			MessageBox(szFilePath, L"only support 24 bit, and 32bit png file");
			delete img;
		}
		else
		{
			img->imgRect.SetRect(0, 0, img->img.GetWidth(), img->img.GetHeight());
			images.push_back(img);
			//if(m_chkDir.GetCheck())
			{
				wcscpy(img->fileName, strFile);
			}
			//else
				//_wsplitpath(strFile, 0, 0, img->fileName, 0);
			img->Optimize(m_chkAlpha.GetCheck());
			img->offset.x	=	img->imgRect.left;
			img->offset.y	=	img->imgRect.top;
			//wchar_t buf[512];
			//swprintf(buf, L"%s%d.png", szPath, i);
			//img->Save(buf);
		}
	}
	CString str;
	m_cbSize.GetWindowTextW(str);
	int imageSize	=	_wtoi(str);
	std::vector<std::vector<ImageFile*> > maps;
	for(int i=0;i<imageSize;i++)
	{
		std::vector<ImageFile*> v;
		maps.push_back(v);
	}
	if(images.size() > 1)
	{
		int maxW = 0;
		int maxH = 0;
		for(int i=0;i<images.size();i++)
		{
			maxH += images[i]->imgRect.Height();
			maxW += images[i]->imgRect.Width();
		}
		if(m_cbAlign.GetCurSel() <= 0)
			qsort(&images.at(0), images.size(), sizeof(ImageFile*), sortImage);
		else
		{
			//qsort(&images.at(0), images.size(), sizeof(ImageFile*), sortImageH);
			for(int i=0;i<images.size();i++)
			{
				int height = images[i]->imgRect.Height();
				maps[height].push_back(images[i]);
			}
			images.clear();
			for(int i=maps.size()-1;i>0;i--)
			{
				for(int j=0;j<maps[i].size();j++)
				{
					images.push_back(maps[i].at(j));
				}
			}
		}
	}
	//CImage* imageOut	=	new CImage();
	//imageOut->Create(imageSize, imageSize, 32);
	//outputs.push_back(imageOut);
	int reserved = 1;
	CString szReserved;
	m_edReserved.GetWindowText(szReserved);
	reserved = _wtoi(szReserved);
	if(reserved<1)reserved = 1;
	std::vector< LineImage* > lines;
	std::vector< LineImage* > linesH;
	//
	OutpuImage* imgOutput	=	new OutpuImage();
	imgOutput->img.Create(imageSize, imageSize, 32, CImage::createAlphaChannel);
	outputs.push_back(imgOutput);
	int rect = 0;
	g_freePixels.clear();
	for(int img=0;img<images.size();img++)
	{
		ImageRect imgRect;
		imgRect.img = &images[img]->img;
		wcscpy(imgRect.fileName, images[img]->fileName);
		imgRect.offset		=	images[img]->offset;
		imgRect.imgRect		=	images[img]->imgRect;
		int nWidth			=	images[img]->imgRect.Width();
		int nHeight			=	images[img]->imgRect.Height();
		if(!GetFreeRect(imgRect.rect, CSize(nWidth + (nWidth==1024?0:reserved), nHeight+(nHeight==1024?0:reserved)), imageSize, imageSize))
		{
			MessageBox(Language(L"将会生成超过1个图片，不能继续？"), 0, MB_OK);
			{
				delete imgOutput;
				for(int i=0;i<images.size();i++)
					delete images[i];
				return;
			}
			imgOutput	=	new OutpuImage();
			imgOutput->img.Create(imageSize, imageSize, 32, CImage::createAlphaChannel);
			outputs.push_back(imgOutput);
			g_freePixels.clear();
		}
		imgOutput->rects.push_back(imgRect);
		//
		for(int x=0;x<nWidth;x++)
		{
			for(int y=0;y<nHeight;y++)
			{
				int dx	=	imgRect.rect.left + x;
				int dy	=	imgRect.rect.top + y;
				if(dx > imgOutput->img.GetWidth() || dy > imgOutput->img.GetHeight())
				{
					MessageBox(Language(L"目标文件太小，要加大图片或者文件自己分割一下"));
					return;
				}
				BYTE* dRGBA = (BYTE*)imgOutput->img.GetPixelAddress(dx, dy);
				int sx	=	x + imgRect.imgRect.left;
				int sy	=	y + imgRect.imgRect.top;
				if(imgRect.img->GetBPP() == 32)
				{
					BYTE* sRGBA = (BYTE*)imgRect.img->GetPixelAddress(sx, sy);
					dRGBA[0] = sRGBA[0];
					dRGBA[1] = sRGBA[1];
					dRGBA[2] = sRGBA[2];
					dRGBA[3] = sRGBA[3];
				}
				else
				{
					BYTE* sRGB = (BYTE*)imgRect.img->GetPixelAddress(sx, sy);
					dRGBA[0] = sRGB[0];
					dRGBA[1] = sRGB[1];
					dRGBA[2] = sRGB[2];
					dRGBA[3] = 255;
				}
			}
		}
	}
	//{
	//	std::vector<CRect> optRects;
	//	LineImage* line = new LineImage();
	//	lines.push_back(line);
	//	int nLeft	=	0;
	//	for(int i=0;i<images.size();i++)
	//	{
	//		CImage* img	=	&images[i]->img;
	//		int nWidth	=	images[i]->imgRect.Width() + reserved;
	//		int nHeight	=	images[i]->imgRect.Height() + reserved;
	//		if(nWidth	>	imageSize)
	//			imageSize	*=	2;
	//		if(nHeight > imageSize)
	//			imageSize	*=	2;
	//		int nRight	=	nLeft + nWidth;
	//		CRect rcImage(nLeft, 0, nLeft + nWidth, nHeight);//rcImage(nLeft + 1, 1, nLeft + nWidth - 1, nHeight - 1);
	//		if(nRight > imageSize)
	//		{
	//			CRect retRect;
	//			if(!GetExistRectW(optRects, retRect, CSize(nWidth, nHeight), imageSize, imageSize))
	//			{
	//				line = new LineImage();
	//				lines.push_back(line);
	//				nLeft	=	0;
	//				rcImage.SetRect(nLeft, 0, nLeft + nWidth, nHeight);
	//				nLeft		+=	nWidth;
	//			}
	//			else
	//				rcImage = retRect;
	//		}
	//		else
	//			nLeft		+=	nWidth;
	//		optRects.push_back(rcImage);
	//		line->maxHeight	=	max(line->maxHeight, nHeight);
	//		ImageRect l;
	//		l.img		=	img;
	//		l.rect		=	rcImage;
	//		l.offset	=	images[i]->offset;
	//		l.imgRect	=	images[i]->imgRect;
	//		wcscpy(l.fileName, images[i]->fileName);
	//		line->images.push_back(l);
	//	}
	//	OutpuImage* imgOutput	=	new OutpuImage();
	//	//if(!imgOutput->IsNull())
	//	//imgOutput->Destroy();
	//	imgOutput->img.Create(imageSize, imageSize, 32, CImage::createAlphaChannel);
	//	outputs.push_back(imgOutput);
	//	int nTop	=	0;
	//	//GetExistRectW
	//	for(int i=0;i<lines.size();i++)
	//	{
	//		int nBottom	=	nTop + lines[i]->maxHeight;
	//		int jBegin = 0;
	//		if(nBottom > imageSize)
	//		{
	//			for(jBegin=0;jBegin<lines[i]->images.size();jBegin++)
	//			{
	//				ImageRect& imgRect	=	lines[i]->images[jBegin];
	//				ImageRect srcRect	=	imgRect;
	//				srcRect.rect.top	+=	nTop;
	//				srcRect.rect.bottom	+=	nTop;
	//				CRect& srcR		=	srcRect.imgRect;
	//				int nWidth		=	srcR.Width();
	//				int nHeight		=	srcR.Height();
	//				CRect retRect;
	//				if(!GetExistRectW(optRects, retRect, CSize(nWidth, nHeight), imageSize, imageSize))
	//					break;
	//				srcRect.rect = retRect;
	//				imgOutput->rects.push_back(srcRect);
	//				for(int x=0;x<nWidth;x++)
	//				{
	//					for(int y=0;y<nHeight;y++)
	//					{
	//						int dx	=	srcRect.rect.left + x;
	//						int dy	=	srcRect.rect.top + y;
	//						BYTE* dRGBA = (BYTE*)imgOutput->img.GetPixelAddress(dx, dy);
	//						int sx	=	x + srcR.left;
	//						int sy	=	y + srcR.top;
	//						if(imgRect.img->GetBPP() == 32)
	//						{
	//							BYTE* sRGBA = (BYTE*)imgRect.img->GetPixelAddress(sx, sy);
	//							dRGBA[0] = sRGBA[0];
	//							dRGBA[1] = sRGBA[1];
	//							dRGBA[2] = sRGBA[2];
	//							dRGBA[3] = sRGBA[3];
	//						}
	//						else
	//						{
	//							BYTE* sRGB = (BYTE*)imgRect.img->GetPixelAddress(sx, sy);
	//							dRGBA[0] = sRGB[0];
	//							dRGBA[1] = sRGB[1];
	//							dRGBA[2] = sRGB[2];
	//							dRGBA[3] = 255;
	//						}
	//					}
	//				}
	//			}
	//			optRects.clear();
	//			imgOutput	=	new OutpuImage();
	//			imgOutput->img.Create(imageSize, imageSize, 32, CImage::createAlphaChannel);
	//			outputs.push_back(imgOutput);
	//			nTop	=	0;
	//		}
	//		optRects.clear();
	//		for(int j=jBegin;j<lines[i]->images.size();j++)
	//		{
	//			ImageRect& imgRect	=	lines[i]->images[j];
	//			ImageRect srcRect	=	imgRect;
	//			srcRect.rect.top	+=	nTop;
	//			srcRect.rect.bottom	+=	nTop;
	//			imgOutput->rects.push_back(srcRect);
	//			optRects.push_back(srcRect.rect);
	//			//
	//			CRect& srcR		=	srcRect.imgRect;
	//			int nWidth		=	srcR.Width();
	//			int nHeight		=	srcR.Height();
	//			for(int x=0;x<nWidth;x++)
	//			{
	//				for(int y=0;y<nHeight;y++)
	//				{
	//					int dx	=	srcRect.rect.left + x;
	//					int dy	=	srcRect.rect.top + y;
	//					if(dx > imgOutput->img.GetWidth() || dy > imgOutput->img.GetHeight())
	//					{
	//						MessageBox(Language(L"目标文件太小，要加大图片或者文件自己分割一下"));
	//						return;
	//					}
	//					BYTE* dRGBA = (BYTE*)imgOutput->img.GetPixelAddress(dx, dy);
	//					int sx	=	x + srcR.left;
	//					int sy	=	y + srcR.top;
	//					if(imgRect.img->GetBPP() == 32)
	//					{
	//						BYTE* sRGBA = (BYTE*)imgRect.img->GetPixelAddress(sx, sy);
	//						dRGBA[0] = sRGBA[0];
	//						dRGBA[1] = sRGBA[1];
	//						dRGBA[2] = sRGBA[2];
	//						dRGBA[3] = sRGBA[3];
	//					}
	//					else
	//					{
	//						BYTE* sRGB = (BYTE*)imgRect.img->GetPixelAddress(sx, sy);
	//						dRGBA[0] = sRGB[0];
	//						dRGBA[1] = sRGB[1];
	//						dRGBA[2] = sRGB[2];
	//						dRGBA[3] = 255;
	//					}
	//				}
	//			}
	//		}
	//		nTop	+=	lines[i]->maxHeight;
	//	}
	//}
	//{
	//	if(images.size() > 1)
	//	{
	//		qsort(&images.at(0), images.size(), sizeof(ImageFile*), sortImageH);
	//	}
	//	LineImage* line = new LineImage();
	//	linesH.push_back(line);
	//	int nTop	=	0;
	//	for(int i=0;i<images.size();i++)
	//	{
	//		CImage* img	=	&images[i]->img;
	//		int nWidth	=	images[i]->imgRect.Width() + reserved;// + 2;
	//		int nHeight	=	images[i]->imgRect.Height() + reserved;// + 2;
	//		if(nWidth	>	imageSize)
	//			imageSize	*=	2;
	//		if(nHeight > imageSize)
	//			imageSize	*=	2;
	//		int nBottom	=	nTop + nHeight;
	//		if(nBottom > imageSize)
	//		{
	//			line = new LineImage();
	//			linesH.push_back(line);
	//			nTop	=	0;
	//		}
	//		CRect rcImage(0, nTop, nWidth, nTop + nHeight);//rcImage(1, nTop + 1, nWidth - 1, nTop + nHeight - 1);
	//		line->maxHeight	=	max(line->maxHeight, nWidth);
	//		nTop		+=	nHeight;
	//		ImageRect l;
	//		l.img		=	img;
	//		l.rect		=	rcImage;
	//		l.offset	=	images[i]->offset;
	//		l.imgRect	=	images[i]->imgRect;
	//		wcscpy(l.fileName, images[i]->fileName);
	//		line->images.push_back(l);
	//	}
	//	OutpuImage* imgOutput	=	new OutpuImage();
	//	//if(!imgOutput->IsNull())
	//	//imgOutput->Destroy();
	//	imgOutput->img.Create(imageSize, imageSize, 32, CImage::createAlphaChannel);
	//	outputsH.push_back(imgOutput);
	//	int nLeft	=	0;
	//	for(int i=0;i<linesH.size();i++)
	//	{
	//		int nRight	=	nLeft + linesH[i]->maxHeight;
	//		if(nRight > imageSize)
	//		{
	//			imgOutput	=	new OutpuImage();
	//			imgOutput->img.Create(imageSize, imageSize, 32, CImage::createAlphaChannel);
	//			outputsH.push_back(imgOutput);
	//			nLeft	=	0;
	//		}
	//		for(int j=0;j<linesH[i]->images.size();j++)
	//		{
	//			ImageRect& imgRect	=	linesH[i]->images[j];
	//			ImageRect srcRect	=	imgRect;
	//			srcRect.rect.left	+=	nLeft;
	//			srcRect.rect.right	+=	nLeft;
	//			imgOutput->rects.push_back(srcRect);
	//			CRect& srcR		=	srcRect.imgRect;
	//			int nWidth		=	srcR.Width();
	//			int nHeight		=	srcR.Height();
	//			for(int x=0;x<nWidth;x++)
	//			{
	//				for(int y=0;y<nHeight;y++)
	//				{
	//					int dx	=	srcRect.rect.left + x;
	//					int dy	=	srcRect.rect.top + y;
	//					BYTE* dRGBA = (BYTE*)imgOutput->img.GetPixelAddress(dx, dy);
	//					int sx	=	x + srcR.left;
	//					int sy	=	y + srcR.top;
	//					if(imgRect.img->GetBPP() == 32)
	//					{
	//						BYTE* sRGBA = (BYTE*)imgRect.img->GetPixelAddress(sx, sy);
	//						dRGBA[0] = sRGBA[0];
	//						dRGBA[1] = sRGBA[1];
	//						dRGBA[2] = sRGBA[2];
	//						dRGBA[3] = sRGBA[3];
	//					}
	//					else
	//					{
	//						BYTE* sRGB = (BYTE*)imgRect.img->GetPixelAddress(sx, sy);
	//						dRGBA[0] = sRGB[0];
	//						dRGBA[1] = sRGB[1];
	//						dRGBA[2] = sRGB[2];
	//						dRGBA[3] = 255;
	//					}
	//				}
	//			}
	//		}
	//		nLeft	+=	linesH[i]->maxHeight;
	//	}
	//}
	std::vector<OutpuImage*>*	outputsPtr = &outputs;
	//if(outputsH.size() < outputs.size())
	//	outputsPtr = &outputsH;
	SetProjectPath();
	CDynamicArray<char> rets;
	int tableBegin = 0;
	wchar_t buffile[512];
	for(int i=0;i<(*outputsPtr).size();i++)
	{
		if((*outputsPtr)[i]->rects.size() == 0)
			continue;
		if(i > 0)
			swprintf(buffile, L"%s%d.png", szPath, i);
		else
			swprintf(buffile, L"%s.png", szPath);
		
		//GetImageSaveData(rets, (*outputsPtr)[i]->img);
		(*outputsPtr)[i]->img.Save(buffile);
		//
		wchar_t buf[256];
		if(i > 0)
			swprintf(buf, L"%s%d.rect", szPath, i);
		else
			swprintf(buf, L"%s.rect", szPath);
		char texts[1024];
		FILE* fT = _wfopen(buf, L"wb");
		if(!fT)
		{
			MessageBox(L"can't open dest file");
			break;
		}
		unsigned char head[3];
		head[0] = 0xef;
		head[1] = 0xbb;
		head[2] = 0xbf;
		fwrite(head, 3, 1, fT);
		char utf8buf[4096];
		sprintf(texts, "name\tl\tt\tr\tb\toffx\toffy\torigW\torigH\r\n");
		//int oldsize = rets.size();
		//tableBegin = oldsize;
		//int lentext = strlen(texts);
		//rets.SetArrayCount(oldsize + lentext);
		//memcpy(rets.GetPtr() + oldsize, texts, lentext);
		ToUtf8(utf8buf, texts);
		fwrite(utf8buf, strlen(utf8buf), 1, fT);
		for(int j=0;j<(*outputsPtr)[i]->rects.size();j++)
		{
			ImageRect& rc	=	(*outputsPtr)[i]->rects[j];
			FileName files  = toA(rc.fileName).buf;
			files.setIsFile();
			sprintf(texts, "%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\r\n", files.t, rc.rect.left, rc.rect.top, rc.rect.right - reserved, rc.rect.bottom - reserved, rc.offset.x, rc.offset.y, 
				rc.img->GetWidth(), rc.img->GetHeight());
			ToUtf8(utf8buf, texts);
			fwrite(utf8buf, strlen(utf8buf), 1, fT);
			//oldsize = rets.size();
			//lentext = strlen(texts);
			//rets.SetArrayCount(oldsize + lentext);
			//memcpy(rets.GetPtr() + oldsize, texts, lentext);
		}
		fclose(fT);
	}
	/*int len = sizeof(Phantom::EncryptVal);
	wchar_t buf[512];
	swprintf(buf, L"%s.locki", szPath);
	FILE* f = fopen(A(buf), "wb");
	int version = 1;
	char head[4] = {'l','o','c','k'};
	fwrite(head, sizeof(head), 1, f);
	fwrite(&version, sizeof(int), 1, f);
	fwrite(&tableBegin, sizeof(int), 1, f);
	AuthGUID guid;
	Phantom::EncryptVal v;
	GetAuthGUID(&guid);
	Phantom::GenuineGUID gen;
	GUIDToGenuine(gen, guid);
	Phantom::RandomVal rnd;
	Phantom::EncryptRandomVal(rnd, gen);
	fwrite(&rnd, sizeof(Phantom::RandomVal), 1, f);
	EncryptData(rets.GetPtr(), 128, rnd, v);
	fwrite(&v, sizeof(Phantom::EncryptVal), 1, f);
	EncryptData(rets.GetPtr() + tableBegin, rets.size()-tableBegin, rnd, v);
	fwrite(&v, sizeof(Phantom::EncryptVal), 1, f);
	fwrite(rets.GetPtr(), rets.size(), 1, f);
	fclose(f);*/
	//m_dlg.ShowWindow(SW_SHOW);
	for(int i=0;i<lines.size();i++)
		delete lines[i];
	for(int i=0;i<images.size();i++)
		delete images[i];
	for(int i=0;i<outputs.size();i++)
		delete outputs[i];
	for(int i=0;i<linesH.size();i++)
		delete linesH[i];
	for(int i=0;i<outputsH.size();i++)
		delete outputsH[i];
	MessageBox(L"convert ok!");
}

void CUIPngMaker::OnBnClickedOk()
{
}


void CUIPngMaker::OnBnClickedCheck1()
{
	m_lbAddFile.ResetContent();
	int nCount = m_cbDirectory.GetCurSel();
	for(int i=0;i<this->m_files.size();i++)
	{
		wchar_t buf[128], dir[258];
		_wsplitpath(m_files[i].c_str(), 0, dir, buf, 0);
		if(nCount > 0)
		{
			int nt = nCount;
			int len = wcslen(dir);
			for(int j=len-2;j>=0;j--)
			{
				if(dir[j] == '\\')
				{
					nt--;
					if(nt > 0)
						continue;
					wcscat(dir, buf);
					this->m_lbAddFile.AddString(dir + j + 1);
					break;
				}
			}
		}
		else
		{
			this->m_lbAddFile.AddString(buf);
		}
	}
}


void CUIPngMaker::OnBnClicked1005()
{
	OnBnClickedButton2();
	OnBnClickedCheck1();
}


void CUIPngMaker::OnBnClicked1004()
{
	OnBnClickedButton1();
}

void CUIPngMaker::OnBnClicked1008()
{
	m_loadFile = L"";
	OnBnClickedButton1();
}


void CUIPngMaker::OnBnClickedBuildAll2()
{
	CString strOutputName;
	this->m_edOutputName.GetWindowTextW(strOutputName);
	if(strOutputName.GetLength() <= 1)
	{
		MessageBox(L"写入输出文件名，和相对路径");
		return;
	}
	wchar_t szPath[512], szDir[512];
	wcscpy(szPath, W(GetProjectPath()).buf);
	wcscat(szPath, strOutputName);
	//
	std::vector<ImageFile*>		images;
	std::vector<OutpuImage*>	outputs;
	INT nCount	=	this->m_lbAddFile.GetCount();
	CString str;
	m_cbSize.GetWindowTextW(str);
	int imageSize	=	_wtoi(str);
	for(int i=0;i<nCount;i++)
	{
		CString strFile;
		this->m_lbAddFile.GetText(i, strFile);
		wchar_t szExt[128];
		_wsplitpath(strFile, 0, 0, 0, szExt);
		ImageFile* img = new ImageFile();
		img->offset.SetPoint(0, 0);
		BOOL bLoadOK	=	false;
		if(wcsicmp(szExt, L".png") == 0)
		{
			bLoadOK	=	SUCCEEDED(img->img.Load(strFile));
		}
		else if(wcsicmp(szExt, L".tga") == 0)
		{
			Phantom::StreamBase* ss	=	open_s(A(strFile), true);
			TextureTga tex;
			if(ss && LoadTGA(&tex, ss))
			{
				bLoadOK	=	true;
				tex.ConvertToImage(img->img);
			}
			else
				bLoadOK	=	false;
			if(ss)
				close_s(&ss);
		}
		if(!bLoadOK)//FAILED(img->img.Load(strFile)))
		{
			MessageBox(strFile, L"can't open png file");
			delete img;
		}
		else if(img->img.GetBPP() < 24)
		{
			MessageBox(strFile, L"only support 24 bit, and 32bit png file");
			delete img;
		}
		else
		{
			img->imgRect.SetRect(0, 0, img->img.GetWidth(), img->img.GetHeight());
			_wsplitpath(strFile, 0, 0, img->fileName, 0);
			img->Optimize(m_chkAlpha.GetCheck());
			img->offset.x	=	img->imgRect.left;
			img->offset.y	=	img->imgRect.top;
			int nWidth		=	img->imgRect.Width();
			int nHeight		=	img->imgRect.Height();
			int xCount		=	nWidth / imageSize;
			if((nWidth % imageSize) > 0)
				xCount++;
			int yCount		=	nHeight / imageSize;
			if((nHeight % imageSize) > 0)
				yCount++;
			//
			if(xCount >= 1 && yCount >= 1)
			{
				for(int x=0;x<xCount;x++)
				{
					for(int y=0;y<yCount;y++)
					{
						int imageWidth	=	(nWidth - x * imageSize);
						int imageHeight	=	(nHeight - y * imageSize);
						int nMM	=	imageSize;
						while(true)
						{
							if(imageWidth == nMM)
								break;
							if(imageWidth > nMM)
							{
								imageWidth = nMM * 2;
								break;
							}
							nMM	=	nMM / 2;
						}
						nMM	=	imageSize;
						while(true)
						{
							if(imageHeight == nMM)
								break;
							if(imageHeight > nMM)
							{
								imageHeight = nMM * 2;
								break;
							}
							nMM	=	nMM / 2;
						}
						imageWidth	=	min(imageWidth, imageSize);
						imageHeight	=	min(imageHeight, imageSize);
						CImage temp;
						temp.Create(imageWidth, imageHeight, 32, CImage::createAlphaChannel);
						//
						for(int xx=0;xx<imageWidth;xx++)
						{
							for(int yy=0;yy<imageHeight;yy++)
							{
								int dx	=	xx;
								int dy	=	yy;
								BYTE* dRGBA = (BYTE*)temp.GetPixelAddress(dx, dy);
								int sx	=	xx + x * imageSize;
								int sy	=	yy + y * imageSize;
								if(sx >= nWidth || sy >= nHeight)
								{
									dRGBA[0] = dRGBA[1] = dRGBA[2] = dRGBA[3] = 0;
									continue;
								}
								if(img->img.GetBPP() == 32)
								{
									BYTE* sRGBA = (BYTE*)img->img.GetPixelAddress(sx, sy);
									dRGBA[0] = sRGBA[0];
									dRGBA[1] = sRGBA[1];
									dRGBA[2] = sRGBA[2];
									dRGBA[3] = sRGBA[3];
								}
								else
								{
									BYTE* sRGB = (BYTE*)img->img.GetPixelAddress(sx, sy);
									dRGBA[0] = sRGB[0];
									dRGBA[1] = sRGB[1];
									dRGBA[2] = sRGB[2];
									dRGBA[3] = 255;
								}
							}
						}
						wchar_t buf[512];
						swprintf(buf, L"%s_%02dx%02d.png", szPath, x, y);
						temp.Save(buf);
					}
				}
			}
			delete img;
		}
	}
	MessageBox(L"convert ok!");
}


void CUIPngMaker::OnCbnSelchangeCombo1()
{
	this->OnBnClickedCheck1();
}


void CUIPngMaker::OnBnClickedBrowser2()
{
	if (afxShellManager == NULL)
	{
		CWinAppEx* pApp = DYNAMIC_DOWNCAST(CWinAppEx, AfxGetApp());
		if (pApp != NULL)
		{
			pApp->InitShellManager();
		}
	}
	BOOL bUpdate = false;
	CString strPath;
	if (afxShellManager == NULL)
		ASSERT(FALSE);
	else
	{
		bUpdate = afxShellManager->BrowseForFolder(strPath, this, W(GetProjectPath()), Language(L"选择要保存的项目工程目录"), BIF_RETURNONLYFSDIRS|BIF_USENEWUI );
	}
	if(bUpdate)
	{
		std::string sf[4];
		sf[0] = ".jpg";
		sf[1] = ".bmp";
		sf[2] = ".png";
		sf[3] = ".tga";
		//|*.png;*.jpg;*.bmp|TGA files|*.tga
		PMyFileList* files = ::enum_files(A(strPath), 1000, sf, 4);
		if(!files)
			return;
		for(int i=0;i<files->vFiles.size();i++)
		{
			BOOL bFind	=	false;
			W r(files->vFiles[i].szRealName);
			for(int j=0;j<m_files.size();j++)
			{
				if(wcsicmp(m_files[j].c_str(), r) == 0)
				{
					bFind	=	true;
					break;
				}
			}
			if(!bFind)
			{
				if(m_files.size() == 0)
				{
					wchar_t buf[256], dir[256];
					_wsplitpath(W(files->vFiles[i].szRealName), buf, dir, 0, 0);
					wcscat(buf, dir);
					wcscat(buf, L"img");
					if(m_edOutputName.GetWindowTextLengthW() == 0)
						this->m_edOutputName.SetWindowTextW(buf);
				}
				m_files.push_back(W(files->vFiles[i].szRealName).buf);
			}
		}
		destroyFileList(files);
		OnBnClickedCheck1();
	}
}


void CUIPngMaker::OnBnClickedButton100()
{
	CDynamicArray<Phantom::FileName> files;
	Phantom::NameT<16> ft = ".rpgui";
	enum_files("dlgs/", files, 5, &ft, 1);
	for(int i=0;i<files.size();i++){
		Phantom::Dialog dlg;
		if(Phantom::GetDlgManager()->LoadDialog(dlg, files[i].t)){
			for(int i=0;i<dlg->GetControlCount();i++){
				Phantom::UIControl* c = dlg->GetControlPtr(i);
				for(int j=0;j<c->GetElementCount();j++){
					Phantom::UIElement* e = c->GetElementPtr(j);
					e->SetTextureMode(e->GetTextureMode());
					for(int k=0;k<e->GetUVAnimationCount();k++){
						e->GetUVAnimationPtr(k)->RegetRects();
					}
				}
			}
			dlg->SaveDialog(files[i].t);
		}
	}
}
