// ScreenEffectEditor.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "ScreenEffectEditor.h"


// CScreenEffectEditor 对话框

IMPLEMENT_DYNAMIC(CScreenEffectEditor, CDialogBase)

CScreenEffectEditor::CScreenEffectEditor(CWnd* pParent /*=NULL*/)
	: CDialogBase(CScreenEffectEditor::IDD, pParent)
{

}

CScreenEffectEditor::~CScreenEffectEditor()
{
}

void CScreenEffectEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_METHOD_LIST, m_lbMethods);
	DDX_Control(pDX, IDC_POST_ITEMS, m_lbPosts);
	DDX_Control(pDX, IDC_QRENDER_LIST, m_cbQPosts);
	DDX_Control(pDX, IDC_METHOD_ENABLE, m_chkPostEnable);
	DDX_Control(pDX, IDC_METHOD_NAME, m_edMethodName);
	DDX_Control(pDX, IDC_METHOD_VARIANT_VALUE, m_lbVariants);
	DDX_Control(pDX, IDC_VARIANT_VALUE, m_edX);
	DDX_Control(pDX, IDC_VARIANT_VALUE2, m_edY);
	DDX_Control(pDX, IDC_VARIANT_VALUE3, m_edZ);
	DDX_Control(pDX, IDC_VARIANT_VALUE4, m_edW);
	DDX_Control(pDX, IDC_SCROLLBAR_X, m_sbX);
	DDX_Control(pDX, IDC_SCROLLBAR_Y, m_sbY);
	DDX_Control(pDX, IDC_SCROLLBAR_Z, m_sbZ);
	DDX_Control(pDX, IDC_SCROLLBAR_W, m_sbW);
	DDX_Control(pDX, IDC_TEXTURE_LIST, m_lbTextures);
	DDX_Control(pDX, IDC_TEXTURE_FILE, m_edTextureFile);
	DDX_Control(pDX, IDC_SUPPORTs, m_cbSupports);
}


BEGIN_MESSAGE_MAP(CScreenEffectEditor, CDialogBase)
	ON_BN_CLICKED(IDC_LOAD_METHOD_LIST, &CScreenEffectEditor::OnBnClickedLoadMethodList)
	ON_BN_CLICKED(IDC_SAVE_METHOD_LIST, &CScreenEffectEditor::OnBnClickedSaveMethodList)
	ON_BN_CLICKED(IDC_SAVE_METHOD_LIST2, &CScreenEffectEditor::OnBnClickedSaveMethodList2)
	ON_BN_CLICKED(IDC_ADD_METHOD, &CScreenEffectEditor::OnBnClickedAddMethod)
	ON_BN_CLICKED(IDC_DEL_METHOD, &CScreenEffectEditor::OnBnClickedDelMethod)
	ON_BN_CLICKED(IDC_SET_ACTIVE, &CScreenEffectEditor::OnBnClickedSetActive)
	ON_LBN_SELCHANGE(IDC_METHOD_LIST, &CScreenEffectEditor::OnLbnSelchangeMethodList)
	ON_LBN_SELCHANGE(IDC_POST_ITEMS, &CScreenEffectEditor::OnLbnSelchangePostItems)
	ON_BN_CLICKED(IDC_ADD_METHOD_ITEM, &CScreenEffectEditor::OnBnClickedAddMethodItem)
	ON_BN_CLICKED(IDC_DELETE_METHOD_ITEM, &CScreenEffectEditor::OnBnClickedDeleteMethodItem)
	ON_BN_CLICKED(IDC_METHOD_ENABLE, &CScreenEffectEditor::OnBnClickedMethodEnable)
	ON_BN_CLICKED(IDC_SET_METHOD_NAME, &CScreenEffectEditor::OnBnClickedSetMethodName)
	ON_LBN_SELCHANGE(IDC_METHOD_VARIANT_VALUE, &CScreenEffectEditor::OnLbnSelchangeMethodVariantValue)
	ON_BN_CLICKED(IDC_SET_VARIANT_NAME, &CScreenEffectEditor::OnBnClickedSetVariantName)
	ON_LBN_SELCHANGE(IDC_TEXTURE_LIST, &CScreenEffectEditor::OnLbnSelchangeTextureList)
	ON_BN_CLICKED(IDC_SET_TEXTURE, &CScreenEffectEditor::OnBnClickedSetTexture)
	ON_CBN_SELCHANGE(IDC_POST_LAYER, &CScreenEffectEditor::OnCbnSelchangePostLayer)
	ON_BN_CLICKED(IDC_CLONE_METHOD, &CScreenEffectEditor::OnBnClickedCloneMethod)
	ON_WM_SHOWWINDOW()
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_UPPER, &CScreenEffectEditor::OnBnClickedUpper)
	ON_BN_CLICKED(IDC_DOWNLER, &CScreenEffectEditor::OnBnClickedDownler)
	ON_CBN_SELCHANGE(IDC_QRENDER_LIST, &CScreenEffectEditor::OnCbnSelchangeQrenderList)
	ON_BN_CLICKED(IDC_ADD_METHOD_ITEM2, &CScreenEffectEditor::OnBnClickedAddMethodItem2)
