/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
#include "StdAfx.h"
#include "resource.h"
#include "MyMFCPropertyGridProperty.h"
#include "BigTextEditor.h"
#include "ListBoxEditor.h"
#include "ChangeSpinButtonCtrl.h"
#include "assert.h"

IMPLEMENT_DYNAMIC(CMFCPropertyGridBigTextProperty, CMFCPropertyGridProperty)

CMFCPropertyGridBigTextProperty::CMFCPropertyGridBigTextProperty(const CString& strName, const CString& strBigText, LPCTSTR lpszDescr, INT nMaxTextLength)
 : CMFCPropertyGridProperty(strName, COleVariant((LPCTSTR)strBigText), lpszDescr, 0)
{
	m_dwFlags = 0x0002;
	m_nMaxTextLength	=	nMaxTextLength;
}


CMFCPropertyGridBigTextProperty::~CMFCPropertyGridBigTextProperty(void)
{
}


void CMFCPropertyGridBigTextProperty::SetValue(const COleVariant& varValue)
{
	_bstr_t v(varValue);
	wchar_t* wcs	=	new wchar_t[m_nMaxTextLength];
	wcscpy(wcs, v);
	wcs[m_nMaxTextLength - 1] = 0;
	__super::SetValue(_variant_t(wcs));
}

void CMFCPropertyGridBigTextProperty::OnClickButton(CPoint point)
{
	ASSERT_VALID(this);
	ASSERT_VALID(m_pWndList);
	ASSERT_VALID(m_pWndInPlace);
	ASSERT(::IsWindow(m_pWndInPlace->GetSafeHwnd()));

	m_bButtonIsDown = TRUE;
	Redraw();
	//
	CBigTextEditor edit(m_pWndList);
	edit.m_editText	=	m_varValue.bstrVal;
	if(edit.DoModal() == IDOK)
	{
		wchar_t* wcs	=	new wchar_t[m_nMaxTextLength];
		wcscpy(wcs, edit.m_editText);
		wcs[m_nMaxTextLength - 1] = 0;
		m_varValue	=	wcs;
		delete[] wcs;
		m_pWndList->OnPropertyChanged(this);
	}
	//
	m_bButtonIsDown = FALSE;
	Redraw();
	if (m_pWndInPlace != NULL)
	{
		m_pWndInPlace->SetFocus();
	}
	else
	{
		m_pWndList->SetFocus();
	}
}


IMPLEMENT_DYNAMIC(CMFCPropertyGridTextProperty, CMFCPropertyGridProperty)

CMFCPropertyGridTextProperty::CMFCPropertyGridTextProperty(const CString& strName, const CString& strBigText, LPCTSTR lpszDescr, INT nMaxTextLength)
 : CMFCPropertyGridProperty(strName, COleVariant((LPCTSTR)strBigText), lpszDescr, 0)
{
	m_dwFlags = 0;
	m_nMaxTextLength	=	nMaxTextLength;
}


CMFCPropertyGridTextProperty::~CMFCPropertyGridTextProperty(void)
{
}


void CMFCPropertyGridTextProperty::SetValue(const COleVariant& varValue)
{
	_bstr_t v(varValue);
	wchar_t* wcs	=	new wchar_t[m_nMaxTextLength];
	wcscpy(wcs, v);
	wcs[m_nMaxTextLength - 1] = 0;
	__super::SetValue(_variant_t(wcs));
}

IMPLEMENT_DYNAMIC(MFCPropertyGridListProperty, CMFCPropertyGridProperty)

MFCPropertyGridListProperty::MFCPropertyGridListProperty(const CString& strName, LPCTSTR lpszDescr)
 : CMFCPropertyGridProperty(strName, COleVariant((LPCTSTR)L""), lpszDescr, 0)
{
	m_dwFlags = 0x0002;
}


MFCPropertyGridListProperty::~MFCPropertyGridListProperty(void)
{
}

VOID MFCPropertyGridListProperty::SetList(std::vector<std::wstring>& list)
{
	m_lists		=	list;
	std::wstring str	=	L"";
	for(int i=0;i<m_lists.size();i++)
	{
		str	+=	m_lists[i].c_str();
	}
	m_varValue	=	str.c_str();
}

