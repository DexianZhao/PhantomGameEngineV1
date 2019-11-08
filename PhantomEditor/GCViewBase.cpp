/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
#include "StdAfx.h"
#include "GCViewBase.h"
#include "phantom-gcDoc.h"
#include "phantom-gcView.h"
#include "FileView.h"
#include "MainFrm.h"
#include "PropertiesWnd.h"
#include "PropertiesDialogWnd.h"
#include "ChildFrm.h"
#include <ShellAPI.h>

void		SetCurrentProperties(GCPropertyBase* base, const wchar_t* szPropertyName, CDialogEx* dlgProp)
{
	GetPropertiesWindow()->SetCurrentProperties(base, szPropertyName);
	GetPropertiesDlgWindowPtr()->SetCurrentDialog(dlgProp, szPropertyName);
	GetMainFrame()->m_wndClassView.SetChildWnd(0);
}

void		SetCurrentPropertiesW(CDialogEx* base, const wchar_t* szPropertyName, CDialogEx* dlgProp)
{
	GetPropertiesWindow()->SetCurrentWindow(base, szPropertyName);
	GetPropertiesDlgWindowPtr()->SetCurrentDialog(dlgProp, szPropertyName);
	GetMainFrame()->m_wndClassView.SetChildWnd(0);
}

GCViewBase::GCViewBase(CPhantomGCDoc* docPtr, const wchar_t* szEditorMode, INT nImageListID)
	:m_treeView(nImageListID)
{
	Phantom::CameraDataLocal data;
	Phantom::GetCameraPtr()->GetCameraConfig(data);
	Phantom::GetCameraPtr()->SetTarget(Phantom::float3(0, 0, 0));
	Phantom::GetCameraPtr()->SetEye(Phantom::float3(10.0f, 10.0f, 10.0f));
	Phantom::GetCameraPtr()->GetCameraConfig(m_savedCamera);
	Phantom::GetCameraPtr()->SetCameraConfig(data);
	//
	this->m_szEditorMode	=	szEditorMode;
	this->m_docPtr	=	docPtr;
	m_hViewItem		=	0;
	m_childFrame	=	0;
	m_bShowSelectOnly	=	false;
	m_bShowUIGrid	=	false;
	m_bIsReadOnly	=	false;
	m_currBuffer	=	0;
	m_currMode		=	UndoRedoBuffer::URMode_None;
	m_undoIndex		=	0;
}

GCViewBase::~GCViewBase(void)
{
	Phantom::g_manager.ReleaseAllTextureState();
//	safe_release(m_packagePtr);
	//
	BOOL bFind	=	FALSE;
	for(int i=0;i<m_properties.size();i++)
	{
		if(m_properties[i] == GetPropertiesWindow()->GetCurrentProperties())
		{
			bFind	=	true;
			break;
		}
	}
	for(int i=0;i<m_propertyWnds.size();i++)
	{
		if(m_propertyWnds[i] == GetPropertiesDlgWindowPtr()->m_dlgCurrent)
		{
			bFind	=	true;
			break;
		}
	}
	if(bFind)
	{
		GetPropertiesWindow()->SetCurrentProperties(0, Language(L"无属性"), TRUE);
		GetPropertiesDlgWindowPtr()->SetCurrentDialog(0, Language(L"无属性"), TRUE);
		GetMainFrame()->m_wndClassView.SetChildWnd(0);
	}
	for(int i=0;i<m_grids.size();i++)
	{
		try{
			delete m_grids[i];
		}
		catch(...)
		{
			assert(false);
		}
	}
}

BOOL	GCViewBase::IsCanHotkey()
{
	char className[256];
	className[0]	=	0;
	::GetClassNameA(GetFocus(), className, 256);
	strlwr(className);
	if(strstr(className, "edit"))
		return false;

	//HWND hWnd	=	GetFocus();
	//if(hWnd == GetDeviceWindow())
	//	return true;
	//if(hWnd == this->GetViewHandle())
	//	return true;
	//if(hWnd == this->GetTreeView()->m_hWnd)
	return true;
	//if(hWnd == this->m_propertyWnds
	//return false;
}

BOOL	GCViewBase::IsShowUIGrid()
{
	return m_bShowUIGrid;
}

VOID	GCViewBase::ShowUIGrid(BOOL b)
{
	m_bShowUIGrid	=	b;
}

