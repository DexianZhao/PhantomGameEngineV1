// PlotCallViewer.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "PlotCallViewer.h"
#include "afxdialogex.h"
#include "phantom-gc.h"
#include "GCGameProject.h"
//#include "LuaScriptEditor.h"

// CPlotCallViewer 对话框

IMPLEMENT_DYNAMIC(CPlotCallViewer, CDialogBase)

CPlotCallViewer::CPlotCallViewer(CWnd* pParent /*=NULL*/)
	: CDialogBase(CPlotCallViewer::IDD, pParent)
{
	m_plotCall	=	0;
	m_gamePtr	=	0;
}

CPlotCallViewer::~CPlotCallViewer()
{
}

void CPlotCallViewer::DoDataExchange(CDataExchange* pDX)
{
	CDialogBase::DoDataExchange(pDX);
	//	DDX_Control(pDX, IDC_MODIFY, m_edRename);
	DDX_Control(pDX, IDC_ICON_LIST, m_listCall);
	DDX_Control(pDX, IDC_INSERT_VARIANTSET, m_btVariantSet);
	DDX_Control(pDX, IDC_INSERT_PROPERTYCALL, m_btPropertyCall);
	DDX_Control(pDX, IDC_INSERT_LUASCRIPT, m_btLuaScript);
	DDX_Control(pDX, IDC_REMOVE_SELECT, m_btRemove);
}


BEGIN_MESSAGE_MAP(CPlotCallViewer, CDialogBase)
	ON_BN_CLICKED(IDOK, &CPlotCallViewer::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CPlotCallViewer::OnBnClickedCancel)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_SIZE()
	ON_LBN_SELCHANGE(IDC_ICON_LIST, &CPlotCallViewer::OnLbnSelchangeIconList)
	ON_BN_CLICKED(IDC_INSERT_VARIANTSET, &CPlotCallViewer::OnInsertVariantSet)
	ON_BN_CLICKED(IDC_INSERT_PROPERTYCALL, &CPlotCallViewer::OnInsertPropCall)
	ON_BN_CLICKED(IDC_INSERT_LUASCRIPT, &CPlotCallViewer::OnInserScript)
	ON_BN_CLICKED(IDC_REMOVE_SELECT, &CPlotCallViewer::OnRemove)
END_MESSAGE_MAP()

// CPlotCallViewer 消息处理程序

