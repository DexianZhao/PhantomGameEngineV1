// MyWebBrowser.cpp : 实现文件
//

#include "stdafx.h"
#include "MyWebBrowser.h"


// CMyWebBrowser 对话框

IMPLEMENT_DYNCREATE(CMyWebBrowser, CDHtmlDialog)

CMyWebBrowser::CMyWebBrowser(CWnd* pParent /*=NULL*/)
	: CDHtmlDialog(CMyWebBrowser::IDD, CMyWebBrowser::IDH, pParent)
{

}

CMyWebBrowser::~CMyWebBrowser()
{
}
extern BOOL		RunDocumentHttp(const char* proj);
void CMyWebBrowser::OnBeforeNavigate(LPDISPATCH pDisp, LPCTSTR szUrl)
{
	int len = strlen(szUrl);
	if(len<1024){
		char szExt[1024];
		_splitpath(szUrl, 0, 0, 0, szExt);
		if(stricmp(szExt, ".phantom") == 0 && RunDocumentHttp(szUrl))
		{
			this->m_pBrowserApp->Stop();
			return;
		}
	}
	__super::OnBeforeNavigate(pDisp, szUrl);
}
void CMyWebBrowser::OnNavigateComplete(LPDISPATCH pDisp, LPCTSTR szUrl)
{
	int len = strlen(szUrl);
	if(len<1024){
		char szExt[1024];
		_splitpath(szUrl, 0, 0, 0, szExt);
		if(stricmp(szExt, ".phantom") == 0)
		{
			m_pBrowserApp->GoBack();
			return;
		}
	}
	__super::OnNavigateComplete(pDisp, szUrl);
	CRect rc;
	this->GetParent()->GetClientRect(&rc);
	this->MoveWindow(0, 20, rc.Width(), rc.Height());
	this->ShowWindow(SW_SHOW);
}
void CMyWebBrowser::DoDataExchange(CDataExchange* pDX)
{
	CDHtmlDialog::DoDataExchange(pDX);
}

BOOL CMyWebBrowser::OnInitDialog()
{
	CDHtmlDialog::OnInitDialog();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

BEGIN_MESSAGE_MAP(CMyWebBrowser, CDHtmlDialog)
END_MESSAGE_MAP()

BEGIN_DHTML_EVENT_MAP(CMyWebBrowser)
	DHTML_EVENT_ONCLICK(_T("ButtonOK"), OnButtonOK)
	DHTML_EVENT_ONCLICK(_T("ButtonCancel"), OnButtonCancel)
END_DHTML_EVENT_MAP()



// CMyWebBrowser 消息处理程序

HRESULT CMyWebBrowser::OnButtonOK(IHTMLElement* /*pElement*/)
{
	OnOK();
	return S_OK;
}

HRESULT CMyWebBrowser::OnButtonCancel(IHTMLElement* /*pElement*/)
{
	OnCancel();
	return S_OK;
}