VOID	GCViewBase::InitViewerProps()
{
	InitProp(m_viewerProps);
	CMFCPropertyGridProperty* pGroup1		= new CMFCPropertyGridProperty(Language(L"文件属性"));
	pGroup1->AddSubItem(m_viewerFile		= AddPropText(Language(L"文件名"), Language(L"文件名_说明", L"所选项目的文件路径")));
	pGroup1->AddSubItem(m_viewerBrowser		= AddPropButton(Language(L"打开所在目录"), Language(L"打开所在目录_说明", L"打开文件所在目录")));
	m_viewerFile->AllowEdit(FALSE);
	m_viewerProps.AddProperty(pGroup1);
	SetPropText(m_viewerFile, this->m_strFile.c_str());
}
GCViewBase*		GCViewBase::s_currView = 0;
void	GCViewBase::OnPropertyChanged(const GCPropertyBase* wnd, const CMFCPropertyGridProperty* pProp)
{
	s_currView = this;
	if(pProp == m_viewerBrowser)
	{
		wchar_t wbuf[512];
		swprintf(wbuf, L"/select,%s", m_strFile.c_str());
		ShellExecute(NULL, _T("open"), _T("Explorer.exe"), wbuf, NULL, SW_SHOWDEFAULT);
	}
}

VOID	GCViewBase::OnActive()		//被激活
{
	SetRenderParent(GetViewHandle());
	GetActiveView()->ChangeViewScroll();
	//
	GetFileViewPtr()->AdjustLayout();
	Phantom::GetCameraPtr()->SetCameraConfig(m_savedCamera);
	if(GetTreeView())
		GetTreeView()->ShowWindow(SW_SHOW);
	BuildFileViewText();
	UpdateMainText(m_szEditorMode.c_str(), GetFileViewName(), IsModifyed());
	OnChangeTreeSelect(&this->m_treeView, 0);
	//
	//GetMainFrame()->m_uiProperties.SetEditorMode(this->GetViewerType());
}

VOID	GCViewBase::SetActiveFrame(CChildFrame* f)
{
	m_childFrame = f;
	if(m_childFrame)
	{
		m_childFrame->SetIcon(GetViewerIcon());
		m_childFrame->SetToolTip(this->m_strFile.c_str());
	}
}

VOID	GCViewBase::UpdateMainTitle()
{
	BuildFileViewText();
	if(m_docPtr)
		m_docPtr->SetTitle(m_strFileView.c_str());
	UpdateMainText(m_szEditorMode.c_str(), GetFileViewName(), IsModifyed());
}

CChildFrame*		GCViewBase::GetActiveFrame()	//返回View指针
{
	return m_childFrame;
}

VOID	GCViewBase::BuildFileViewText()
{
	wchar_t szFile[128], szExt[128];
	_wsplitpath(this->m_strFile.c_str(), 0, 0, szFile, szExt);
	wcscat(szFile, szExt);
	if(!szFile[0])
		GetNewFileName(szFile);
	if(this->m_bIsReadOnly)
		wcscat(szFile, Language(L"[只读]"));
	else if(IsModifyed())
		wcscat(szFile, L"*");
	this->m_strFileView	=	szFile;
	if(m_childFrame)
	{
		m_childFrame->SendMessage(WM_SETTEXT, 0, (LPARAM)szFile);
	}
	if(this->m_docPtr)
		m_docPtr->SetTitle(m_strFileView.c_str());
	//CChildFrame* c	=	GetChildFrame(this->GetActiveView()->m_hWnd);
}

VOID	GCViewBase::OnDeActive()	//失去激活
{
	if(GetTreeView())
		GetTreeView()->ShowWindow(SW_HIDE);
	Phantom::GetCameraPtr()->GetCameraConfig(m_savedCamera);
	SetRenderParent(NULL);
}

HWND	GCViewBase::GetViewHandle()
{
	POSITION pos	=	m_docPtr->GetFirstViewPosition();
	return m_docPtr->GetNextView(pos)->m_hWnd;
}

CPhantomGCView*		GCViewBase::GetActiveView()	//返回View指针
{
	POSITION pos	=	m_docPtr->GetFirstViewPosition();
	return static_cast<CPhantomGCView*>(m_docPtr->GetNextView(pos));
}

TreeItemType*		GCViewBase::GetSelectedItem()
{
	std::map<int,int> selects;
	std::map<int,TreeItemType*> selectTypes;
	for(int i=0;i<m_treeView.GetSelectCount();i++)
	{
		HTREEITEM hItem		=	m_treeView.GetSelect(i);
		TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
		if(type)
		{
			selectTypes[type->type]	=	type;
			selects[type->type]++;
		}
	}
	int nMax = 0;
	int selType	=	0;
	for(std::map<int, int>::iterator it = selects.begin();it != selects.end(); it++)
	{
		if(it->second > nMax)
		{
			nMax	=	it->second;
			selType	=	it->first;
		}
	}
	if(selType > 0)
		return selectTypes[selType];
	return 0;
}

