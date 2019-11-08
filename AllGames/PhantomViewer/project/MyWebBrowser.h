#pragma once
#include "resource.h"
#ifdef _WIN32_WCE
#error "Windows CE 不支持 CDHtmlDialog。"
#endif 

// CMyWebBrowser 对话框

class CMyWebBrowser : public CDHtmlDialog
{
	DECLARE_DYNCREATE(CMyWebBrowser)

public:
	CMyWebBrowser(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMyWebBrowser();
// 重写
	HRESULT OnButtonOK(IHTMLElement *pElement);
	HRESULT OnButtonCancel(IHTMLElement *pElement);
	virtual void OnNavigateComplete(LPDISPATCH pDisp, LPCTSTR szUrl);
	virtual void OnBeforeNavigate(LPDISPATCH pDisp, LPCTSTR szUrl);
// 对话框数据
	enum { IDD = IDD_MYWEBBROWSER, IDH = IDR_HTML_MYWEBBROWSER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	DECLARE_DHTML_EVENT_MAP()
};
