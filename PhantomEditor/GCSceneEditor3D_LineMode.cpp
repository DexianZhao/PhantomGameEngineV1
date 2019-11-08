//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	文件       :   GCSceneEditor3D.cpp
	软件名称   :   幻影游戏引擎

	设计编写者 :   赵德贤(英文:ZhaoDexian)
	Email: yuzhou_995@hotmail.com
	
	Copyright 2009-2016 Zhao Dexian
	
	-------------------------------------------------------------------------------------------------


	-------------------------------------------------------------------------------------------------
	*/
//////////////////////////////////////////////////////////////////////////////////////////////////////


#include "StdAfx.h"
#include "GCSceneEditor3D.h"
#include "MainFrm.h"
#include "PropertiesDialogWnd.h"

VOID						GCSceneEditor3D::AddToLinear(LinearData* d)
{
	//for(int i=0;i<m_selectLines.size();i++)
	//{
	//	if(m_selectLines[i].id == d->id && m_selectLines[i].sccPoint == d->sccPoint)
	//		return;
	//}
	//m_selectLines.push_back(*d);
	////
	//double x=0,y=0,z=0;
	//for(int i=0;i<this->m_selectLines.size();i++)
	//{
	//	LinearData& ld	=	m_selectLines[i];
	//	const SccPoint* p = ld.sccPoint->GetPoint(Phantom::PtrID(-1, ld.id));
	//	x	+=	p->f.x;
	//	y	+=	p->f.y;
	//	z	+=	p->f.z;
	//	ld.downPos	=	p->f;
	//	ld.newPos	=	p->f;
	//}
	//double inv = 1.0f / (double)m_selectLines.size();
	//m_axisMoving.m_viewWorld.identity();
	//m_axisMoving.m_viewWorld.SetPosition( x * inv, y * inv, z * inv );
	//m_selectCenter	=	m_axisMoving.m_viewWorld.GetPosition();
}
bool						GCSceneEditor3D::OnWindowProcLine(HWND hWnd, unsigned int uMsg, unsigned int wParam, unsigned int lParam)
{
	switch(uMsg)
	{
	case WM_LBUTTONUP:
		{
			/*POINT pos;
			GetCursorPos(&pos);
			ScreenToClient(GetDeviceWindow(), &pos);
			m_bDoEvent			=	false;
			m_bMouseMove		=	false;
			if(m_objectEditMode == ObjectEditMode_GetHeight)
			{
				for(int i=0;i<this->m_selectLines.size();i++)
				{
					LinearData& ld	=	m_selectLines[i];
					const SccPoint* p = ld.sccPoint->GetPoint(Phantom::PtrID(-1, ld.id));
					Phantom::float3 pos	=	p->f;
					if(m_scene3D->GetHeight(pos, pos.z, 0))
						ld.sccPoint->SetPoint(Phantom::PtrID(-1, ld.id), pos);
				}
			}
			else if(m_objectEditMode == ObjectEditMode_Select)
			{
				if(m_axisMoving.OnLButtonUp(pos))
				{
					m_objectEditMode	=	ObjectEditMode_None;
					this->SetModifyed();
					return true;
				}
				m_selectLines.clear();
				CRect region(min(m_downPoint.x, pos.x), min(m_downPoint.y, pos.y), max(m_downPoint.x, pos.x), max(m_downPoint.y, pos.y));
				for(int a=0;a<this->m_scene3D->GetAnimationManager().GetDataCount();a++)
				{
					ISCA_Animation* ani	=	this->m_scene3D->GetAnimationManager().GetDataPtr(a);
					for(int s = 0; s < ani->GetScaManager().GetDataCount(); s++)
					{
						ISCA_Base* base	=	ani->GetScaManager().GetDataPtr(s);
						for(int t=0;t<base->GetTrackManager().GetDataCount(); t++)
						{
							ISCA_Track* track	=	base->GetTrackManager().GetDataPtr(t);
							if(track->GetSccPosition())
							{
								for(int p=0;p<track->GetSccPosition()->GetPointCount();p++)
								{
									const SccPoint* pos	=	track->GetSccPosition()->GetPoint(p);
									Phantom::Pixel out;
									if(Phantom::GetCameraPtr()->Get2DFrom3D(out, pos->f))
									{
										if(region.PtInRect(out))
										{
											LinearData d;
											d.newPos	=	pos->f;
											d.id		=	track->GetSccPosition()->GetPointID(p);
											d.sccPoint	=	track->GetSccPosition();
											d.track		=	track;
											d.ani		=	ani;
											d.base		=	base;
											this->AddToLinear(&d);
										}
									}
								}
							}
						}
					}
				}
			}
			m_objectEditMode = ObjectEditMode_None;*/
			return true;
		}
		break;
	case WM_MBUTTONDOWN:
		{
			AutoAddRegion();
		}
		break;
	case WM_LBUTTONDOWN:
		{
			GetCursorPos(&m_downPoint);
			ScreenToClient(GetDeviceWindow(), &m_downPoint);
			//
			m_nEditRegionCount	=	0;
			m_fCalculateTime	=	0;
			m_bDoEvent			=	false;
			m_bMouseMove		=	false;
			if(m_axisMoving.OnLButtonDown(m_downPoint))
				return true;
			m_objectEditMode	=	ObjectEditMode_Select;
		}
		break;
	case WM_KEYDOWN:
		{
			//if(wParam == VK_DELETE)
			//{
			//	if(MessageBox(this->GetViewHandle(), Language(L"是否真的要删除?"), Language(L"提示"), MB_YESNO) == IDYES)
			//	{
			//		for(int i=0;i<this->m_selectLines.size();i++)
			//		{
			//			Phantom::PtrID id;
			//			id.id	=	m_selectLines[i].id;
			//			m_selectLines[i].sccPoint->RemovePoint(id);
			//			this->SetModifyed();
			//		}
			//		m_selectLines.clear();
			//	}
			//}
		}
		break;
	case WM_LBUTTONDBLCLK:
		{
			//GetCursorPos(&m_downPoint);
			//ScreenToClient(GetDeviceWindow(), &m_downPoint);
			////
			//m_nEditRegionCount	=	0;
			//m_fCalculateTime	=	0;
			//m_bDoEvent			=	false;
			//m_bMouseMove		=	false;
			//m_objectEditMode	=	ObjectEditMode_None;
			////
			//TreeItemType* type	=	GetSelectedItem();
			//if(m_axisMoving.OnLButtonDown(m_downPoint))
			//{
			//	m_objectEditMode	=	ObjectEditMode_GetHeight;
			//}
			//else if(type && type->type == this->SCENE3D_SCA_TRACK)
			//{
			//	ISCA_Animation* ani	=	m_scene3D->GetAnimationManager().SearchDataPtr(type->p1);
			//	if(ani)
			//	{
			//		ISCA_Base* base	=	ani->GetScaManager().SearchDataPtr(type->p2);
			//		if(base)
			//		{
			//			ISCA_Track* track	=	base->GetTrackManager().SearchDataPtr(type->p3);
			//			if(track && track->GetSccPosition())
			//			{
			//				Phantom::Pixel cursor;
			//				::GetCursorPos((LPPOINT)&cursor);
			//				ScreenToClient((HWND)GetDeviceWindow(), (LPPOINT)&cursor);
			//				Phantom::float3 orig, dir;
			//				Phantom::GetCameraPtr()->Get3DFrom2D(orig, dir, cursor);
			//				Phantom::PickPoint data;
			//				if(m_scene3D->IntersectTerrainOnly(orig, dir, data))
			//				{
			//					track->GetSccPosition()->AddPoint(data.pos);
			//					this->SetModifyed();
			//				}
			//				else
			//					break;
			//			}
			//		}
			//	}
			//}
			//else
			//{
			//	GetMainFrame()->MessageBox(Language(L"先选择一个控制器之后才能添加"), 0, MB_OK);
			//}
		}
		break;
	case WM_RBUTTONUP:
		{
		}
		break;
	case WM_RBUTTONDOWN:
	case WM_RBUTTONDBLCLK:
		{
		}
		break;
	case WM_MOUSEMOVE:
		{
			POINT pos;
			GetCursorPos(&pos);
			ScreenToClient(GetDeviceWindow(), &pos);
			if(m_axisMoving.OnMouseMove(pos))
				return true;
			m_bMouseMove		=	true;
			return false;
		}
	case WM_MOUSEWHEEL:
		if(IsKeyDown(VK_CONTROL))
		{
			int zDelta = (short) HIWORD(wParam) / WHEEL_DELTA;
			if(zDelta > 0)
			{
			}
			else
			{
			}
			return true;
		}
		break;
	}
	return false;
}