VOID				GCViewBase::CloseFile()
{
}
BOOL				GCViewBase::OpenFile(const wchar_t* szFile)	//打开文件,如果失败则会自动删除
{
	SetProjectPath();
	if(szFile)
	{
		wchar_t drive[512], dir[512],szExt[256];
		_wsplitpath(szFile, drive, dir, 0, szExt);
		wcscat(drive, dir);
		//
		//int len = wcslen(drive);
		//for(int i=len - 1;i>0;i--)
		//{
		//	if(drive[i] == L'/' || drive[i] == '\\')
		//	{
		//		drive[i + 1] = 0;
		//		wcscpy(dir, drive);
		//		wchar_t szIni[256];
		//		wcscpy(szIni, dir);
		//		wcscat(szIni, L"PhantomV1.ini");
		//		if(fexist(0, A(szIni)))
		//		{
		//			wchar_t screenW[128],screenH[128];
		//			::GetPrivateProfileStringW(L"game", L"screenW", L"", screenW, 128, szIni);
		//			::GetPrivateProfileStringW(L"game", L"screenH", L"", screenH, 128, szIni);
		//			int w = _wtoi(screenW);
		//			int h = _wtoi(screenH);
		//			if(w > 0 && h > 0)
		//				SetRenderWindowSize(w, h);
		//			SetProjectPath(A(drive));
		//			break;
		//		}
		//	}
		//}
		//safe_release(m_packagePtr);
		_wsplitpath(szFile, 0, 0, 0, szExt);
		//if(wcsnicmp(szExt, L".z_", 3) == 0)
		//{
		//	m_bIsReadOnly	=	true;
		//	PackageInfo info;
		//	ImportResult result = ImportResult_OK;
		//	if(!(m_packagePtr = LoadFilePackage(0, A(szFile), info, 0, &result)))
		//	{
		//		if(result == ImportResult_FailureAuth)
		//		{
		//			//while(true)
		//			{
		//				//CFileOpenAuthDlg dlg(GetMainFrame());
		//				//dlg.m_fileInfo	=	info;
		//				//dlg.m_strFile	=	szFile;
		//				//if(dlg.DoModal() != IDOK)
		//				//	return false;
		//				//A a(dlg.m_strAuthGUID);
		//				//GUID guid;
		//				//std::vector<unsigned char> ret;
		//				//HexEncode(ret, a.buf);
		//				//BOOL bAuth	=	false;
		//				//if(ret.size() >= sizeof(GUID))
		//				//{
		//					//memcpy(&guid, &ret.at(0), sizeof(GUID));
		//					if((m_packagePtr = LoadFilePackage(0, A(szFile), info, 0, &result)))
		//					{
		//						//break;
		//					}
		//					//else
		//						//GetMainFrame()->MessageBox(Language(L"你目前还没有获得这个资源的授权,无法使用\n这个资源必须得获得授权才能使用!"), Language(L"提示"), MB_OK);
		//				//}
		//				
		//			}
		//		}
		//		else
		//			return false;
		//	}
		//}
		//else
			m_bIsReadOnly	=	false;
	}
	else
		m_bIsReadOnly	=	true;
	if(GetTreeView() && !GetTreeView()->m_hWnd)
	{
		CRect rectDummy;
		rectDummy.SetRectEmpty();
		// 创建视图:
		const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;
		//
		if(!GetTreeView()->Create(dwViewStyle, rectDummy, GetFileViewPtr(), 0))
			return false;
	}
	BuildFileViewText();
	UpdateMainText(m_szEditorMode.c_str(), GetFileViewName(), IsModifyed());
	return true;
}

BOOL				GCViewBase::SaveFile(BOOL bSaveAs)	//保存文件
{
	SetModifyed(false);
	BuildFileViewText();
	if(m_docPtr)
		m_docPtr->SetTitle(m_strFileView.c_str());
	UpdateMainText(m_szEditorMode.c_str(), GetFileViewName(), IsModifyed());
	return true;
}

VOID				GCViewBase::SetModifyed(BOOL bModifyed)	//设置修改状态
{
	if((this->m_docPtr && IsModifyed() != bModifyed) && !this->m_bIsReadOnly)// || (bModifyed && this->m_strFileView.size() > 0 && m_strFileView.at(m_strFileView.size() - 1) != L'*'))
	{
		m_docPtr->SetModifiedFlag(bModifyed);
		BuildFileViewText();
		if(m_docPtr)
			m_docPtr->SetTitle(m_strFileView.c_str());
		UpdateMainText(m_szEditorMode.c_str(), GetFileViewName(), IsModifyed());
	}
}

BOOL				GCViewBase::IsModifyed()
{
	if(m_docPtr)
		return m_docPtr->IsModified();
	return false;
}

const wchar_t*		GCViewBase::GetFileViewName()
{
	return this->m_strFileView.c_str();
}