VOID			CPlotCallViewer::SetPlotCall(CPlotState* c)
{
	m_plotCall	=	c;
	int sel		=	m_listCall.GetCurSel();
	this->m_listCall.ResetContent();
	if(m_plotCall)
	{
		for(int i=0;i<m_plotCall->GetCallCount();i++)
		{
			CCallBase*	base	=	m_plotCall->GetCallPtr(i);
			switch(base->GetCallType())
			{
			case CCallBase::CallType_PropertyCall:
				{
					CCallPropertySet* propSet	=	static_cast<CCallPropertySet*>(base);
					//
					std::wstring str;
					str	=	L"Call:";
					wchar_t buf[1024];
					CPlotPropertiesMap* map	=	m_plotCall->GetProjectPtr()->SearchPropertiesMap(propSet->m_propertiesMapID);
					CPlotProperties* prop	=	map ? map->GetPropertyPtrByID(propSet->m_propertyCall) : 0;
					if(map)
					{
						swprintf(buf, L"%s", map->GetName());
						str	+=	buf;
					}
					if(prop)
					{
						swprintf(buf, L".%s", prop->GetName());
						str	+=	buf;
					}
					str	+=	L"(";
					if(prop)
					{
						for(int p=0;p<propSet->m_paramCount;p++)
						{
							if(p > 0)
								str	+=	L",";
							PlotValueList* list	=	map->GetPropParameter(propSet->m_propertyCall, p);//prop->GetParameterDefine(p);
							if(propSet->m_params[p].otherVariantID > 0)
							{
								CPlotVariant* otherVarPtr	=	m_plotCall->GetProjectPtr()->SearchVariant(propSet->m_params[p].otherVariantID);//c->GetProjectPtr()->SearchVariant(variantID);
								if(otherVarPtr)
								{
									swprintf(buf, L".%s", otherVarPtr->GetName());
									str	+=	buf;
								}
							}
							else
							{
								switch(list->variantType)
								{
								case VariantType_Double:
									{
										swprintf(buf, L"%.01f", propSet->m_params[p].value.getDouble());
										str	+=	buf;
									}
									break;
								case VariantType_Text:
									{
										Text text	=	propSet->m_params[p].value.getText();
										if(text.size() > 30)
										{
											text.texts.SetArrayCount(30);
											text.texts[27] = '.';
											text.texts[28] = '.';
											text.texts[29] = '.';
										}
										swprintf(buf, L"%s", W(text).buf);
										str	+=	buf;
									}
									break;
								case VariantType_Int:
									{
										bool bExistOther	=	false;
										int nValue	=	propSet->m_params[p].value.getInt();
										switch(list->pvlType)
										{
										case PlotValueList::PVL_None:
											{
												if(list->values.size() > 0)
												{
													for(int j=0;j<list->values.size();j++)
													{
														if(list->values[j].value == nValue)
														{
															str	+=	(const wchar_t*)list->values[j].name;
															break;
														}
													}
												}
												else
												{
													swprintf(buf, L"%d", nValue);
													str	+=	buf;
													bExistOther	=	true;
												}
											}
											break;
										case PlotValueList::PVL_DramaList:
											{
												for(int j=0;j<map->GetChapterPtr()->GetDramaCount();j++)
												{
													if(map->GetChapterPtr()->GetDramaPtr(j)->GetID() == nValue)
													{
														str	+=	Language(L"篇章：");
														str	+=	map->GetChapterPtr()->GetDramaPtr(j)->GetName();
													}
												}
											}
											break;
										case PlotValueList::PVL_ChapterList:
											{
												for(int j=0;j<map->GetProjectPtr()->GetChapterCount();j++)
												{
													if(map->GetProjectPtr()->GetChapterPtr(j)->GetID() == nValue)
													{
														str	+=	Language(L"章节：");
														str	+=	map->GetProjectPtr()->GetChapterPtr(j)->GetName();
													}
												}
											}
											break;
										case PlotValueList::PVL_ActionList:
											{
												CDynamicArray<CPlotAction*> rets;
												map->GetProcessPtr()->GetAllPlotActions(rets);
												for(int j=0;j<rets.size();j++)
												{
													if(rets[j]->GetID() == nValue)
													{
														str	+=	rets[j]->GetName();
													}
												}
											}
											break;
										case PlotValueList::PVL_SelectUIControl:
											{
												if(map->GetPlotType() == PlotType_ElementUI)
												{
													CPlotElementUI* ui	=	static_cast<CPlotElementUI*>(map);
													int value	=	0;
													for(int j=0;j<ui->m_controls.size();j++)
													{
														if(ui->m_controls[j].value == nValue)
														{
															str	+=	ui->m_controls[j].name.str();
														}
													}
												}
											}
											break;
										case PlotValueList::PVL_VariantList:
											{
												CDynamicArray<CPlotVariant*> rets;
												map->GetProcessPtr()->GetAllPlotVariants(rets);
												for(int j=0;j<rets.size();j++)
												{
													if(rets[j]->GetID() == nValue)
													{
														str	+=	rets[j]->GetName();
													}
												}
											}
											break;
										case PlotValueList::PVL_CallList:
											{
												CDynamicArray<CPlotState*> vars;
												CPlotAction* action	=	map->GetActionPtr();
												if(action)
												{
													int value	=	0;
													for(int j=0;j<action->GetStateCount();j++)
													{
														if(action->GetStatePtr(j)->GetID() == nValue)
															str	+=	action->GetStatePtr(j)->GetName();
													}
												}
											}
											break;
										case PlotValueList::PVL_Color:
											{
												color4 clr(nValue);
												swprintf(buf, L"RGBa[%.0f,%.0f,%.0f,%.0f]", clr.r * 255.0f, clr.g * 255.0f, clr.b * 255.0f, clr.a * 255.0f);
												str	+=	buf;
											}
											break;
										}
									}
									break;
								}
							}
						}
					}
					str	+=	L")";
					m_listCall.AddString(str.c_str(), 2);
				}
				break;
			case CCallBase::CallType_VariantSet:
				{
					CCallVariantSet* varSet	=	static_cast<CCallVariantSet*>(base);
					//
					std::wstring str;
					str	=	L"Set:";
					wchar_t buf[1024];

					CPlotVariant* varPtr	=	m_plotCall->GetProjectPtr()->SearchVariant(varSet->m_variantSetID);//c->GetProjectPtr()->SearchVariant(variantID);
					if(varPtr)
					{
						swprintf(buf, L".%s", varPtr->GetName());
						str	+=	buf;
					}
					str	+=	L"(";
					if(varSet->m_value.otherVariantID > 0)
					{
						CPlotVariant* otherVarPtr	=	m_plotCall->GetProjectPtr()->SearchVariant(varSet->m_value.otherVariantID);//c->GetProjectPtr()->SearchVariant(variantID);
						if(otherVarPtr)
						{
							swprintf(buf, L".%s", otherVarPtr->GetName());
							str	+=	buf;
						}
					}
					else
					{
						Text text	=	varSet->m_value.value.getText();
						if(text.size() > 30)
						{
							text.texts.SetArrayCount(30);
							text.texts[27] = '.';
							text.texts[28] = '.';
							text.texts[29] = '.';
						}
						swprintf(buf, L"%d,%.01f,%s", varSet->m_value.value.getInt(), varSet->m_value.value.getDouble(), W(text).buf);
						str	+=	buf;
						swprintf(buf, L"[%d,%d,%d,%d]", varSet->m_value.value.m_i4.left, varSet->m_value.value.m_i4.top, varSet->m_value.value.m_i4.right, varSet->m_value.value.m_i4.bottom);
						str	+=	buf;
						swprintf(buf, L"[%.01f,%01f,%01f,%01f]", varSet->m_value.value.m_f4.x, varSet->m_value.value.m_f4.y, varSet->m_value.value.m_f4.z, varSet->m_value.value.m_f4.w);
						str	+=	buf;
					}
					str	+=	L")";
					m_listCall.AddString(str.c_str(), 3);
				}
				break;
			//case CCallBase::CallType_Script:
			//	{
			//		CCallLuaScript* script	=	static_cast<CCallLuaScript*>(base);
			//		std::wstring str;
			//		str	=	L"Script:";
			//		wchar_t buf[128];
			//		memset(buf, 0,sizeof(buf));
			//		str	+=	L"(";
			//		if(script)
			//		{
			//			wcsncpy(buf, W(script->GetScriptText()), 100);
			//			str	+=	buf;
			//		}
			//		str	+=	L")";
			//		m_listCall.AddString(str.c_str(), 1);
			//	}
			//	break;
			}
		}
	}
	if(sel >= m_listCall.GetCount())
		sel	=	-1;
	m_listCall.SetCurSel(sel);
}