VOID						GCSceneEditor3D::OnFrameMoveLine()
{
	//SceneEditorInfo& info	=	this->m_editorInfo[this->m_editorMode];
	//m_scene3D->SetNormalBuildVar(info.fNormalVar);
	//Phantom::Camera* c		=	Phantom::GetCameraPtr();
	//Phantom::Pixel pixel	=	c->getCursorPos();
	//Phantom::float3 orig, dir;
	//c->Get3DFrom2D(orig, dir, pixel);
	//Phantom::PickPoint data;
	//m_nTextureBlendChannel	=	this->m_layerImages.m_imageViewSelect;
	//if(m_scene3D->IntersectTerrainOnly(orig, dir, data))
	//{
	//}
}
VOID						GCSceneEditor3D::DrawLine3DArrow(const Phantom::float3& from, const Phantom::float3& to, unsigned int color, float fArrow)
{
	//Phantom::g_manager.DrawLine(from, to, &color4(color));
	//Phantom::float3 dir	=	to - from;
	//if((dir.x*dir.x+dir.y*dir.y) < 0.001f)
	//{
	//}
	//else
	//{
	//}
}
VOID						GCSceneEditor3D::OnRenderingLine()
{
	//Phantom::matrix4x4 mat;
	//mat.identity();
	//r.Render->SetWorldMatrix(&mat);
	//r.Render->SetAlphaMode(ALPHAMODE_ALPHABLEND, false);
	//r.Render->SetZWriteEnable(true);
	////
	//if(m_objectEditMode == ObjectEditMode_Select)
	//{
	//	CPoint			pos;
	//	GetCursorPos(&pos);
	//	ScreenToClient(GetDeviceWindow(), &pos);
	//	GetDlgResManager()->DrawRect(&PRect(min(pos.x, m_downPoint.x), min(pos.y, m_downPoint.y), max(pos.x, m_downPoint.x), max(pos.y, m_downPoint.y)), 0xffff0000, false);
	//}
	////m_pointTexture->SetTexture(0);
	////
	//for(int i=0;i<this->m_treeView.GetSelectCount();i++)
	//{
	//	HTREEITEM hItem		=	m_treeView.GetSelect(i);
	//	TreeItemType* type	=	this->GetTreeItem(this->m_treeView.GetItemData(hItem));
	//	if(!type)
	//		continue;
	//	switch(type->type)
	//	{
	//	case SCENE3D_ANIMATION:
	//		{
	//			ISCA_Animation* ani	=	m_scene3D->GetAnimationManager().SearchDataPtr(type->p1);
	//			for(int s = 0; s < ani->GetScaManager().GetDataCount(); s++)
	//			{
	//				ISCA_Base* base	=	ani->GetScaManager().GetDataPtr(s);
	//				for(int t=0;t<base->GetTrackManager().GetDataCount(); t++)
	//				{
	//					ISCA_Track* track	=	base->GetTrackManager().GetDataPtr(t);
	//					r.Render->SetTexture(0, 0);
	//					if(track->GetSccPosition())
	//					{
	//						for(int p=1;p<track->GetSccPosition()->GetPointCount();p++)
	//						{
	//							const SccPoint* fromPos	=	track->GetSccPosition()->GetPoint(p - 1);
	//							const SccPoint* pos		=	track->GetSccPosition()->GetPoint(p);
	//							r.Phantom::g_manager.DrawLine(fromPos->f, pos->f, 0xffff3f00);
	//						}
	//					}
	//				}
	//			}
	//		}
	//		break;
	//	case SCENE3D_SCA:
	//		{
	//			ISCA_Animation* ani	=	m_scene3D->GetAnimationManager().SearchDataPtr(type->p1);
	//			if(ani)
	//			{
	//				ISCA_Base* base	=	ani->GetScaManager().SearchDataPtr(type->p2);
	//				for(int t=0;t<base->GetTrackManager().GetDataCount(); t++)
	//				{
	//					ISCA_Track* track	=	base->GetTrackManager().GetDataPtr(t);
	//					r.Render->SetTexture(0, 0);
	//					if(track->GetSccPosition())
	//					{
	//						for(int p=1;p<track->GetSccPosition()->GetPointCount();p++)
	//						{
	//							const SccPoint* fromPos	=	track->GetSccPosition()->GetPoint(p - 1);
	//							const SccPoint* pos		=	track->GetSccPosition()->GetPoint(p);
	//							r.Phantom::g_manager.DrawLine(fromPos->f, pos->f, 0xffff3f00);
	//						}
	//					}
	//				}
	//			}
	//		}
	//		break;
	//	case SCENE3D_SCA_TRACK:
	//		{
	//			ISCA_Animation* ani	=	m_scene3D->GetAnimationManager().SearchDataPtr(type->p1);
	//			if(ani)
	//			{
	//				ISCA_Base* base	=	ani->GetScaManager().SearchDataPtr(type->p2);
	//				if(!base)
	//					break;
	//				ISCA_Track* track	=	base->GetTrackManager().SearchDataPtr(type->p3);
	//				if(track && track->GetSccPosition())
	//				{
	//					r.Render->SetTexture(0, 0);
	//					for(int p=1;p<track->GetSccPosition()->GetPointCount();p++)
	//					{
	//						const SccPoint* fromPos	=	track->GetSccPosition()->GetPoint(p - 1);
	//						const SccPoint* pos		=	track->GetSccPosition()->GetPoint(p);
	//						r.Phantom::g_manager.DrawLine(fromPos->f, pos->f, 0xffff3f00);
	//					}
	//					for(int p=0;p<track->GetSccPosition()->GetPointCount();p++)
	//					{
	//						const SccPoint* pos		=	track->GetSccPosition()->GetPoint(p);
	//						float f	=	.3f;
	//						Phantom::AABox box;
	//						box._min.setxyz(pos->f.x - f,pos->f.y - f,pos->f.z - f);
	//						box._max.setxyz(pos->f.x + f,pos->f.y + f,pos->f.z + f);
	//						if(IsSelectLinear(pos->id))
	//							r.Render->drawBox(box, 0xffff5f00, true);
	//						else
	//							r.Render->drawBox(box, 0xff5fff00, true);
	//					}
	//				}
	//			}
	//		}
	//		break;
	//	}
	//}
	////
	//if(this->m_selectLines.size() > 0)
	//{
	//	m_axisMoving.DrawAxis();
	//}
	//r.Render->SetZEnable(true);
	//r.Render->SetAlphaMode(ALPHAMODE_DISABLE);
	//r.Render->SetZWriteEnable(true);
}

