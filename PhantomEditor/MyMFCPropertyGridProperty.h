/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com


Copyright 2009-2016 Zhao Dexian
-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
#pragma once
#include <string>
#include <vector>
#include <map>
#include "afxpropertygridctrl.h"
#include "ChangeSpinButtonCtrl.h"

class CMFCPropertyGridButtonProperty :public CMFCPropertyGridProperty
{
	DECLARE_DYNAMIC(CMFCPropertyGridButtonProperty)
public:
	CMFCPropertyGridButtonProperty(const CString& strName, const CString& strBigText, LPCTSTR lpszDescr = NULL);
	virtual ~CMFCPropertyGridButtonProperty();
public:
	virtual void OnClickButton(CPoint point);
};


class CMFCPropertyGridMyFileProperty : public CMFCPropertyGridProperty
{
	DECLARE_DYNAMIC(CMFCPropertyGridMyFileProperty)

// Construction
public:

	CMFCPropertyGridMyFileProperty(const CString& strName, const CString& strFolderName, DWORD_PTR dwData = 0, LPCTSTR lpszDescr = NULL, DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT);
	CMFCPropertyGridMyFileProperty(const CString& strName, BOOL bOpenFileDialog, const CString& strFileName, LPCTSTR lpszDefExt = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, LPCTSTR lpszFilter = NULL, LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0);
	virtual ~CMFCPropertyGridMyFileProperty();

	VOID		SetInitDir(const wchar_t* str);
	CString		m_strInitDir;
	std::vector<std::wstring>	m_browser;
// Overrides
public:
	virtual void OnClickButton(CPoint point);

// Attributes
protected:
	BOOL    m_bIsFolder;

	// File open dialog atributes:
	BOOL    m_bOpenFileDialog; // TRUE - use "File Open/Save" diaog; otherwise - folder selection dialog
	DWORD   m_dwFileOpenFlags;
	CString m_strDefExt;
	CString m_strFilter;
};


class MFCPropertyGridListProperty: public CMFCPropertyGridProperty
{
	DECLARE_DYNAMIC(MFCPropertyGridListProperty)
public:
	MFCPropertyGridListProperty(const CString& strName, LPCTSTR lpszDescr = NULL);
	virtual ~MFCPropertyGridListProperty();
// Overrides
	VOID						SetList(std::vector<std::wstring>& list);
	std::vector<std::wstring>	m_lists;
public:
	virtual void OnClickButton(CPoint point);
};

class MFCPropertyGridListSelProperty: public CMFCPropertyGridProperty
{
	DECLARE_DYNAMIC(MFCPropertyGridListSelProperty)
public:
	MFCPropertyGridListSelProperty(const CString& strName, LPCTSTR lpszDescr = NULL);
	virtual ~MFCPropertyGridListSelProperty();
// Overrides
	VOID						SetSelect(std::vector<int>& list);
	std::vector<int>			m_lists;
	VOID						InitMap(std::map<int,std::wstring>& list);
	std::map<int,std::wstring>	m_maps;
public:
	virtual void OnClickButton(CPoint point);
};

class CMFCPropertyGridBigTextProperty :public CMFCPropertyGridProperty
{
	DECLARE_DYNAMIC(CMFCPropertyGridBigTextProperty)
public:
	CMFCPropertyGridBigTextProperty(const CString& strName, const CString& strBigText, LPCTSTR lpszDescr = NULL, INT nMaxTextLength = 100000);
	virtual ~CMFCPropertyGridBigTextProperty();
	void	SetValue(const COleVariant& varValue);
// Overrides
public:
	int		m_nMaxTextLength;
	virtual void OnClickButton(CPoint point);
};

class CMFCPropertyGridTextProperty :public CMFCPropertyGridProperty
{
	DECLARE_DYNAMIC(CMFCPropertyGridTextProperty)
public:
	CMFCPropertyGridTextProperty(const CString& strName, const CString& strText, LPCTSTR lpszDescr = NULL, INT nMaxTextLength = 100000);
	virtual ~CMFCPropertyGridTextProperty();
	void	SetValue(const COleVariant& varValue);
// Overrides
public:
	int		m_nMaxTextLength;
};

class CMFCPropertyGridIntProperty :public CMFCPropertyGridProperty
{
	DECLARE_DYNAMIC(CMFCPropertyGridIntProperty)
public:
	CMFCPropertyGridIntProperty(const CString& strName, INT nMin, INT nMax, INT nPage, LPCTSTR lpszDescr = NULL);
	virtual ~CMFCPropertyGridIntProperty();
	VOID		OnUpClick();
	VOID		OnDownClick();
public:
	void		OnUpdate(const COleVariant& var);
	virtual CSpinButtonCtrl* CreateSpinControl(CRect rectSpin);
	virtual void SetValue(const COleVariant& varValue);
	VOID		SetMinMax(int dMin, int dMax){m_nMin = dMin;  m_nMax = dMax;}
public:
	INT		m_nMin;
	INT		m_nMax;
	INT		m_nPage;
};

class CMFCPropertyGridFloatProperty :public CMFCPropertyGridProperty
{
	DECLARE_DYNAMIC(CMFCPropertyGridFloatProperty)
public:
	CMFCPropertyGridFloatProperty(const CString& strName, double nMin, double nMax, double nPage, LPCTSTR lpszDescr = NULL);
	virtual ~CMFCPropertyGridFloatProperty();
	virtual void SetValue(const COleVariant& varValue);
	VOID		OnUpClick();
	VOID		OnDownClick();
	VOID		SetMinMax(double dMin, double dMax){m_fMin = dMin;  m_fMax = dMax;}
public:
	void		OnUpdate(const COleVariant& var);
	virtual CSpinButtonCtrl* CreateSpinControl(CRect rectSpin);
public:
	double		m_fMin;
	double		m_fMax;
	double		m_fPage;
};
