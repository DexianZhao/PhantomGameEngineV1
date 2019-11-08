#pragma once
#include "resource.h"
#include "GCMFCHelper.h"

#ifdef _WIN32_WCE
#error "Windows CE 不支持 CDHtmlDialog。"
#endif 

// CDlgViewerHttp 对话框

class CDlgViewerHttp : public CDHtmlDialog, public ViewerWindow
{
	DECLARE_DYNCREATE(CDlgViewerHttp)

public:
	CDlgViewerHttp(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgViewerHttp();
// 重写
	HRESULT OnButtonOK(IHTMLElement *pElement);
	HRESULT OnButtonCancel(IHTMLElement *pElement);
	CWnd*			GetWindowPtr(){return this;}
	VOID			OnMouseWheel(int delta){}
	VOID			OnKeyDown(int key){}
	VOID			OnKeyUp(int key){}
	VOID			OnViewSize(CRect* rc){}

// 对话框数据
	enum { IDD = IDD_VIEWER_HTTP, IDH = IDR_HTML_DLGVIEWERHTTP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	DECLARE_DHTML_EVENT_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