BOOL						GCSceneEditor3D::BeginEditorLine(Phantom::matrix4x4& matOut)
{
	//matOut.identity();
	//if(m_selectLines.size() == 0)
	//	return false;
	//double x=0,y=0,z=0;
	//for(int i=0;i<this->m_selectLines.size();i++)
	//{
	//	LinearData& ld	=	m_selectLines[i];
	//	const SccPoint* p = ld.sccPoint->GetPoint(Phantom::PtrID(-1, ld.id));
	//	x	+=	p->f.x;
	//	y	+=	p->f.y;
	//	z	+=	p->f.z;
	//	ld.downPos	=	p->f;
	//	ld.newPos	=	p->f;
	//}
	//double inv = 1.0f / (double)m_selectLines.size();
	//matOut.SetPosition( x * inv, y * inv, z * inv );
	return true;
}

VOID						GCSceneEditor3D::OnEditorLine(const Phantom::matrix4x4& downMat, const Phantom::matrix4x4& newMat)
{
	//Phantom::matrix4x4 origMat	=	newMat;
	//origMat.SetPosition(0,0,0);
	//Phantom::float3 offset	=	newMat.GetPosition() - downMat.GetPosition();
	//for(int i=0;i<this->m_selectLines.size();i++)
	//{
	//	LinearData& ld	=	m_selectLines[i];
	//	const SccPoint* p = ld.sccPoint->GetPoint(Phantom::PtrID(-1, ld.id));
	//	//
	//	BOOL bControl	=	GetSelectKeyboard();
	//	Phantom::matrix4x4 downMatrix,newMatrix;
	//	downMatrix.identity();
	//	downMatrix.SetPosition(ld.downPos);
	//	newMatrix.identity();
	//	newMatrix.SetPosition(ld.newPos);
	//	if(bControl)
	//	{
	//		Phantom::matrix4x4 mat;
	//		matrixInverse(&mat, NULL, &downMat);
	//		matrixMultiply(&mat, &downMatrix, &mat);
	//		matrixMultiply(&newMatrix, &mat, &newMat);//origMat);
	//	}
	//	else
	//	{
	//		newMatrix	=	downMatrix;
	//		newMatrix.SetPosition(0,0,0);
	//		matrixMultiply(&newMatrix, &newMat, &newMatrix);
	//		newMatrix.SetPosition( downMatrix.GetPosition() + offset );
	//	}
	//	ld.newPos	=	newMatrix.GetPosition();
	//	if(m_objectEditMode != ObjectEditMode_Copy)
	//	{
	//		ld.sccPoint->SetPoint(Phantom::PtrID(-1, ld.id), ld.newPos);
	//	}
	//}
}