END_MESSAGE_MAP()


// CScreenEffectEditor 消息处理程序

void CScreenEffectEditor::OnBnClickedLoadMethodList()
{
	CFileDialog fdlg(TRUE, 0,0, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, L"*.posts;*.posts;|*.posts;*.posts|*.*|*.*||", this);
	if(fdlg.DoModal()!=IDOK)return;
	SetProjectPath();
	GetScreenEffectPtr()->LoadMethods(0, A(fdlg.GetPathName()));
	m_strFileName	=	fdlg.GetPathName();
	this->RefreshMethods();
}

void CScreenEffectEditor::OnBnClickedSaveMethodList()
{
	SetProjectPath();
	GetScreenEffectPtr()->SaveMethods("./effects.posts");
	MessageBox(Language(L"保存成功"));
}

void CScreenEffectEditor::OnBnClickedSaveMethodList2()
{
	CFileDialog fdlg(FALSE, 0,0, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, L"*.posts;*.posts;|*.posts;*.posts|*.*|*.*||", this);
	if(fdlg.DoModal()!=IDOK)return;
	SetProjectPath();
	wchar_t szBuffer[255], szDir[255], szFile[255];
	_wsplitpath(fdlg.GetPathName(), szBuffer, szDir, szFile, 0);
	wcscat(szBuffer, szDir);
	wcscat(szBuffer, szFile);
	wcscat(szBuffer, L".posts");
	m_strFileName	=	szBuffer;
	GetScreenEffectPtr()->SaveMethods(A(szBuffer));
	MessageBox(Language(L"保存成功"));
}

void CScreenEffectEditor::RefreshMethods()
{
	int sel = 0;
	if(m_lbMethods.GetCurSel() >= 0)
		sel = m_lbMethods.GetItemData(m_lbMethods.GetCurSel());
	this->m_lbMethods.ResetContent();
	m_lbMethods.SetItemData(m_lbMethods.AddString(Language(L"无效果")), 0);
	for(int i=0;i<GetScreenEffectPtr()->GetMethodCount();i++)
	{
		IScreenEffectMethod* method	=	GetScreenEffectPtr()->GetMethodPtr(i);
		int index	=	m_lbMethods.AddString(W(method->GetMethodName()));
		m_lbMethods.SetItemData(index, (DWORD_PTR)method);
		if((INT)method == sel)//&& GetScreenEffectPtr()->GetActiveMethod() == method)
			m_lbMethods.SetCurSel(index);
	}
	OnLbnSelchangeMethodList();
}

void CScreenEffectEditor::OnBnClickedAddMethod()
{
	GetScreenEffectPtr()->AddMethod();
	this->RefreshMethods();
}

void CScreenEffectEditor::OnBnClickedDelMethod()
{
	if(m_lbMethods.GetCurSel() < 0)return;
	IScreenEffectMethod* method	=	(IScreenEffectMethod*)m_lbMethods.GetItemData(m_lbMethods.GetCurSel());
	if(!method)return;
	GetScreenEffectPtr()->RemoveMethod(method);
	this->RefreshMethods();
}