int					GCViewBase::AddTreeItem(int nType, int p1, int p2, int p3, int p4, int p5, const wchar_t* szText)
{
	for(int i=0;i<m_treeItems.size();i++)
	{
		TreeItemType& type = m_treeItems[i];
		if(type.type == nType && type.p1 == p1 && type.p2 == p2 && type.p3 == p3 && type.p4 == p4 && type.p5 == p5)
		{
			if(szText)
				if(wcsicmp(szText, type.path) != 0)
					continue;
			return type.id;
		}
	}
	TreeItemType v;
	v.type	=	nType;
	v.id	=	66666688 + m_treeItems.size();
	v.p1	=	p1;
	v.p2	=	p2;
	v.p3	=	p3;
	v.p4	=	p4;
	v.p5	=	p5;
	if(szText)
		wcscpy(v.path, szText);
	m_treeItems.push_back(v);
	return v.id;
}

HTREEITEM			GCViewBase::FindTreeItem(HTREEITEM hParent, int nType, int p1, int p2, int p3, int p4, int p5)
{
	if(!this->GetTreeView())
		return 0;
	HTREEITEM hChild	=	this->GetTreeView()->GetChildItem(hParent);
	while(hChild)
	{
		TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hChild));
		if(type)
		{
			if(type->type == nType && type->p1 == p1 && type->p2 == p2 && type->p3 == p3 && type->p4 == p4 && type->p5 == p5)
				return hChild;
		}
		HTREEITEM hRet	=	FindTreeItem(hChild, nType, p1, p2, p3, p4, p5);
		if(hRet)
			return hRet;
		hChild	=	GetTreeView()->GetNextSiblingItem(hChild);
	}
	return 0;
}

VOID				GCViewBase::ClearTreeItem()
{
	m_treeItems.clear();
}

TreeItemType*		GCViewBase::GetTreeItem(int id)
{
	for(int i=0;i<m_treeItems.size();i++)
	{
		if(m_treeItems[i].id == id)
			return &m_treeItems[i];
	}
	return 0;
}

VOID	EnableRedraw(CMFCPropertyGridProperty* p, BOOL b)
{
	//static CMFCPropertyGridCtrl* wnd	= NULL;
	//if(b)
	//{
	//	assert(wnd != NULL);
	//	((CMyPropertyGridProperty*)p)->SetWndList(wnd);
	//	wnd	=	0;
	//}
	//else
	//{
	//	assert(wnd == NULL);
	//	wnd	=	((CMyPropertyGridProperty*)p)->GetWndList();
	//	((CMyPropertyGridProperty*)p)->SetWndList(NULL);
	//}
}

VOID				GCViewBase::SetPropInt(PropertyPtr p, int v)
{
	long l = v;
	EnableRedraw(p, FALSE);
	p->SetValue(_variant_t(l));
	EnableRedraw(p, TRUE);
}
VOID				GCViewBase::SetPropFloat(PropertyPtr p, float v)
{
	int t	=	m_floatMap[p];
	if(t == FloatType_Meter)
		v	=	ToMeter(v);
	else if(t == FloatType_Angle)
		v	=	Phantom::Degress(v);
	EnableRedraw(p, FALSE);
	p->SetValue(_variant_t(v));
	EnableRedraw(p, TRUE);
}
VOID				GCViewBase::SetPropColor(PropertyColorPtr p, COLORREF v)
{
	EnableRedraw(p, FALSE);
	p->SetColor(v);
	EnableRedraw(p, TRUE);
}
//VOID				GCViewBase::SetPropColor(PropertyColorPtr p, const Phantom::color4& c)
//{
//	EnableRedraw(p, false);
//	p->SetColor(c.getRGB());
//	EnableRedraw(p, TRUE);
//}
VOID				GCViewBase::SetPropColor(PropertyColorPtr p, const color4& c)
{
	EnableRedraw(p, false);
	p->SetColor(c.getRGB());
	EnableRedraw(p, TRUE);
}
VOID				GCViewBase::SetPropBool(PropertyPtr p, BOOL b)
{
	bool _b = (b == TRUE);
	EnableRedraw(p, false);
	p->SetValue(_variant_t(_b));
	EnableRedraw(p, TRUE);
}
VOID				GCViewBase::SetPropText(PropertyPtr p, const wchar_t* szText)
{
	EnableRedraw(p, false);
	p->SetValue(_variant_t(szText));
	EnableRedraw(p, TRUE);
}

VOID				GCViewBase::SetPropOption(PropertyPtr p, CTypeOption& opt, INT nIndex)
{
	EnableRedraw(p, false);
	p->SetValue(_variant_t(opt[nIndex]));
	EnableRedraw(p, TRUE);
}

VOID				GCViewBase::SetPropText(PropertyPtr p, const char* szText)
{
	EnableRedraw(p, false);
	p->SetValue(_variant_t(W(szText)));
	EnableRedraw(p, TRUE);
}

VOID				GCViewBase::SetPropFile(PropertyFilePtr p, const wchar_t* szText)
{
	EnableRedraw(p, false);
	p->SetValue(_variant_t(szText));
	if(szText[1] == L':')
		p->SetInitDir(szText);
	else
		p->SetInitDir(m_strFile.c_str());
	EnableRedraw(p, TRUE);
}