extern	VOID	RenderPlotBackground(HDC dcDest, const CRect& rcDest, CImage& imgBg, CSize wrapSize, const CSize& clipSize);

BOOL CPlotCallViewer::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(&rc);
	pDC->FillRect(&CRect(0, 0, rc.right, 16), &CBrush(RGB(192,192,192)));
	return TRUE;
}


void CPlotCallViewer::OnLButtonDown(UINT nFlags, CPoint point)
{
	CDialogBase::OnLButtonDown(nFlags, point);
}


void CPlotCallViewer::OnSize(UINT nType, int cx, int cy)
{
	CRect rc;
	GetClientRect(&rc);
	if(m_listCall.m_hWnd)
	{
		const int height	=	16;
		m_listCall.MoveWindow(&CRect(0, height, rc.right, rc.bottom));
	}
	CDialogBase::OnSize(nType, cx, cy);
}


BOOL CPlotCallViewer::OnInitDialog()
{
	CDialogBase::OnInitDialog();
	CRect rectDummy;
	GetClientRect(&rectDummy);
	m_listCall.MoveWindow(&rectDummy);//.Create(rectDummy, this))
	m_listCall.m_bShowIndex	=	true;
	m_listCall.SetIconRes(IDB_PLOT_CALL_ICON);
	m_listCall.SetFont(GetFont());
	m_imageButton.LoadFromResource(AfxGetResourceHandle(), IDB_PROPERTIES_PLOTCALL);
	CPoint pos(10, 0);
	this->m_btVariantSet.SetImage(this->m_imageButton, 2, -1, 4, &pos);
	this->m_btPropertyCall.SetImage(this->m_imageButton, 2, -1, 2, &pos);
	//this->m_btLuaScript.SetImage(this->m_imageButton, 2, -1, 1, &pos);
	this->m_btRemove.SetImage(this->m_imageButton, 2, -1, 3, &pos);
	m_btLuaScript.ShowWindow(SW_HIDE);
	AddTool(m_btVariantSet, Language(L"插入一个变量设置方式"));
	AddTool(m_btPropertyCall, Language(L"插入一个属性调用方式"));
	AddTool(m_btLuaScript, Language(L"插入一个Lua脚本代码段"));
	AddTool(m_btRemove, Language(L"删除一个处理过程"));

	return TRUE;
}