void CScreenEffectEditor::OnBnClickedSetActive()
{
	if(m_lbMethods.GetCurSel() < 0)return;
	IScreenEffectMethod* method	=	(IScreenEffectMethod*)m_lbMethods.GetItemData(m_lbMethods.GetCurSel());
	GetScreenEffectPtr()->ActiveMethod(method);
}

void CScreenEffectEditor::OnLbnSelchangeMethodList()
{
	if(m_lbMethods.GetCurSel() < 0)return;
	IScreenEffectMethod* method	=	(IScreenEffectMethod*)m_lbMethods.GetItemData(m_lbMethods.GetCurSel());
	if(!method)
		return;
	m_cbSupports.SetCurSel(0);
	for(int i=0;i<this->m_cbSupports.GetCount();i++)
	{
		if(m_cbSupports.GetItemData(i) == method->GetSupport())
		{
			m_cbSupports.SetCurSel(i);
			break;
		}
	}
	this->m_edMethodName.SetWindowText(W(method->GetMethodName()));
	//
	RefreshPosts();
}

void CScreenEffectEditor::RefreshPosts()
{
	if(m_lbMethods.GetCurSel() < 0)return;
	IScreenEffectMethod* method	=	(IScreenEffectMethod*)m_lbMethods.GetItemData(m_lbMethods.GetCurSel());
	if(!method)return;
	int sels[512];
	int count	=	m_lbPosts.GetSelItems(512, sels);
	std::vector<IPostProcess*> selects;
	for(int i=0;i<count;i++)
	{
		IPostProcess* post	=	(IPostProcess*)m_lbPosts.GetItemData(sels[i]);
		selects.push_back(post);
	}
	IPostProcess* post	=	0;
	if(m_lbPosts.GetCurSel() >= 0)
		post	=	(IPostProcess*)m_lbPosts.GetItemData(this->m_lbPosts.GetCurSel());
	m_lbPosts.ResetContent();
	for(int i=0;i<method->GetPostCount();i++)
	{
		int index	=	m_lbPosts.AddString(W(method->GetPostPtr(i)->GetPostName()));
		m_lbPosts.SetItemData(index, (DWORD_PTR)method->GetPostPtr(i));
		if(post == method->GetPostPtr(i))
			m_lbPosts.SetCurSel(index);
		for(int k=0;k<selects.size();k++)
		{
			if(selects[k] == method->GetPostPtr(i))
			{
				m_lbPosts.SetSel(index, true);
			}
		}
	}
	this->OnLbnSelchangePostItems();
}

void CScreenEffectEditor::OnLbnSelchangePostItems()
{
	if(m_lbPosts.GetCurSel() < 0)return;
	int po	=	m_lbPosts.GetItemData(m_lbPosts.GetCurSel());
	if(po < 0)
		return;
	IPostProcess* pPost	=	(IPostProcess*)po;
	//
	W postname(pPost->GetPostName());
	this->m_chkPostEnable.SetCheck(pPost->IsEnabled());
	for(int i=0;i<m_cbQPosts.GetCount();i++)
	{
		if(stricmp(GetScreenEffectPtr()->GetQRenderName(m_cbQPosts.GetItemData(i)), pPost->GetPostName()) == 0)
		{
			m_cbQPosts.SetCurSel(i);
			break;
		}
	}
	RefreshVariants();
	RefreshTextures();
}

void CScreenEffectEditor::RefreshVariants()
{
	if(m_lbPosts.GetCurSel() < 0)return;
	IPostProcess* pPost	=	(IPostProcess*)m_lbPosts.GetItemData(m_lbPosts.GetCurSel());
	int sel = -1;
	if(m_lbVariants.GetCurSel() >= 0)
		sel = m_lbVariants.GetItemData(m_lbVariants.GetCurSel());
	m_lbVariants.ResetContent();
	for(int i=0;i<pPost->GetVariantCount();i++)
	{
		int index	=	m_lbVariants.AddString(W(pPost->GetVariantPtr(i)->desc));
		m_lbVariants.SetItemData(index, (DWORD_PTR)i);
		if(i == sel)
			m_lbVariants.SetCurSel(index);
	}
	OnLbnSelchangeMethodVariantValue();
}

