// DlgResItems.cpp : 实现文件
//

#include "stdafx.h"
#include "afxdialogex.h"
#include "GCViewBase.h"
#include "MainFrm.h"
#include <io.h>
#include "DlgResDetail.h"
#include "IDEHttp.h"
#include "GCLanguage.h"
#include "DlgResShop.h"
#include "GCViewManager.h"
#include "GCSceneEditor3D.h"

#define	ITEM_WIDTH	132
#define	ITEM_HEIGHT	200
HomeResItem::HomeResItem()
{
	m_buyState	=	0;
	m_itemID	=	0;
	m_state		=	0;
	m_bResize	=	false;
	m_nState	=	0;
	m_bIsDownload	=	false;
}
VOID		HomeResItem::SetRect(CRect rc)
{
	rc.right+=rc.left;
	rc.bottom+=rc.top;
	m_rcWindow = rc;
	m_rcName.SetRect(rc.left, rc.top, rc.Width(), 20);
	m_rcName.right+=m_rcName.left;m_rcName.bottom+=m_rcName.top;
	int width = 64;
	m_rcDetail.SetRect(rc.left+1, rc.bottom-25, 64, 24);
	m_rcDetail.right+=m_rcDetail.left;m_rcDetail.bottom+=m_rcDetail.top;
	m_rcBuy.SetRect(rc.right-65, rc.bottom-25, 64, 24);
	m_rcBuy.right+=m_rcBuy.left;m_rcBuy.bottom+=m_rcBuy.top;
	m_rcVb.SetRect(rc.right-64, rc.bottom-50, 64, 24);
	m_rcVb.right+=m_rcVb.left;m_rcVb.bottom+=m_rcVb.top;
	m_rcImage.SetRect(m_rcWindow.left+2, m_rcWindow.top+21, 0, 0);
	m_rcImage.right=m_rcImage.left+128;
	m_rcImage.bottom=m_rcImage.top+128;
	m_bResize = true;
}

// CDlgResShop 对话框
CDlgResShop*	s_resItem = 0;
IMPLEMENT_DYNAMIC(CDlgResShop, CDialogEx)

CDlgResShop::CDlgResShop(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgResShop::IDD, pParent)
{
	s_resItem	=	this;
	m_nBeginY	=	0;
	m_downDetailItem = m_downBuyItem = 0;
	m_nSelect	=	0;
}

CDlgResShop::~CDlgResShop()
{
	for(int i=0;i<m_items.size();i++){
		delete m_items[i];
	}
}
VOID CDlgResShop::Update()
{
	CRect rc;
	GetClientRect(&rc);
	this->InvalidateRect(&CRect(rc.left, 22, rc.right, rc.bottom));
}
VOID CDlgResShop::Resize()
{
	CRect rc;
	GetClientRect(&rc);
	int nLeft = 5;
	int nTop = 22;
	int numW = ((rc.Width()-nLeft)/(ITEM_WIDTH+10));
	int numH = (m_items.size()/numW+1);//((rc.Height()-nTop)/(ITEM_HEIGHT+10));
	int nMaxH = (ITEM_HEIGHT+10) * (m_items.size()/numW+1) + nTop + 100;
	int index = 0;
	for(int y=0;y<numH;y++){
		for(int x=0;x<numW;x++){
			if(index>=m_items.size())
				break;
			HomeResItem* item = m_items[index++];
			item->SetRect(CRect(nLeft + x*(ITEM_WIDTH+10), nTop + y*(ITEM_HEIGHT+10) - m_nBeginY, ITEM_WIDTH, ITEM_HEIGHT));
			//item->Invalidate();
		}
	}
	this->InvalidateRect(&CRect(rc.left, 23, rc.right, rc.bottom));
}

void CDlgResShop::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_TAB1, m_tab);
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgResShop, CDialogEx)
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CDlgResShop::OnTcnSelchangeTab1)
	ON_BN_CLICKED(IDC_REFRESH, &CDlgResShop::OnBnClickedRefresh)
	ON_WM_MOUSEHWHEEL()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()


// CDlgResShop 消息处理程序