void MFCPropertyGridListProperty::OnClickButton(CPoint point)
{
	ASSERT_VALID(this);
	ASSERT_VALID(m_pWndList);
	ASSERT_VALID(m_pWndInPlace);
	ASSERT(::IsWindow(m_pWndInPlace->GetSafeHwnd()));

	m_bButtonIsDown = TRUE;
	Redraw();
	//
	CListBoxEditor edit(m_pWndList);
	edit.m_lists	=	this->m_lists;
	if(edit.DoModal() == IDOK)
	{
		SetList(edit.m_lists);
		m_pWndList->OnPropertyChanged(this);
	}
	//
	m_bButtonIsDown = FALSE;
	Redraw();
	if (m_pWndInPlace != NULL)
	{
		m_pWndInPlace->SetFocus();
	}
	else
	{
		m_pWndList->SetFocus();
	}
}

IMPLEMENT_DYNAMIC(MFCPropertyGridListSelProperty, CMFCPropertyGridProperty)

MFCPropertyGridListSelProperty::MFCPropertyGridListSelProperty(const CString& strName, LPCTSTR lpszDescr)
 : CMFCPropertyGridProperty(strName, COleVariant((LPCTSTR)L""), lpszDescr, 0)
{
	m_dwFlags = 0x0002;
}


MFCPropertyGridListSelProperty::~MFCPropertyGridListSelProperty(void)
{
}

VOID MFCPropertyGridListSelProperty::InitMap(std::map<int,std::wstring>& list)
{
	m_maps		=	list;
}
VOID MFCPropertyGridListSelProperty::SetSelect(std::vector<int>& list)
{
	m_lists		=	list;
	std::wstring str	=	L"";
	for(int i=0;i<m_lists.size();i++){
		std::map<int,std::wstring>::iterator it = m_maps.find(m_lists[i]);
		if(it!=m_maps.end()){
			if(str.size()>0)
				str += L",";
			str += it->second.c_str();
		}
	}
	m_varValue	=	str.c_str();
}
void MFCPropertyGridListSelProperty::OnClickButton(CPoint point)
{
	ASSERT_VALID(this);
	ASSERT_VALID(m_pWndList);
	ASSERT_VALID(m_pWndInPlace);
	ASSERT(::IsWindow(m_pWndInPlace->GetSafeHwnd()));

	m_bButtonIsDown = TRUE;
	Redraw();
	//
	CListBoxEditor edit(m_pWndList);
	edit.m_maps	=	this->m_maps;
	edit.m_mapsel = this->m_lists;
	if(edit.DoModal() == IDOK)
	{
		SetSelect(edit.m_mapsel);
		m_pWndList->OnPropertyChanged(this);
	}
	//
	m_bButtonIsDown = FALSE;
	Redraw();
	if (m_pWndInPlace != NULL)
	{
		m_pWndInPlace->SetFocus();
	}
	else
	{
		m_pWndList->SetFocus();
	}
}
IMPLEMENT_DYNAMIC(CMFCPropertyGridButtonProperty, CMFCPropertyGridProperty)

CMFCPropertyGridButtonProperty::CMFCPropertyGridButtonProperty(const CString& strName, const CString& strBigText, LPCTSTR lpszDescr)
 : CMFCPropertyGridProperty(strName, COleVariant((LPCTSTR)strBigText), lpszDescr, 0)
{
	m_dwFlags = 0x0002;
}


CMFCPropertyGridButtonProperty::~CMFCPropertyGridButtonProperty(void)
{
}

void CMFCPropertyGridButtonProperty::OnClickButton(CPoint point)
{
	ASSERT_VALID(this);
	ASSERT_VALID(m_pWndList);
	ASSERT_VALID(m_pWndInPlace);
	ASSERT(::IsWindow(m_pWndInPlace->GetSafeHwnd()));

	m_bButtonIsDown = TRUE;
	Redraw();
	//
	m_pWndList->OnPropertyChanged(this);
	//
	m_bButtonIsDown = FALSE;
	Redraw();
	if (m_pWndInPlace != NULL)
	{
		m_pWndInPlace->SetFocus();
	}
	else
	{
		m_pWndList->SetFocus();
	}
}


IMPLEMENT_DYNAMIC(CMFCPropertyGridMyFileProperty, CMFCPropertyGridProperty)

CMFCPropertyGridMyFileProperty::CMFCPropertyGridMyFileProperty(const CString& strName, const CString& strFolderName, DWORD_PTR dwData, LPCTSTR lpszDescr, DWORD dwFileFlags) : CMFCPropertyGridProperty(strName, COleVariant((LPCTSTR)strFolderName), lpszDescr, dwData), m_bIsFolder(TRUE), m_dwFileOpenFlags(dwFileFlags)
{
	m_dwFlags = 0x0002;
}