void CScreenEffectEditor::RefreshTextures()
{
	if(m_lbPosts.GetCurSel() < 0)return;
	IPostProcess* pPost	=	(IPostProcess*)m_lbPosts.GetItemData(m_lbPosts.GetCurSel());
	int sel = -1;
	if(m_lbTextures.GetCurSel() >= 0)
		sel = m_lbTextures.GetItemData(m_lbTextures.GetCurSel());
	m_lbTextures.ResetContent();
	for(int i=0;i<pPost->GetTextureCount();i++)
	{
		int index	=	m_lbTextures.AddString(W(pPost->GetTexture(i)->textureVar.name));
		m_lbTextures.SetItemData(index, (DWORD_PTR)i);
		if(i == sel)
			m_lbTextures.SetCurSel(index);
	}
	OnLbnSelchangeTextureList();
}

void CScreenEffectEditor::OnBnClickedAddMethodItem()
{
	if(m_lbMethods.GetCurSel() < 0)return;
	IScreenEffectMethod* method	=	(IScreenEffectMethod*)m_lbMethods.GetItemData(m_lbMethods.GetCurSel());
	if(!method)return;
	if(m_cbQPosts.GetCurSel() < 0)
		return;
	method->AddPostProcess(GetScreenEffectPtr()->GetQRenderName(m_cbQPosts.GetItemData(m_cbQPosts.GetCurSel())));
	this->RefreshPosts();
}

void CScreenEffectEditor::OnBnClickedDeleteMethodItem()
{
	if(m_lbMethods.GetCurSel() < 0)return;
	IScreenEffectMethod* method	=	(IScreenEffectMethod*)m_lbMethods.GetItemData(m_lbMethods.GetCurSel());
	if(!method)return;
	int sels[512];
	int count	=	m_lbPosts.GetSelItems(512, sels);
	//
	for(int i=0;i<count;i++)
	{
		IPostProcess* pPost	=	(IPostProcess*)m_lbPosts.GetItemData(sels[i]);
		method->RemovePostProcess(pPost);
	}
	this->RefreshPosts();
}

void CScreenEffectEditor::OnBnClickedMethodEnable()
{
	if(m_lbPosts.GetCurSel() < 0)return;
	int sels[512];
	int count	=	m_lbPosts.GetSelItems(512, sels);
	//
	for(int i=0;i<count;i++)
	{
		IPostProcess* pPost	=	(IPostProcess*)m_lbPosts.GetItemData(sels[i]);
		pPost->SetEnabled(this->m_chkPostEnable.GetCheck());
	}
}

void CScreenEffectEditor::OnBnClickedSetMethodName()
{
	if(m_lbMethods.GetCurSel() < 0)return;
	IScreenEffectMethod* method	=	(IScreenEffectMethod*)m_lbMethods.GetItemData(m_lbMethods.GetCurSel());
	if(!method)return;
	CString str;
	this->m_edMethodName.GetWindowText(str);
	method->SetMethodName(A(str));
	RefreshMethods();
}

VOID	SetRange(CScrollBar& sb, float fMin, float fMax)
{
	sb.SetScrollRange(0, 1000);
}

VOID	SetPos(CScrollBar& sb, float fProgress)
{
	sb.SetScrollPos((int)(fProgress * 1000.0f));
}

float	GetPos(CScrollBar& sb, float fMin, float fMax)
{
	int pos = sb.GetScrollPos();
	return (((float)pos / 1000.0f) * (fMax - fMin) + fMin);
}