VOID				GCViewBase::SetPropFile(PropertyFilePtr p, const char* szText)
{
	EnableRedraw(p, false);
	p->SetValue(_variant_t(W(szText)));
	if(szText[1] == ':')
		p->SetInitDir(W(szText));
	else
		p->SetInitDir(m_strFile.c_str());
	EnableRedraw(p, TRUE);
}

VOID				GCViewBase::SetPropList(PropertyListPtr p, std::vector<std::wstring>& lists)
{
	EnableRedraw(p, false);
	p->SetList(lists);
	EnableRedraw(p, true);
}

VOID				GCViewBase::SetPropList(PropertySelListPtr p, std::vector<int>& lists)
{
	EnableRedraw(p, false);
	p->SetSelect(lists);
	EnableRedraw(p, true);
}
VOID				GCViewBase::GetPropList(PropertySelListPtr p, std::vector<int>& lists)
{
	lists	=	p->m_lists;
}

INT					GCViewBase::GetPropInt(PropertyPtr p)
{
	p->SetValue(p->GetValue());
	_variant_t v(p->GetValue());
	assert(v.vt == VT_I4);
	return v.lVal;
}

float				GCViewBase::GetPropFloat(PropertyPtr p)
{
	p->SetValue(p->GetValue());
	_variant_t v(p->GetValue());
	assert(v.vt == VT_R4);
	int t	=	m_floatMap[p];
	if(t == FloatType_Meter)
		return To3DValue(v.operator float());
	else if(t == FloatType_Angle)
		return Phantom::Radians(v.operator float());
	return v.operator float();
}

float*				GCViewBase::GetPropFloatPtr(PropertyPtr p)
{
	static float f;
	f = GetPropFloat(p);
	return &f;
}

BOOL				GCViewBase::GetPropBool(PropertyPtr p)
{
	_variant_t v(p->GetValue());
	assert(v.vt == VT_BOOL);
	return (v.operator bool());
}

VOID				GCViewBase::GetPropText(PropertyPtr p, std::wstring& ret)
{
	_bstr_t v(p->GetValue());
	const wchar_t* str = v;
	ret	=	str;
}

BOOL				GCViewBase::GetSortFile(std::wstring& ret, const wchar_t* szFileIn, BOOL bInFilePath)
{
	std::wstring strCopy	=	szFileIn;
	const wchar_t* str		=	strCopy.c_str();
	if(str[1] != L':')
	{
		ret		=	str;
		return false;
	}
	//和目标目录比较
	wchar_t geomPath[256], geomDir[256];
	wchar_t newPath[256], newDir[256];
	wchar_t szFile[256], szExt[256];
	wchar_t destPath[256];
	_wsplitpath(this->m_strFile.c_str(), geomPath, geomDir, 0, 0);
	_wsplitpath(str, newPath, newDir, szFile, szExt);
	wcscat(newPath, newDir);
	wcscat(geomPath, geomDir);
	wcscpy(destPath, geomPath);
	wcscat(szFile, szExt);
	wcslwr(newPath);
	//INT nGeomPath	=	wcslen(geomPath);
	//if(nGeomPath > 0 && wcsnicmp(geomPath, newPath, nGeomPath) == 0)
	//{
	//	ret	=	str + nGeomPath;
	//	return true;
	//}
	//和程序目录进行比较
	int temp = wcslen(newPath);
	for(int i=0;i<temp;i++){
		if(newPath[i]==L'\\')
			newPath[i]=L'/';
	}
	if(bInFilePath)
	{
		_wsplitpath(m_strFile.c_str(), geomPath, geomDir, 0, 0);
		wcscat(geomPath, geomDir);
		wcslwr(geomPath);
		temp = wcslen(geomPath);
		for(int i=0;i<temp;i++){
			if(geomPath[i]==L'\\')
				geomPath[i]=L'/';
		}

		INT nGeomPath	=	wcslen(geomPath);
		if(wcsnicmp(geomPath, newPath, nGeomPath) == 0)
		{
			ret	=	str + nGeomPath;
			if(!wcsstr(ret.c_str(), L"\\"))
				return true;
		}
	}
	{
		_wsplitpath(W(GetProjectPath()), geomPath, geomDir, 0, 0);
		wcscat(geomPath, geomDir);
		INT nGeomPath	=	wcslen(geomPath);
		temp = wcslen(geomPath);
		for(int i=0;i<temp;i++){
			if(geomPath[i]==L'\\')
				geomPath[i]=L'/';
		}
		if(wcsnicmp(geomPath, newPath, nGeomPath) == 0)
		{
			ret	=	str + nGeomPath;
			return true;
		}
		ret	=	str;
		if(MessageBox(this->GetActiveView()->m_hWnd, Language(L"所有选择的路径，都必须要在工程目录内，否则将无法找到路径。\n是否拷贝到本程序内？"), Language(L"提示!"), MB_YESNO) == IDYES)
		{
			wcscat(destPath, szFile);
			FILE* f = fopen(A(destPath), "rb");
			if(!f)
				CopyFile(str, destPath, FALSE);
			if(f)fclose(f);
			ret	=	szFile;
		}
	}
	return false;
}