CMFCPropertyGridMyFileProperty::CMFCPropertyGridMyFileProperty( const CString& strName, BOOL bOpenFileDialog, const CString& strFileName, LPCTSTR lpszDefExt, DWORD dwFileFlags, LPCTSTR lpszFilter, LPCTSTR lpszDescr, DWORD_PTR dwData) : CMFCPropertyGridProperty(strName, COleVariant((LPCTSTR)strFileName), lpszDescr, dwData), m_bOpenFileDialog(bOpenFileDialog), m_dwFileOpenFlags(dwFileFlags)
{
	m_dwFlags = 0x0002;
	m_strDefExt = lpszDefExt == NULL ? _T("") : lpszDefExt;
	m_strFilter = lpszFilter == NULL ? _T("") : lpszFilter;

	m_bIsFolder  = FALSE;
}

CMFCPropertyGridMyFileProperty::~CMFCPropertyGridMyFileProperty()
{
}

VOID CMFCPropertyGridMyFileProperty::SetInitDir(const wchar_t* str)
{
	wchar_t szDrive[256], szDir[256];
	_wsplitpath(str, szDrive, szDir, 0, 0);
	wcscat(szDrive, szDir);
	this->m_strInitDir	=	szDrive;
}

void CMFCPropertyGridMyFileProperty::OnClickButton(CPoint /*point*/)
{
	ASSERT_VALID(this);
	ASSERT_VALID(m_pWndList);
	ASSERT_VALID(m_pWndInPlace);
	ASSERT(::IsWindow(m_pWndInPlace->GetSafeHwnd()));

	m_bButtonIsDown = TRUE;
	Redraw();
	CString strPath = m_varValue.bstrVal;
	BOOL bUpdate = FALSE;

	if (m_bIsFolder)
	{
		if (afxShellManager == NULL)
		{
			CWinAppEx* pApp = DYNAMIC_DOWNCAST(CWinAppEx, AfxGetApp());
			if (pApp != NULL)
			{
				pApp->InitShellManager();
			}
		}

		if (afxShellManager == NULL)
		{
			ASSERT(FALSE);
		}
		else
		{
			bUpdate = afxShellManager->BrowseForFolder(strPath, m_pWndList, strPath);
		}
	}
	else
	{
		m_browser.clear();
		const wchar_t* ch	=	L"default_init_zip";
		if(wcsnicmp(strPath, ch, wcslen(ch)) == 0)
			strPath	=	L"";
		CFileDialog dlg(m_bOpenFileDialog, m_strDefExt, strPath, m_dwFileOpenFlags, m_strFilter, m_pWndList);
		dlg.m_pOFN->lpstrInitialDir	=	m_strInitDir;
		if (dlg.DoModal() == IDOK)
		{
			bUpdate = TRUE;
			if(this->m_dwFileOpenFlags & OFN_ALLOWMULTISELECT)
			{
				POSITION pos	=	dlg.GetStartPosition();
				while(pos)
				{
					m_browser.push_back((const wchar_t*)dlg.GetNextPathName(pos));
				}
				if(m_browser.size() > 0)
					strPath = m_browser[0].c_str();
			}
			else
			{
				strPath = dlg.GetPathName();
				m_browser.push_back((const wchar_t*)strPath);
			}
		}
	}
	SetProjectPath();
	if (bUpdate)
	{
		if (m_pWndInPlace != NULL)
		{
			m_pWndInPlace->SetWindowText(strPath);
		}

		m_varValue = (LPCTSTR) strPath;
	}

	m_bButtonIsDown = FALSE;
	Redraw();

	if (m_pWndInPlace != NULL)
	{
		m_pWndInPlace->SetFocus();
	}
	else
	{
		m_pWndList->SetFocus();
	}
}



IMPLEMENT_DYNAMIC(CMFCPropertyGridIntProperty, CMFCPropertyGridProperty)

CMFCPropertyGridIntProperty::CMFCPropertyGridIntProperty(const CString& strName, INT nMin, INT nMax, INT nPage, LPCTSTR lpszDescr)
 : CMFCPropertyGridProperty(strName, (_variant_t)(long)0, lpszDescr, 0)
{
	m_dwFlags	=	0x0004;
	m_nMin		=	nMin;
	m_nMax		=	nMax;
	m_nPage		=	nPage;
}