void CScreenEffectEditor::OnLbnSelchangeMethodVariantValue()
{
	if(m_lbVariants.GetCurSel() < 0)return;
	if(m_lbPosts.GetCurSel() < 0)return;
	IPostProcess* pPost	=	(IPostProcess*)m_lbPosts.GetItemData(m_lbPosts.GetCurSel());
	PostShaderVar* var	=	(PostShaderVar*)pPost->GetVariantPtr(m_lbVariants.GetItemData(m_lbVariants.GetCurSel()));
	wchar_t buf[256];
	swprintf(buf, L"%.05f", var->value.x);
	this->m_edX.SetWindowText(buf);
	swprintf(buf, L"%.05f", var->value.y);
	this->m_edY.SetWindowText(buf);
	swprintf(buf, L"%.05f", var->value.z);
	this->m_edZ.SetWindowText(buf);
	swprintf(buf, L"%.05f", var->value.w);
	this->m_edW.SetWindowText(buf);
	//
	SetRange(m_sbX, var->fMin, var->fMax);
	SetPos(m_sbX, (var->value.x - var->fMin) / (var->fMax - var->fMin));
	//
	SetRange(m_sbY, var->fMin, var->fMax);
	SetPos(m_sbY, (var->value.y - var->fMin) / (var->fMax - var->fMin));
	//
	SetRange(m_sbZ, var->fMin, var->fMax);
	SetPos(m_sbZ, (var->value.z - var->fMin) / (var->fMax - var->fMin));
	//
	SetRange(m_sbW, var->fMin, var->fMax);
	SetPos(m_sbW, (var->value.w - var->fMin) / (var->fMax - var->fMin));
}

void CScreenEffectEditor::OnBnClickedSetVariantName()
{
	if(m_lbMethods.GetCurSel() < 1)return;
	if(m_lbPosts.GetCurSel() < 0)return;
	if(m_lbVariants.GetCurSel() < 0)return;
	IPostProcess* pPost	=	(IPostProcess*)m_lbPosts.GetItemData(m_lbPosts.GetCurSel());
	PostShaderVar* var	=	(PostShaderVar*)pPost->GetVariantPtr(m_lbVariants.GetItemData(m_lbVariants.GetCurSel()));
	CString szX,szY,szZ,szW;
	m_edX.GetWindowText(szX);
	m_edY.GetWindowText(szY);
	m_edZ.GetWindowText(szZ);
	m_edW.GetWindowText(szW);
	//
	var->value.setXYZW(_wtof(szX), _wtof(szY), _wtof(szZ), _wtof(szW));
	//
	pPost->SaveValues();
}

void CScreenEffectEditor::OnLbnSelchangeTextureList()
{
	if(m_lbTextures.GetCurSel() < 0)return;
	if(m_lbPosts.GetCurSel() < 0)return;
	IPostProcess* pPost	=	(IPostProcess*)m_lbPosts.GetItemData(m_lbPosts.GetCurSel());
	PostTexture* var	=	pPost->GetTexture(m_lbTextures.GetItemData(m_lbTextures.GetCurSel()));
	this->m_edTextureFile.SetWindowText(W(var->textureFile));
}

void CScreenEffectEditor::OnBnClickedSetTexture()
{
	// TODO: 在此添加控件通知处理程序代码
}

void CScreenEffectEditor::OnCbnSelchangePostLayer()
{
	if(m_lbPosts.GetCurSel() < 0)return;
	//IPostProcess* pPost	=	(IPostProcess*)m_lbPosts.GetItemData(m_lbPosts.GetCurSel());
	//pPost->SetPostLayer(this->m_cbPostLayer.GetItemData(m_cbPostLayer.GetCurSel()));
	this->RefreshPosts();
}

void CScreenEffectEditor::OnBnClickedCloneMethod()
{
	if(this->m_lbMethods.GetCurSel()<0)return;
	IScreenEffectMethod* method = (IScreenEffectMethod*)m_lbMethods.GetItemData(m_lbMethods.GetCurSel());
	if(method)
	{
		GetScreenEffectPtr()->AddMethod(method);
		this->RefreshMethods();
	}
}