VOID				GCViewBase::GetPropFile(PropertyFilePtr p, std::wstring& ret, BOOL bInFilePath)
{
	_bstr_t v(p->GetValue());
	if(GetSortFile(ret, v, bInFilePath))
		SetPropText(p, ret.c_str());
}

VOID				GCViewBase::GetPropText(PropertyPtr p, std::string& ret)
{
	_bstr_t v(p->GetValue());
	ret	=	(const char*)v;
}

COLORREF			GCViewBase::GetPropColor(PropertyColorPtr p)
{
	return p->GetColor();
}

INT					GCViewBase::GetPropOption(PropertyPtr p, CTypeOption& opt)
{
	_bstr_t s(p->GetValue());
	const wchar_t* w = s;
	return opt[w];
}

VOID				GCViewBase::AddOptions(PropertyPtr p, std::vector<std::wstring>& texts)
{
	for(int i=0;i<texts.size();i++)
	{
		p->AddOption(texts[i].c_str());
	}
}

PropertyPtr			GCViewBase::AddPropOption(const wchar_t* szName, const wchar_t* szDesc, CTypeOption& opt)
{
	const wchar_t* l = L"";
	PropertyPtr p = new CMFCPropertyGridProperty(szName, (_variant_t)l, szDesc);
	for(int i=0;i<opt.m_options.size();i++)
	{
		p->AddOption(opt.m_options[i].text);
	}
	p->AllowEdit(FALSE);
	return p;
}

//
PropertyPtr			GCViewBase::AddPropInt(const wchar_t* szName, const wchar_t* szDesc, INT nMin, INT nMax, INT nPage)
{
	CMFCPropertyGridIntProperty* ip	=	new CMFCPropertyGridIntProperty(szName, nMin, nMax, nPage, szDesc);
	m_grids.push_back(ip);
	m_ints.push_back(ip);
	return ip;
}
PropertyFloatPtr	GCViewBase::AddPropFloat(const wchar_t* szName, const wchar_t* szDesc, int nFloatType, float fMin, float fMax, float fPage)
{
	PropertyFloatPtr	p	=	new CMFCPropertyGridFloatProperty(szName, fMin, fMax, fPage, szDesc);
	m_grids.push_back(p);
	m_floatMap[p] = nFloatType;
	m_floats.push_back(p);
	return p;
}
PropertyPtr			GCViewBase::AddPropBool(const wchar_t* szName, const wchar_t* szDesc)
{
	bool b = true;
	PropertyPtr ip	=	new CMFCPropertyGridProperty(szName, (_variant_t)b, szDesc);
	m_grids.push_back(ip);
	return ip;
}

PropertyFontPtr		GCViewBase::AddPropFont(const wchar_t* szName, const wchar_t* szDesc)
{
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));
	PropertyFontPtr ip = new CMFCPropertyGridFontProperty(szName, lf, CF_SCREENFONTS, szDesc);
	m_grids.push_back(ip);
	return ip;
}

VOID				GCViewBase::GetPropFont(PropertyFontPtr p, Phantom::FontNode2D& ret)
{
	ret.fontName = Autf8(p->GetLogFont()->lfFaceName).buf;
	ret.bUnderline	=	p->GetLogFont()->lfUnderline;
	ret.bItalic		=	p->GetLogFont()->lfItalic;
	ret.nFontHeight	=	p->GetLogFont()->lfHeight;
	ret.nWeight		=	p->GetLogFont()->lfWeight;
}
VOID				GCViewBase::SetPropFont(PropertyFontPtr p, const Phantom::FontNode2D* node)
{
	if(!node)
	{
		p->GetLogFont()->lfFaceName[0]	=	0;
		p->GetLogFont()->lfUnderline	=	0;
		p->GetLogFont()->lfItalic		=	0;
		p->GetLogFont()->lfHeight		=	0;
		p->GetLogFont()->lfWeight		=	0;
		return;
	}
	wcscpy(p->GetLogFont()->lfFaceName, Wutf8(node->fontName.t).buf);
	p->GetLogFont()->lfUnderline	=	node->bUnderline;
	p->GetLogFont()->lfItalic		=	node->bItalic;
	p->GetLogFont()->lfHeight		=	node->nFontHeight;
	p->GetLogFont()->lfWeight		=	node->nWeight;
}
//
//VOID				GCViewBase::GetPropFont(PropertyFontPtr p, FontNode2D& ret)
//{
//	wcscpy(ret.strFace, p->GetLogFont()->lfFaceName);
//	ret.bUnderline	=	p->GetLogFont()->lfUnderline;
//	ret.bItalic		=	p->GetLogFont()->lfItalic;
//	ret.nFontHeight	=	p->GetLogFont()->lfHeight;
//	ret.nWeight		=	p->GetLogFont()->lfWeight;
//}
//VOID				GCViewBase::SetPropFont(PropertyFontPtr p, const FontNode2D* node)
//{
//	if(!node)
//	{
//		p->GetLogFont()->lfFaceName[0]	=	0;
//		p->GetLogFont()->lfUnderline	=	0;
//		p->GetLogFont()->lfItalic		=	0;
//		p->GetLogFont()->lfHeight		=	0;
//		p->GetLogFont()->lfWeight		=	0;
//		return;
//	}
//	wcscpy(p->GetLogFont()->lfFaceName, node->strFace);
//	p->GetLogFont()->lfUnderline	=	node->bUnderline;
//	p->GetLogFont()->lfItalic		=	node->bItalic;
//	p->GetLogFont()->lfHeight		=	node->nFontHeight;
//	p->GetLogFont()->lfWeight		=	node->nWeight;
//}