VOID						GCSceneEditor3D::EndEditorLine(const Phantom::matrix4x4& downMat)
{
	//if(m_objectEditMode == ObjectEditMode_Copy)
	//{
	//	if(GetCopyKeyboard())
	//	{
	//		std::vector<LinearData> lines	=	m_selectLines;
	//		m_selectLines.clear();
	//		for(int i=0;i<lines.size();i++)
	//		{
	//			LinearData& ld	=	lines[i];
	//			Phantom::PtrID id(-1, ld.id);
	//			ld.sccPoint->GetPoint(id);
	//			const SccPoint* pos	=	ld.sccPoint->AddPoint(ld.newPos, id.index + 1);
	//			//
	//			LinearData ll;
	//			ll	=	ld;
	//			ll.id	=	pos->id;
	//			this->AddToLinear(&ll);
	//		}
	//	}
	//}
	//else
	//{
	//	for(int i=0;i<m_selectLines.size();i++)
	//	{
	//		LinearData& ld	=	m_selectLines[i];
	//		Phantom::PtrID id(-1, ld.id);
	//		ld.sccPoint->SetPoint(id, ld.newPos);
	//	}
	//}
}

BOOL						GCSceneEditor3D::IsSelectLinear(int id)
{
	for(int i=0;i<this->m_selectLines.size();i++)
		if(m_selectLines[i].id == id)
			return true;
	return false;
}