void CScreenEffectEditor::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogBase::OnShowWindow(bShow, nStatus);

	if(bShow && GetScreenEffectPtr())
	{
		m_cbQPosts.ResetContent();
		for(int i=0;i<GetScreenEffectPtr()->GetQRenderCount();i++)
		{
			m_cbQPosts.SetItemData(m_cbQPosts.AddString(W(GetScreenEffectPtr()->GetQRenderName(i))), i);
		}
		RefreshMethods();
	}
}

void CScreenEffectEditor::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if(pScrollBar)
	{
		int minpos;
		int maxpos;
		pScrollBar->GetScrollRange(&minpos, &maxpos); 
		maxpos = pScrollBar->GetScrollLimit();
		int curpos = pScrollBar->GetScrollPos();
		switch (nSBCode)
		{
		case SB_LEFT:
			curpos = minpos;
			break;
		case SB_RIGHT:
			curpos = maxpos;
			break;
		case SB_ENDSCROLL:   // End scroll.
			break;
		case SB_LINELEFT:      // Scroll left.
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
				pScrollBar->GetScrollInfo(&info, SIF_ALL);
				if (curpos > minpos)
					curpos = max(minpos, curpos - (int) info.nPage);
			}
			break;
		case SB_PAGERIGHT:      // Scroll one page right.
			{
				SCROLLINFO   info;
				pScrollBar->GetScrollInfo(&info, SIF_ALL);
				if (curpos < maxpos)
					curpos = min(maxpos, curpos + (int) info.nPage);
			}
			break;
		case SB_THUMBPOSITION: // Scroll to absolute position. nPos is the position
			curpos = nPos;      // of the scroll box at the end of the drag operation.
			break;
		case SB_THUMBTRACK:   // Drag scroll box to specified position. nPos is the
			curpos = nPos;     // position that the scroll box has been dragged to.
			break;
		}
		int nmin, nmax;
		pScrollBar->GetScrollRange(&nmin, &nmax);
		if(curpos < nmin)
			curpos = nmin;
		else if(curpos > nmax)
			curpos = nmax;
		pScrollBar->SetScrollPos(curpos);
		//
		if(m_lbMethods.GetCurSel() < 1)return;
		int sels[512];
		int count	=	m_lbPosts.GetSelItems(512, sels);
		//
		if(m_lbVariants.GetCurSel() < 0)return;
		for(int i=0;i<count;i++)
		{
			IPostProcess* pPost	=	(IPostProcess*)m_lbPosts.GetItemData(sels[i]);
			//	pPost->SetEnabled(this->m_chkPostEnable.GetCheck());
			//}
			//if(m_lbPosts.GetCurSel() < 0)return;
			//IPostProcess* pPost	=	(IPostProcess*)m_lbPosts.GetItemData(m_lbPosts.GetCurSel());
			PostShaderVar* var	=	(PostShaderVar*)pPost->GetVariantPtr(m_lbVariants.GetItemData(m_lbVariants.GetCurSel()));
			if(!var)
				continue;
			if(pScrollBar == &m_sbX)
				var->value.x = GetPos(m_sbX, var->fMin, var->fMax);
			else if(pScrollBar == &m_sbY)
				var->value.y = GetPos(m_sbY, var->fMin, var->fMax);
			else if(pScrollBar == &m_sbZ)
				var->value.z = GetPos(m_sbZ, var->fMin, var->fMax);
			else if(pScrollBar == &m_sbW)
				var->value.w = GetPos(m_sbW, var->fMin, var->fMax);
			wchar_t buf[256];
			swprintf(buf, L"%.05f", var->value.x);
			this->m_edX.SetWindowText(buf);
			swprintf(buf, L"%.05f", var->value.y);
			this->m_edY.SetWindowText(buf);
			swprintf(buf, L"%.05f", var->value.z);
			this->m_edZ.SetWindowText(buf);
			swprintf(buf, L"%.05f", var->value.w);
			this->m_edW.SetWindowText(buf);
		}
	}
	CDialogBase::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CScreenEffectEditor::OnBnClickedUpper()
{
	if(m_lbPosts.GetCurSel() < 0)
		return;
	if(this->m_lbMethods.GetCurSel()<0)return;
	int sels[512];
	int count	=	m_lbPosts.GetSelItems(512, sels);
	std::vector<IPostProcess*> selects;
	for(int i=0;i<count;i++)
	{
		IPostProcess* post	=	(IPostProcess*)m_lbPosts.GetItemData(sels[i]);
		selects.push_back(post);
	}
	//
	IScreenEffectMethod* method = (IScreenEffectMethod*)m_lbMethods.GetItemData(m_lbMethods.GetCurSel());
	for(int i=0;i<count;i++)
	{
		IPostProcess* pPost	=	(IPostProcess*)m_lbPosts.GetItemData(sels[i]);
		method->SetPostMove(pPost, true);
	}
	this->RefreshPosts();
	for(int i=0;i<m_lbPosts.GetCount();i++)
	{
		IPostProcess* pPost	=	(IPostProcess*)m_lbPosts.GetItemData(sels[i]);
		bool sel = false;
		for(int j=0;j<selects.size();j++)
		{
			if(pPost == selects[j])
			{
				sel	=	true;
				break;
			}
		}
		m_lbPosts.SetSel(i, sel);
	}
}