VOID				GCViewBase::SetPropFont(PropertyFontPtr p, INT nFontID)
{
	return SetPropFont(p, Phantom::GetDlgManager()->GetFontNode(nFontID));//GetDlgResManager()->GetFontPtr(nFontID));
}

VOID				GCViewBase::GetPropList(PropertyListPtr p, std::vector<std::wstring>& lists)
{
	lists	=	p->m_lists;
}
PropertyListPtr		GCViewBase::AddPropList(const wchar_t* szName, const wchar_t* szDesc)
{
	PropertyListPtr ip = new MFCPropertyGridListProperty(szName, szDesc);
	m_grids.push_back(ip);
	return ip;
}
PropertySelListPtr	GCViewBase::AddPropSelList(const wchar_t* szName, const wchar_t* szDesc)
{
	PropertySelListPtr ip = new MFCPropertyGridListSelProperty(szName, szDesc);
	m_grids.push_back(ip);
	return ip;
}

PropertyPtr			GCViewBase::AddPropButton(const wchar_t* szName, const wchar_t* szDesc)
{
	const wchar_t* l = Language(L"...");
	PropertyPtr p	= new CMFCPropertyGridButtonProperty(szName, (_variant_t)l, szDesc);
	p->AllowEdit(FALSE);
	m_grids.push_back(p);
	return p;
}

PropertyPtr			GCViewBase::AddPropText(const wchar_t* szName, const wchar_t* szDesc, BOOL bIsMultilineText, INT nMaxTextLength)
{
	const wchar_t* l = L"";
	PropertyPtr ip = 0;
	if(bIsMultilineText)
		ip = new CMFCPropertyGridBigTextProperty(szName, (_variant_t)l, szDesc, nMaxTextLength);
	else
		ip = new CMFCPropertyGridTextProperty(szName, (_variant_t)l, szDesc, nMaxTextLength);
	m_grids.push_back(ip);
	return ip;
}
PropertyColorPtr			GCViewBase::AddPropColor(const wchar_t* szName, const wchar_t* szDesc)
{
	COLORREF clr = 0;
	PropertyColorPtr ret	=	new CMFCPropertyGridColorProperty(szName, clr, 0, szDesc);
	ret->EnableOtherButton(Language(L"更多..."));
	ret->SetColumnsNumber(6);
	m_grids.push_back(ret);
	return ret;
}

PropertyFilePtr			GCViewBase::AddPropFile(const wchar_t* szName, const wchar_t* szDesc, const wchar_t* szFilter, BOOL bMultiSelect)
{
	PropertyFilePtr p = new CMFCPropertyGridMyFileProperty(szName, TRUE, L"", 0, bMultiSelect ? (OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT) : (OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT), szFilter, szDesc);
	p->AllowEdit(false);
	m_grids.push_back(p);
	return p;
}

VOID				GCViewBase::SelectTreeByType(INT type, HTREEITEM hItem)	//根据类型多选
{
	if(this->GetTreeView())
	{
		GetTreeView()->SelectItem(NULL);
		GetTreeView()->ClearSelect();
		if(OnSelectTreeByType(type, hItem))
			GetTreeView()->Expand(hItem, TVE_EXPAND);
	}
}

CMFCPropertyGridFloatProperty*	GCViewBase::FindFloatPtr(CMFCPropertyGridProperty* g)
{
	for(int i=0;i<m_floats.size();i++)
		if(m_floats[i] == g)
			return m_floats[i];
	return 0;
}
CMFCPropertyGridIntProperty*	GCViewBase::FindIntPtr(CMFCPropertyGridProperty* g)
{
	for(int i=0;i<m_ints.size();i++)
		if(m_ints[i] == g)
			return m_ints[i];
	return 0;
}