VOID CDlgResShop::ResetScroll()
{
	CRect rc;
	GetClientRect(&rc);
	int nLeft = 5;
	int nTop = 5;
	int numW = ((rc.Width()-nLeft)/(ITEM_WIDTH+10));
	int numH = (m_items.size()/numW+1);//((rc.Height()-nTop)/(ITEM_HEIGHT+10));
	int nMaxH = numH * (ITEM_HEIGHT+10)  - rc.Height() + 64;
	this->SetScrollRange(SB_VERT, 0, nMaxH);

}
void CDlgResShop::OnSize(UINT nType, int cx, int cy)
{
	if(!m_back.IsNull())
		m_back.Destroy();
	CDialogEx::OnSize(nType, cx, cy);
	Resize();
	ResetScroll();
}

BOOL CDlgResShop::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_imgGold.LoadFromResource(AfxGetResourceHandle(), IDB_GOLD);
	m_imgButton.LoadFromResource(AfxGetResourceHandle(), IDB_SHOP_BUTTON);
	Resize();
	ResetScroll();
	HICON hIcon;
	hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetIcon(hIcon, TRUE);
	SetIcon(hIcon, FALSE);
	m_tab.InsertItem(0, Language(L"Res", L"已购资源"));
	m_tab.InsertItem(1, Language(L"ResMarket", L"资源超市"));
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgResShop::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int minpos;
	int maxpos;
	GetScrollRange(SB_VERT, &minpos, &maxpos); 
	int curpos = GetScrollPos(SB_VERT);
	switch (nSBCode)
	{
	case SB_LEFT:
		curpos = minpos;
		break;
	case SB_RIGHT:
		curpos = maxpos;
		break;
	case SB_ENDSCROLL:
		break;
	case SB_LINELEFT:
		if (curpos > minpos)
			curpos--;
		break;
	case SB_LINERIGHT:   // Scroll right.
		if (curpos < maxpos)
			curpos++;
		break;
	case SB_PAGELEFT:    // Scroll one page left.
		{
			SCROLLINFO   info;
			GetScrollInfo(SB_VERT, &info, SIF_ALL);
			if (curpos > minpos)
				curpos = max(minpos, curpos - 100);
		}
		break;
	case SB_PAGERIGHT:      // Scroll one page right.
		{
			SCROLLINFO   info;
			GetScrollInfo(SB_VERT, &info, SIF_ALL);

			if (curpos < maxpos)
				curpos = min(maxpos, curpos + 100);
		}
		break;
	case SB_THUMBPOSITION:
		curpos = nPos;
		break;
	case SB_THUMBTRACK:
		curpos = nPos;
		break;
	}
	if(curpos < minpos)
		curpos = minpos;
	else if(curpos >= maxpos)
		curpos	=	maxpos;
	SetScrollPos(SB_VERT, curpos);
	m_nBeginY	=	curpos;
	this->Resize();
	CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
}



BOOL CDlgResShop::OnEraseBkgnd(CDC* pDC)
{
	return true;//CDialogEx::OnEraseBkgnd(pDC);
}