CMFCPropertyGridIntProperty::~CMFCPropertyGridIntProperty(void)
{
}

CSpinButtonCtrl* CMFCPropertyGridIntProperty::CreateSpinControl(CRect rectSpin)
{
	ASSERT_VALID(this);
	ASSERT_VALID(m_pWndList);

	CChangeSpinButtonCtrl*	p	=	new CChangeSpinButtonCtrl;
	p->m_pIntProp	=	this;
	CSpinButtonCtrl* pWndSpin = p;

	if (!pWndSpin->Create(WS_CHILD | WS_VISIBLE | UDS_ARROWKEYS | UDS_SETBUDDYINT | UDS_NOTHOUSANDS, rectSpin, m_pWndList, AFX_PROPLIST_ID_INPLACE))
	{
		return NULL;
	}
	pWndSpin->SetRange32(m_nMin, m_nMax);
	return pWndSpin;
}

VOID		CMFCPropertyGridIntProperty::OnUpClick()
{
	_variant_t v(GetValue());
	assert(v.vt == VT_I4);
	v.lVal	+=	m_nPage;
	OnUpdate(v);
}

VOID		CMFCPropertyGridIntProperty::OnDownClick()
{
	_variant_t v(GetValue());
	assert(v.vt == VT_I4);
	v.lVal	-=	m_nPage;
	OnUpdate(v);
}

void CMFCPropertyGridIntProperty::SetValue(const COleVariant& varValue)
{
	COleVariant v	=	varValue;
	if(v.lVal < m_nMin)
		v.lVal	=	m_nMin;
	else if(v.lVal > m_nMax)
		v.lVal	=	m_nMax;
	__super::SetValue(v);
}

void CMFCPropertyGridIntProperty::OnUpdate(const COleVariant& var)
{
	SetValue(var);
	m_pWndList->OnPropertyChanged(this);
}

IMPLEMENT_DYNAMIC(CMFCPropertyGridFloatProperty, CMFCPropertyGridProperty)

CMFCPropertyGridFloatProperty::CMFCPropertyGridFloatProperty(const CString& strName, double nMin, double nMax, double nPage, LPCTSTR lpszDescr)
 : CMFCPropertyGridProperty(strName, (_variant_t)(float)0.0f, lpszDescr, 0)
{
	m_dwFlags	=	0x0004;
	m_fMin		=	nMin;
	m_fMax		=	nMax;
	m_fPage		=	nPage;
}

CMFCPropertyGridFloatProperty::~CMFCPropertyGridFloatProperty(void)
{
}

CSpinButtonCtrl* CMFCPropertyGridFloatProperty::CreateSpinControl(CRect rectSpin)
{
	ASSERT_VALID(this);
	ASSERT_VALID(m_pWndList);

	CChangeSpinButtonCtrl*	p	=	new CChangeSpinButtonCtrl;
	p->m_pFloatProp	=	this;
	CSpinButtonCtrl* pWndSpin = p;

	if (!pWndSpin->Create(WS_CHILD | WS_VISIBLE | UDS_ARROWKEYS | UDS_SETBUDDYINT | UDS_NOTHOUSANDS, rectSpin, m_pWndList, AFX_PROPLIST_ID_INPLACE))
	{
		return NULL;
	}
	pWndSpin->SetRange32(INT_MIN, INT_MAX);
	return pWndSpin;
}

VOID		CMFCPropertyGridFloatProperty::OnUpClick()
{
	_variant_t v(GetValue());
	assert(v.vt == VT_R4);
	float f	=	v.operator float();
	f		+=	m_fPage;
	OnUpdate(_variant_t(f));
}

VOID		CMFCPropertyGridFloatProperty::OnDownClick()
{
	_variant_t v(GetValue());
	assert(v.vt == VT_R4);
	float f	=	v.operator float();
	f		-=	m_fPage;
	OnUpdate(_variant_t(f));
}


void CMFCPropertyGridFloatProperty::SetValue(const COleVariant& varValue)
{
	COleVariant v	=	varValue;
	if(v.fltVal < m_fMin)
		v.fltVal	=	m_fMin;
	else if(v.fltVal > m_fMax)
		v.fltVal	=	m_fMax;
	__super::SetValue(v);
}


void CMFCPropertyGridFloatProperty::OnUpdate(const COleVariant& var)
{
	SetValue(var);
	m_pWndList->OnPropertyChanged(this);
}