VOID	GCViewBase::InitProp(GCPropertyBase& p)
{
	CRect rectDummy;
	rectDummy.SetRectEmpty();
	if (!p.Create(WS_VISIBLE | WS_CHILD, rectDummy, GetPropertiesWindow(), 1))
	{
		return;      // 未能创建
	}
	p.EnableHeaderCtrl(FALSE);
	p.EnableDescriptionArea();
	p.SetVSDotNetLook();
	//p.MarkModifiedProperties();
	p.m_callback	=	this;
	p.m_valueEditor.Create(p.m_valueEditor.IDD, &p);
	m_properties.push_back(&p);
}

VOID				GCViewBase::AddPropertieWindow(CWnd* wnd)
{
	m_properties.push_back(wnd);
}

BOOL				GCViewBase::OnSelectTreeByType(INT nType, HTREEITEM hItem)	//根据类型多选
{
	BOOL bExpand		=	false;
	HTREEITEM hChild	=	GetTreeView()->GetNextItem(hItem, TVGN_CHILD);
	while(hChild)
	{
		TreeItemType* type	=	GetTreeItem(GetTreeView()->GetItemData(hChild));
		if(type && type->type == nType)
		{
			if(!bExpand)
				GetTreeView()->Expand(hItem, TVE_EXPAND);
			bExpand	=	true;
			GetTreeView()->AddSelect(hChild);
			if(GetTreeView()->GetSelectedItem() == NULL)
				GetTreeView()->SelectItem(hChild);
		}
		if(OnSelectTreeByType(nType, hChild))
		{
			if(!bExpand)
				GetTreeView()->Expand(hItem, TVE_EXPAND);
			bExpand	=	true;
		}
		hChild	=	GetTreeView()->GetNextItem(hChild, TVGN_NEXT);
	}
	return bExpand;
}

VOID				GCViewBase::DeleteChildItems(HTREEITEM hItem)	//根据类型多选
{
	HTREEITEM hChild	=	GetTreeView()->GetNextItem(hItem, TVGN_CHILD);
	while(hChild)
	{
		HTREEITEM hNext	=	GetTreeView()->GetNextItem(hChild, TVGN_NEXT);
		GetTreeView()->RemoveSelect(hChild);
		GetTreeView()->DeleteItem(hChild);
		hChild	=	hNext;
	}
}

VOID				GCViewBase::InitPropWnd(CDialogEx* propWnd, UINT nDialogID)
{
	propWnd->Create(nDialogID, GetPropertiesDlgWindowPtr());
	m_propertyWnds.push_back(propWnd->m_hWnd);
}
//
UndoRedoBuffer*		GCViewBase::AddUndoRedo(int type, ProcUndoRedo proc)
{
	for(int i=0;i<m_redos.size();i++){
		delete m_redos[i];
	}
	m_redos.clear();
	UndoRedoBuffer* undo = new UndoRedoBuffer();
	undo->type		=	type;
	undo->proc		=	proc;
	undo->m_undoIndex	=	m_undoIndex;
	m_undos.push_back(undo);
	m_currBuffer	=	undo;
	m_currMode		=	UndoRedoBuffer::URMode_Add;
	return undo;
}
BOOL		GCViewBase::IsCanUndo()
{
	return (m_undos.size()>0);
}
BOOL		GCViewBase::IsCanRedo()
{
	return (m_redos.size()>0);
}
VOID		GCViewBase::OnUndo()
{
	if(m_undos.size()==0)return;
	UndoRedoBuffer* u = m_undos[m_undos.size()-1];
	int index = u->m_undoIndex;
	while(u&&u->m_undoIndex==index){
		m_redos.push_back(u);
		m_undos.pop_back();
		u->reset();
		if(u->m_bEnabled)
			OnUndoRedo(u, UndoRedoBuffer::URMode_Undo);
		if(m_undos.size()>0)
			u = m_undos[m_undos.size()-1];
		else
			u = 0;
	}
	OnChangeTreeSelect(&this->m_treeView, 0);
}

VOID		GCViewBase::OnRedo()
{
	if(m_redos.size()==0)return;
	UndoRedoBuffer* u = m_redos[m_redos.size()-1];
	int index = u->m_undoIndex;
	while(u&&u->m_undoIndex==index){
		m_undos.push_back(u);
		m_redos.pop_back();
		u->reset();
		if(u->m_bEnabled)
			OnUndoRedo(u, UndoRedoBuffer::URMode_Redo);
		if(m_redos.size()>0)
			u = m_redos[m_redos.size()-1];
		else
			u = 0;
	}
	OnChangeTreeSelect(&this->m_treeView, 0);
}
VOID		GCViewBase::OnUndoRedo(UndoRedoBuffer* buffer, int ur_mode)
{
}