void CPlotCallViewer::OnInserScript()
{
	if(!m_plotCall)
		return;
	//m_plotCall->AddCall(CCallBase::CallType_Script, m_listCall.GetCurSel() + 1);
	//this->SetPlotCall(m_plotCall);
	//m_gamePtr->SetModifyed();
}
void CPlotCallViewer::OnInsertVariantSet()
{
	if(!m_plotCall)
		return;
	m_plotCall->AddCall(CCallBase::CallType_VariantSet, m_listCall.GetCurSel() + 1);
	this->SetPlotCall(m_plotCall);
	m_gamePtr->SetModifyed();
}
void CPlotCallViewer::OnInsertPropCall()
{
	if(!m_plotCall)
		return;
	m_plotCall->AddCall(CCallBase::CallType_PropertyCall, m_listCall.GetCurSel() + 1);
	this->SetPlotCall(m_plotCall);
	m_gamePtr->SetModifyed();
}
void CPlotCallViewer::OnRemove()
{
	if(!m_plotCall)
		return;
	if(m_gamePtr->IsGameRuntime())
	{
		MessageBox(Language(L"游戏运行模式下不能删除任何对象"), Language(L"提示"), MB_OK);
		return;
	}
	if(m_listCall.GetCurSel() >= 0)
	{
		m_plotCall->RemoveCall(m_plotCall->GetCallPtr(m_listCall.GetCurSel()));
		this->SetPlotCall(m_plotCall);
		m_gamePtr->SetModifyed();
	}
}


void CPlotCallViewer::OnLbnSelchangeIconList()
{
	int sel	=	m_listCall.GetCurSel();
	if(!m_plotCall)
		return;
	CCallBase* c	=	m_plotCall->GetCallPtr(sel);
	if(!c)
	{
		m_gamePtr->SetPropertyCallPropCall(0, 0);
		return;
	}
	switch(c->GetCallType())
	{
	case CCallBase::CallType_VariantSet:
		m_gamePtr->SetPropertyCallVarSet(m_plotCall, static_cast<CCallVariantSet*>(c));
		break;
	case CCallBase::CallType_PropertyCall:
		m_gamePtr->SetPropertyCallPropCall(m_plotCall, static_cast<CCallPropertySet*>(c));
		break;
	//case CCallBase::CallType_Script:
	//	m_gamePtr->SetPropertyCallLuaScript(m_plotCall, static_cast<CCallLuaScript*>(c));
	//	break;
	}
}

void CPlotCallViewer::OnBnClickedOk()
{
}

void CPlotCallViewer::OnBnClickedCancel()
{
}