void CScreenEffectEditor::OnBnClickedDownler()
{
	if(m_lbPosts.GetCurSel() < 0)
		return;
	if(this->m_lbMethods.GetCurSel()<0)return;
	int sels[512];
	int count	=	m_lbPosts.GetSelItems(512, sels);
	std::vector<IPostProcess*> selects;
	for(int i=0;i<count;i++)
	{
		IPostProcess* post	=	(IPostProcess*)m_lbPosts.GetItemData(sels[i]);
		selects.push_back(post);
	}
	//
	IScreenEffectMethod* method = (IScreenEffectMethod*)m_lbMethods.GetItemData(m_lbMethods.GetCurSel());
	for(int i=count - 1;i >= 0;i--)
	{
		IPostProcess* pPost	=	(IPostProcess*)m_lbPosts.GetItemData(sels[i]);
		method->SetPostMove(pPost, false);
	}
	this->RefreshPosts();
	for(int i=0;i<m_lbPosts.GetCount();i++)
	{
		IPostProcess* pPost	=	(IPostProcess*)m_lbPosts.GetItemData(sels[i]);
		bool sel = false;
		for(int j=0;j<selects.size();j++)
		{
			if(pPost == selects[j])
			{
				sel	=	true;
				break;
			}
		}
		m_lbPosts.SetSel(i, sel);
	}
}


BOOL CScreenEffectEditor::OnInitDialog()
{
	CDialogBase::OnInitDialog();
	m_cbSupports.SetItemData(m_cbSupports.AddString(L"None"), EffectSupport_None);
	m_cbSupports.SetItemData(m_cbSupports.AddString(L"Highlight"), EffectSupport_Highlight);
	m_cbSupports.SetItemData(m_cbSupports.AddString(L"UI"), EffectSupport_UI);
	m_cbSupports.SetItemData(m_cbSupports.AddString(L"SoftShadow"), EffectSupport_SoftShadow);
	return TRUE;
}


void CScreenEffectEditor::OnCbnSelchangeQrenderList()
{
}


void CScreenEffectEditor::OnBnClickedAddMethodItem2()
{
	if(m_lbMethods.GetCurSel() < 0)return;
	IScreenEffectMethod* method	=	(IScreenEffectMethod*)m_lbMethods.GetItemData(m_lbMethods.GetCurSel());
	if(!method)return;
	if(m_cbQPosts.GetCurSel() < 0)
		return;
	int sels[512];
	int count	=	m_lbPosts.GetSelItems(512, sels);
	for(int i=0;i<count;i++)
	{
		IPostProcess* pPost	=	(IPostProcess*)m_lbPosts.GetItemData(sels[i]);
		pPost->SetPostName(GetScreenEffectPtr()->GetQRenderName(m_cbQPosts.GetItemData(m_cbQPosts.GetCurSel())));
	}
	this->RefreshPosts();
}
