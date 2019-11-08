// DlgViewerHttp.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgViewerHttp.h"


// CDlgViewerHttp 对话框

IMPLEMENT_DYNCREATE(CDlgViewerHttp, CDHtmlDialog)

CDlgViewerHttp::CDlgViewerHttp(CWnd* pParent /*=NULL*/)
	: CDHtmlDialog(CDlgViewerHttp::IDD, CDlgViewerHttp::IDH, pParent)
{

}

CDlgViewerHttp::~CDlgViewerHttp()
{
}

void CDlgViewerHttp::DoDataExchange(CDataExchange* pDX)
{
	CDHtmlDialog::DoDataExchange(pDX);
}

BOOL CDlgViewerHttp::OnInitDialog()
{
	CDHtmlDialog::OnInitDialog();
	SetHostFlags(DOCHOSTUIFLAG_FLAT_SCROLLBAR);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

BEGIN_MESSAGE_MAP(CDlgViewerHttp, CDHtmlDialog)
END_MESSAGE_MAP()

BEGIN_DHTML_EVENT_MAP(CDlgViewerHttp)
END_DHTML_EVENT_MAP()



// CDlgViewerHttp 消息处理程序

HRESULT CDlgViewerHttp::OnButtonOK(IHTMLElement* /*pElement*/)
{
	OnOK();
	return S_OK;
}

HRESULT CDlgViewerHttp::OnButtonCancel(IHTMLElement* /*pElement*/)
{
	OnCancel();
	return S_OK;
}


BOOL CDlgViewerHttp::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	switch(pMsg->message)
	{
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEMOVE:
		return true;
	case WM_KEYDOWN:
	case WM_KEYUP:
		{
			char buf[512];
			::GetClassNameA(pMsg->hwnd, buf, 512);
			if(GetSelectKeyboard() && (pMsg->wParam == 'C' || pMsg->wParam == 'A'))
				return true;
		}
		break;
	}
	return __super::PreTranslateMessage(pMsg);
}