VOID	BltButton(CDC& src, CDC& dest, const CRect& rc, int index){
	dest.BitBlt(rc.left, rc.top, rc.Width(), rc.Height(), &src, 0, index*24, SRCCOPY);
}
void CDlgResShop::OnPaint()
{
	CPaintDC dcPaint(this);
	CRect rc;
	GetClientRect(&rc);
	if(m_back.IsNull())
		m_back.Create(rc.Width(), rc.Height(), 24);
	CFont* font = GetFont();
	CDC& dc = *CDC::FromHandle(m_back.GetDC());
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(20,20,20));
	CFont* oldFont = dc.SelectObject(font);
	dc.FillRect(&rc, &CBrush(RGB(250,250,250)));
	dc.Draw3dRect(&rc, RGB(220,220,220), RGB(220,220,220));
	CDC& dcGold = *CDC::FromHandle(m_imgGold.GetDC());
	CDC& dcButton = *CDC::FromHandle(m_imgButton.GetDC());
	for(int i=0;i<this->m_items.size();i++){
		HomeResItem* res = m_items[i];
		if(res->m_rcWindow.top>rc.bottom||res->m_rcWindow.bottom<rc.top)
			continue;
		dc.Draw3dRect(&CRect(res->m_rcWindow.left, res->m_rcVb.top, res->m_rcWindow.right, res->m_rcVb.bottom), RGB(220,220,220), RGB(220,220,220));
		dc.TransparentBlt(res->m_rcVb.left,res->m_rcVb.top+4, 16, 16, &dcGold, 0, 0, 48, 48, 0);
		dc.DrawText(res->m_payment, &CRect(res->m_rcVb.left+20, res->m_rcVb.top, res->m_rcVb.right, res->m_rcVb.top+24), DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		static const wchar_t* payment = Language(L"ResPay", L"资源价格");
		dc.DrawText(payment, &CRect(res->m_rcWindow.left, res->m_rcVb.top, res->m_rcVb.left, res->m_rcVb.top+24), DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		if(!res->m_image.IsNull())
			res->m_image.BitBlt(dc.m_hDC, res->m_rcImage.left, res->m_rcImage.top);
		dc.DrawText(res->m_name, &res->m_rcName, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		//
		static const wchar_t* detail = Language(L"Detail", L"详细信息");
		BltButton(dcButton, dc, res->m_rcDetail, res == m_downDetailItem?7:6);
		if(res->m_buyState==HomeResItem::OnBuy_BuyPayment||res->m_buyState==HomeResItem::OnBuy_BuyVip2)
			BltButton(dcButton, dc, res->m_rcBuy, res == m_downBuyItem?1:0);
		else if(res->m_buyState==HomeResItem::OnBuy_BuyDownload)
			BltButton(dcButton, dc, res->m_rcBuy, res == m_downBuyItem?3:2);
		else if(res->m_buyState==HomeResItem::OnBuy_Use||res->m_buyState==HomeResItem::OnBuy_UseToGame)
			BltButton(dcButton, dc, res->m_rcBuy, res == m_downBuyItem?5:4);
		dc.DrawText(detail, &res->m_rcDetail, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		dc.DrawText(res->m_buy, &res->m_rcBuy, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		//
		dc.Draw3dRect(&res->m_rcName, RGB(120,120,120), RGB(120,120,120));
		dc.Draw3dRect(&res->m_rcWindow, RGB(120,120,120), RGB(120,120,120));
	}
	m_imgButton.ReleaseDC();
	m_imgGold.ReleaseDC();
	dcPaint.BitBlt(0, 0, rc.Width(), rc.Height(), &dc, 0, 0, SRCCOPY);
	if(oldFont)
		dc.SelectObject(oldFont);
	m_back.ReleaseDC();
}

VOID DownComplete(const char* requestText, const char* indocFile, std::vector<char>& buffers, BOOL bError, LPARAM lParam)
{
	if(!bError){
		for(int i=0;i<s_resItem->m_items.size();i++){
			if(s_resItem->m_items[i]->m_itemID == lParam){
				if(!s_resItem->m_items[i]->m_image.IsNull())
					s_resItem->m_items[i]->m_image.Destroy();
				s_resItem->m_items[i]->m_image.Load(Wutf8(indocFile));
				break;
			}
		}
		s_resItem->Update();
	}
}
const char*	ToLine(char* text){
	int len = strlen(text);
	for(int i=0;i<len;i++){
		if(text[i]=='\\'&&text[i+1]=='n')
		{
			text[i] = '\r';
			text[i+1] = '\n';
		}
	}
	return text;
}

extern	const char*	GetDocumentPath();
VOID		HomeResItem::ResetState()
{
	char email[128];
	ReadRegValue("email", email);
	char file[512];
	FileName ext;
	m_pakFile.splitpath(0, 0, &ext);
	sprintf(file, "%s\\%s\\%04d%s", GetDocumentPath(), email, this->m_itemID, ext.t);
	char shop[512];
	sprintf(shop, "%sresshop/%s%s", GetProjectPath(), A(m_name).buf, ext.t);
	BOOL bUsed = false;
	if(fexist(shop))
		bUsed = true;
	BOOL bExistPak = false;
	FILE* ftxt = fopen(file, "rb");
	if(ftxt)
	{
		bExistPak = true;
		fclose(ftxt);
	}
	int bPower	=	_wtoi(this->m_power);
	m_buyState	=	0;
	if(bPower>0)
	{
		wchar_t wbuf[128];
		swprintf(wbuf, L"VIP%d", bPower);
		m_payment	=	wbuf;
		if(bUsed)
		{
			m_buyState	=	OnBuy_UseToGame;
			m_buy		=	Language(L"BuyUsed", L"使用");
		}
		else if(bExistPak&&m_nState==1)
		{
			m_buyState	=	OnBuy_Use;
			m_buy		=	Language(L"BuyUse", L"使用");
		}
		else{
			if(m_nState==0)
			{
				m_buyState	=	OnBuy_BuyVip2;
				m_buy		=	Language(L"Buy", L"成为VIP");
				m_buy		+=	m_power;
			}
			else{
				m_buyState	=	OnBuy_BuyDownload;
				m_buy		=	Language(L"Buy", L"下载");
			}
		}
	}
	else{
		if(bUsed)
		{
			m_buyState	=	OnBuy_UseToGame;
			m_buy		=	Language(L"BuyUsed", L"使用");
		}
		else if(bExistPak&&m_nState==1)
		{
			m_buyState	=	OnBuy_Use;
			m_buy		=	Language(L"BuyUse", L"使用");
		}
		else{
			if(m_nState==0)
			{
				m_buyState	=	OnBuy_BuyPayment;
				m_buy		=	Language(L"Buy", L"购买");
			}
			else
			{
				m_buyState	=	OnBuy_BuyDownload;
				m_buy		=	Language(L"Buy", L"下载");
			}
		}
	}
	//m_buy		=	Language(L"Buy", L"下载");
	//m_buy		=	Language(L"Buy", L"使用");
}

void WINAPI OnItemListCallback(Name& file, IDCJsonObject& obj, const char* errorText, void* param)//返回函数
{
	s_resItem->GetDlgItem(IDC_REFRESH)->EnableWindow(true);
	if(errorText){
		s_resItem->MessageBox(W(errorText), Language(L"产生错误"), MB_OK|MB_ICONERROR);
		s_resItem->UpdateTitle(Language(L"更新错误..."));//更新标题信息，资源超市+...
		return;
	}
	int result = obj["result"];
	if(result == 1){
		//写入最后一次登录的账号密码
		for(int i=0;i<s_resItem->m_items.size();i++)
			delete s_resItem->m_items[i];
		s_resItem->m_items.clear();
		int nCount = obj["count"];
		for(int i=0;i<nCount;i++){
			char buf[128];
			sprintf(buf, "name%d", i);
			std::string name = obj[buf];
			sprintf(buf, "desc%d", i);
			std::string desc = obj[buf];
			sprintf(buf, "img%d", i);
			std::string img = obj[buf];
			sprintf(buf, "file%d", i);
			std::string pakfile = obj[buf];
			sprintf(buf, "power%d", i);
			std::string power = obj[buf];
			sprintf(buf, "price%d", i);
			std::string price = obj[buf];
			sprintf(buf, "id%d", i);
			std::string id = obj[buf];
			sprintf(buf, "state%d", i);
			std::string state = obj[buf];
			sprintf(buf, "fname%d", i);
			std::string filename = obj[buf];
			//
			HomeResItem* item = new HomeResItem();
			ToLine((char*)desc.c_str());
			item->m_desc = Wutf8(desc.c_str());
			item->m_name = Wutf8(name.c_str());
			item->m_payment = Wutf8(price.c_str());
			item->m_power = Wutf8(power.c_str());
			item->m_itemID = atoi(id.c_str());
			item->m_nState = atoi(state.c_str());
			item->m_pakFile = pakfile.c_str();
			item->m_filename = filename.c_str();
			char szFile[128];
			char email[128];
			ReadRegValue("email", email);
			_splitpath(img.c_str(), 0, 0, szFile, 0);
			char file[512];
			sprintf(file, "%s\\%s\\%04d.jpg", GetDocumentPath(), email, item->m_itemID);
			if(downloadFile(img.c_str(), file, DownComplete, item->m_itemID)){
				item->m_image.LoadFromResource(AfxGetResourceHandle(), IDB_RES_IMAGE);
			}
			item->ResetState();
			s_resItem->m_items.push_back(item);
		}
		s_resItem->Resize();
		s_resItem->ResetScroll();
		s_resItem->UpdateTitle(Language(L"最新列表"));//更新标题信息，资源超市+...
	}
	else
	{
		s_resItem->UpdateTitle(Language(L"更新错误..."));//更新标题信息，资源超市+...
	}
}

void WINAPI OnItemBuyCallback(Name& file, IDCJsonObject& obj, const char* errorText, void* param)//返回函数
{
	if(errorText){
		s_resItem->MessageBox(W(errorText), Language(L"产生错误"), MB_OK|MB_ICONERROR);
		s_resItem->UpdateTitle(Language(L"购买错误..."));//更新标题信息，资源超市+...
		return;
	}
	int result = obj["result"];
	if(result == 1){
		HomeResItem* item = (HomeResItem*)param;
		item->m_nState = 1;
		item->ResetState();
		s_resItem->Update();
	}
	else{
		s_resItem->MessageBox(W(obj["error"]), Language(L"产生错误"), MB_OK|MB_ICONERROR);
		s_resItem->UpdateTitle(Language(L"购买错误..."));//更新标题信息，资源超市+...
	}
}

extern	BOOL	CheckLogin();
VOID CDlgResShop::UpdateTitle(const wchar_t* t)
{
	wchar_t wbuf[512];
	swprintf(wbuf, L"%s - %s", Language(L"资源超市"), t);
	SetWindowText(wbuf);
}

void CDlgResShop::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();
	m_downDetailItem = m_downBuyItem = 0;
	for(int i=0;i<this->m_items.size();i++){
		if(m_items[i]->m_rcDetail.PtInRect(point)){
			m_downDetailItem = m_items[i];
			this->Invalidate();
			break;
		}
		else if(m_items[i]->m_rcBuy.PtInRect(point)){
			m_downBuyItem = m_items[i];
			this->Invalidate();
			break;
		}
	}
	CDialogEx::OnLButtonDown(nFlags, point);
}

VOID DownCompletePak(const char* requestText, const char* indocFile, std::vector<char>& buffers, BOOL bError, LPARAM lParam)
{
	for(int i=0;i<s_resItem->m_items.size();i++){
		if(s_resItem->m_items[i]->m_itemID == lParam){
			s_resItem->m_items[i]->m_bIsDownload = false;
			//对pak文件进行解密并且针对这个用户单独加密，通过用户码进行加密
			//
			if(!bError){
				char ext[512];
				_splitpath(indocFile, 0, 0, 0, ext);
				if(stricmp(ext, ".zip")==0){
					FILE* f = fopen(indocFile, "wb");
					fwrite(&buffers.at(0), buffers.size(), 1, f);
					fclose(f);
					ShellExecuteA(NULL,"open", indocFile, NULL,NULL,SW_SHOW);
				}
				else{
					Phantom::CDynamicArray<char> dest,src;
					src.SetArrayCount(buffers.size());
					memcpy(src.GetPtr(), &buffers.at(0), buffers.size());
					if(Phantom::PakDirectory::ResetPassword(dest, src, 0, 0)){
						FILE* f = fopen(indocFile, "wb");
						fwrite(dest.GetPtr(), dest.size(), 1, f);
						fclose(f);
					}
				}
				s_resItem->m_items[i]->ResetState();
			}
			break;
		}
	}
	s_resItem->Update();
}

extern	VOID	ShowUserInfo();
extern	void	createFolders(const char* szSubFolders);
void CDlgResShop::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(m_downDetailItem&&m_downDetailItem->m_rcDetail.PtInRect(point)){
		CDlgResDetail dlg(this);
		dlg.m_strDetail = m_downDetailItem->m_desc;
		dlg.DoModal();
	}
	else if(m_downBuyItem&&m_downBuyItem->m_rcBuy.PtInRect(point)){
		switch(m_downBuyItem->m_buyState){
		case HomeResItem::OnBuy_Use:
			{
				FileName ext;
				m_downBuyItem->m_pakFile.splitpath(0, 0, &ext);
				char email[128];
				ReadRegValue("email", email);
				char file[512];
				sprintf(file, "%s\\%s\\%04d%s", GetDocumentPath(), email, m_downBuyItem->m_itemID, ext.t);
				if(stricmp(ext.t, ".zip")==0){
					ShellExecuteA(NULL,"open", file, NULL,NULL,SW_SHOW);
					return;
				}
				char path[256];
				path[0] = 0;
				if(stricmp(ext.t, ".pakui")==0)
					strcpy(path, "dlgs/");
				else if(stricmp(ext.t, ".pakmesh")==0)
					strcpy(path, "mesh/");
				char shop[512];
				sprintf(shop, "%s/%s%s%s", GetProjectPath(), path, m_downBuyItem->m_filename.t, ext.t);
				strlwr(shop);
				createFolders(shop);
				m_downBuyItem->m_buyState = 0;
				CopyFileA(file, shop, false);
				m_downBuyItem->ResetState();
			}
		case HomeResItem::OnBuy_UseToGame:
			{
				FileName ext;
				m_downBuyItem->m_pakFile.splitpath(0, 0, &ext);
				char path[256];
				path[0] = 0;
				if(stricmp(ext.t, ".pakui")==0)
					strcpy(path, "dlgs/");
				else if(stricmp(ext.t, ".pakmesh")==0)
					strcpy(path, "mesh/");
				char shop[512];
				sprintf(shop, "%s%s%s", path, m_downBuyItem->m_filename.t, ext.t);
				strlwr(shop);
				if(GetViewManager()->GetActiveView()&&GetViewManager()->GetActiveView()->GetViewerType() == FileViewerType_GCSceneEditor3D){
					static_cast<GCSceneEditor3D*>(GetViewManager()->GetActiveView())->InsertGeometry(W(shop).buf);
				}
				else{
					AfxGetApp()->OpenDocumentFile(W(shop));
				}
			}
			break;
		case HomeResItem::OnBuy_BuyVip2:
			{
				wchar_t wbuf[256];
				int power = _wtoi(m_downBuyItem->m_power);
				swprintf(wbuf, Language(L"VipLevels", L"你不是VIP%d用户，只有VIP%d用户才能免费获得这个资源\n是否升级成VIP%d？"), power, power, power);
				if(MessageBox(wbuf, 0, MB_YESNO)==IDNO)
					break;
				ShowUserInfo();
			}
			break;
		case HomeResItem::OnBuy_BuyPayment:
			{
				wchar_t wbuf[256];
				swprintf(wbuf, Language(L"Buy", L"购买这个资源需要支付%sV币，\n请问是否确定要支付并购买？？"), (const wchar_t*)m_downBuyItem->m_payment);
				if(MessageBox(wbuf, 0, MB_YESNO)==IDNO)
					break;
				IDCJsonObject obj;
				obj["key"] = GetHttpKey();
				obj["itemid"] = m_downBuyItem->m_itemID;
				Request("pvd_itembuy", obj, OnItemBuyCallback, true, m_downBuyItem);
				UpdateTitle(Language(L"正在购买物品..."));//更新标题信息，资源超市+...
			}
			break;
		case HomeResItem::OnBuy_BuyDownload:
			{
				if(!m_downBuyItem->m_bIsDownload){
					m_downBuyItem->m_bIsDownload = true;
					char email[128];
					ReadRegValue("email", email);
					char file[512];
					FileName ext;
					m_downBuyItem->m_pakFile.splitpath(0, 0, &ext);
					sprintf(file, "%s\\%s\\%04d%s", GetDocumentPath(), email, m_downBuyItem->m_itemID, ext.t);
					if(downloadFile(m_downBuyItem->m_pakFile.t, file, DownCompletePak, m_downBuyItem->m_itemID, false)){
					}
				}
			}
			break;
		}
	}
	if(m_downDetailItem || m_downBuyItem)
	{
		m_downDetailItem = m_downBuyItem = 0;
		this->Invalidate();
	}

	CDialogEx::OnLButtonUp(nFlags, point);
}


void CDlgResShop::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	this->m_nSelect = m_tab.GetCurSel();
	*pResult = 0;
}


void CDlgResShop::OnBnClickedRefresh()
{
	if(CheckLogin())
	{
		IDCJsonObject obj;
		obj["key"] = GetHttpKey();
		Request("pvd_itemlist", obj, OnItemListCallback, true);
		UpdateTitle(Language(L"搜索物品信息..."));//更新标题信息，资源超市+...
		GetDlgItem(IDC_REFRESH)->EnableWindow(FALSE);
	}
}


void CDlgResShop::OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt)
{

	CDialogEx::OnMouseHWheel(nFlags, zDelta, pt);
}


BOOL CDlgResShop::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	int minpos;
	int maxpos;
	GetScrollRange(SB_VERT, &minpos, &maxpos);
	int curpos = GetScrollPos(SB_VERT);
	if(zDelta>0)
		curpos -= 50;
	else
		curpos += 50;
	if(curpos < minpos)
		curpos = minpos;
	else if(curpos >= maxpos)
		curpos	=	maxpos;
	SetScrollPos(SB_VERT, curpos);
	m_nBeginY	=	curpos;
	this->Resize();
	return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}
