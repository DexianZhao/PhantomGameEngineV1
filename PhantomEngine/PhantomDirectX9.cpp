#ifdef WIN32
#include <windows.h>
#include <atlimage.h>
#include <io.h>
#endif

#ifdef DIRECT_X_9
extern HWND	GetDeviceWindow();
#include "PhantomDirectX9.h"
#include "PhantomSubMesh.h"
extern D3DFORMAT				GetDepthStencilFormat();
	extern "C"{
		float* 			matrixMultiply2( float *pOut, const float *pM1, const float *pM2 )
		{
			return (float*)D3DXMatrixMultiply((D3DXMATRIX*)pOut, (D3DXMATRIX*)pM1, (D3DXMATRIX*)pM2);
		}
		float* 			matrixInverse2( float *pOut, float *pDeterminant, const float *pM )
		{
			return (float*)D3DXMatrixInverse((D3DXMATRIX*)pOut, pDeterminant, (D3DXMATRIX*)pM);
		}
	};
namespace Phantom{
	BOOL		D3DXMeshToMyMesh(AutoPtr<DeviceMesh>& devmesh, ID3DXMesh* pMesh){
		D3DVERTEXELEMENT9 ele[MAX_FVF_DECL_SIZE];
		pMesh->GetDeclaration(ele);
		int nDeclaration = 0;
		VertexElement vele[32];
		for(int i=0;i<MAX_FVF_DECL_SIZE;i++)
		{
			if(ele[i].Stream != 0xFF)
				continue;
			if(ele[i].Offset != 0)
				continue;
			if(ele[i].Type != D3DDECLTYPE_UNUSED)
				continue;
			if(ele[i].Method != 0)
				continue;
			if(ele[i].Usage != 0)
				continue;
			if(ele[i].UsageIndex != 0)
				continue;
			nDeclaration = i;
			break;
		}
		for(int i=0;i<nDeclaration;i++)
		{
			vele[i].method = ele[i].Method;
			vele[i].type = ele[i].Type;
			vele[i].usage = ele[i].Usage;
			vele[i].usageIndex = ele[i].UsageIndex;
		}
		void* pIndex;
		void* pVertex;
		DWORD* pSubset;
		MeshPtr temp;
		g_manager.CreateMesh(temp);
		devmesh = temp;
		devmesh->Create(pMesh->GetNumVertices(), pMesh->GetNumFaces(), vele, nDeclaration);
		if(FAILED(pMesh->LockVertexBuffer( 0, (void**) &pVertex ) )){
			safe_release(pMesh);
			return false;
		}
		assert(pMesh->GetNumBytesPerVertex() * pMesh->GetNumVertices() == devmesh->GetElementWidth() * devmesh->GetNumVertices());
		memcpy(devmesh->GetVertexPtr(), pVertex, devmesh->GetElementWidth() * devmesh->GetNumVertices());
		pMesh->UnlockVertexBuffer();
		if(FAILED(pMesh->LockIndexBuffer( 0, (void**) &pIndex ) )){
			safe_release(pMesh);
			return false;
		}
		//memcpy(devmesh->GetIndices(), pIndex, sizeof(Triangle) * devmesh->GetNumFaces());
		//
		unsigned char* attr = new unsigned char[devmesh->GetNumFaces()];
		if(FAILED(pMesh->LockAttributeBuffer( 0, &pSubset ))){
			safe_release(pMesh);
			return false;
		}
		for(int i=0;i<devmesh->GetNumFaces();i++){
			attr[i] = pSubset[i];
		}
		devmesh->WriteIndexBuffer((unsigned short*)pIndex, devmesh->GetNumFaces()*3, attr);
		delete[] attr;
		pMesh->UnlockIndexBuffer();
		pMesh->UnlockAttributeBuffer();
		return true;
	}
	extern	int				CreateElements(D3DVERTEXELEMENT9* ret, VertexElement* elements, int elementCount);
	BOOL					LODMesh::GenericLod(void* p1, void* p2)
	{CPUTime(LODMesh);
		HRESULT hr;
		ID3DXMesh* pMesh = (ID3DXMesh*)p1;
		DWORD* pAdjacencyIn = (DWORD*)p2;
		ID3DXMesh* pTempMesh = 0;
		if( FAILED( hr = D3DXCleanMesh( D3DXCLEAN_SIMPLIFICATION, pMesh, (DWORD*)pAdjacencyIn, &pTempMesh, (DWORD*)pAdjacencyIn, NULL ) ) )
			return FALSE;
		D3DVERTEXELEMENT9 vDeclaration[64];
		pMesh->GetDeclaration(vDeclaration);

		IDirect3DIndexBuffer9* pIB = 0;
		if(FAILED(pMesh->GetIndexBuffer(&pIB)))
			return FALSE;
		D3DINDEXBUFFER_DESC desc;
		pIB->GetDesc(&desc);
		pIB->Release();
		BOOL b32Bit	=	(desc.Format == D3DFMT_INDEX32);
		//
		pMesh = pTempMesh;
		//
		LPD3DXPMESH  pPMesh = NULL;
		hr = D3DXGeneratePMesh( pMesh, (DWORD*)pAdjacencyIn, NULL, NULL, 0, D3DXMESHSIMP_VERTEX, &pPMesh );
		if( FAILED(hr) )
			return FALSE;
		ID3DXPMesh* pTempPMesh = 0;
		if(FAILED(pPMesh->ClonePMesh(D3DXMESH_MANAGED | (b32Bit ? D3DXMESH_32BIT : 0), vDeclaration, GetDevice9(), &pTempPMesh)))
		{
			pPMesh->Release();
			return FALSE;
		}
		pPMesh->Release();
		pPMesh = pTempPMesh;
		DWORD cVerticesMin	=	pPMesh->GetMinVertices();
		DWORD cVerticesMax	=	pPMesh->GetMaxVertices();
		if(cVerticesMin<((float)pMesh->GetNumVertices()*0.5f))
			cVerticesMin = pMesh->GetNumVertices()*0.5f;
		float cVerticesPerMesh	=	( cVerticesMax - cVerticesMin) / (float)(MAX_LOD_LEVEL_COUNT);
		ID3DXPMesh* pMeshTemp = 0;
		memset( m_pLODMeshs, 0, sizeof(m_pLODMeshs) );
		INT nIndex = MAX_LOD_LEVEL_COUNT - 1;
		for( UINT iPMesh = 0; iPMesh < MAX_LOD_LEVEL_COUNT; iPMesh++ )
		{
			hr = pPMesh->ClonePMesh( D3DXMESH_MANAGED | D3DXMESH_VB_SHARE, vDeclaration, GetDevice9(), &pMeshTemp );
			if( FAILED(hr) )
			{
				if(pMeshTemp)
					pMeshTemp->Release();
				if( pPMesh )
					pPMesh->Release();
				return false;
			}
			hr = pMeshTemp->TrimByVertices( cVerticesMin + (int)(cVerticesPerMesh * (float)iPMesh), cVerticesMin + (int)(cVerticesPerMesh * (float)(iPMesh + 1)), NULL, NULL);
			if( FAILED(hr) )
			{
				if(pMeshTemp)
					pMeshTemp->Release();
				if( pPMesh )
					pPMesh->Release();
				return false;
			}
			pMeshTemp->SetNumVertices( cVerticesMin + (int)(cVerticesPerMesh * (float)iPMesh) );
			D3DVERTEXELEMENT9 element[64];
			pMeshTemp->GetDeclaration(element);
			ID3DXMesh* lodMesh = 0;
			pMeshTemp->CloneMesh(pMeshTemp->GetOptions(), element, GetDevice9(), &lodMesh);//m_pLODMeshs[nIndex]);
			//hr = m_pLODMeshs[nIndex]->OptimizeBaseLOD( D3DXMESHOPT_VERTEXCACHE, NULL );
			D3DXMeshToMyMesh(this->m_pLODMeshs[nIndex], lodMesh);
			safe_release(lodMesh);
			m_nLodLevelFaces[nIndex] = pMeshTemp->GetNumFaces();//cVerticesMin + (int)(cVerticesPerMesh * (float)iPMesh);
			if( FAILED(hr) )
			{
				if(pMeshTemp)
					pMeshTemp->Release();
				if( pPMesh )
					pPMesh->Release();
				return false;
			}
			pMeshTemp->Release();
			pMeshTemp	=	0;
			nIndex --;
		}
		//
		if( pPMesh )
			pPMesh->Release();
		if(pMesh)
			pMesh->Release();
		return TRUE;
	}
	VOID					SubMeshPolygon::GenericLodMesh(BOOL bGen)
	{CPUTime(SubMeshPolygon);
		this->m_bLodEnable = bGen;
		if(bGen)
			OptimizeMesh(true);
		else
			this->m_lodMesh.DestroyLODMesh();
	}
	BOOL					SubMeshPolygon::OptimizeMesh(BOOL bGenLod)
	{CPUTime(SubMeshPolygon);
		D3DVERTEXELEMENT9 ele[30];
		if(this->m_pRenderMesh->GetNumElements()<=0)
			return false;
		int count = CreateElements(ele, m_pRenderMesh->GetElementPtr(), m_pRenderMesh->GetNumElements());
		ID3DXMesh* pMesh	=	0;
		int numFaces = m_pRenderMesh->GetNumFaces();
		if(FAILED(D3DXCreateMesh(numFaces, m_pRenderMesh->GetNumVertices(), 0, ele, GetDevice9(), &pMesh)))
			return false;
		void* pVertex;
		if(FAILED(pMesh->LockVertexBuffer( 0, (void**) &pVertex ) )){
			safe_release(pMesh);
			return false;
		}
		assert(pMesh->GetNumBytesPerVertex() * pMesh->GetNumVertices() == m_pRenderMesh->GetElementWidth() * m_pRenderMesh->GetNumVertices());
		memcpy(pVertex, m_pRenderMesh->GetVertexPtr(), m_pRenderMesh->GetElementWidth() * m_pRenderMesh->GetNumVertices());
		pMesh->UnlockVertexBuffer();
		void* pIndex;
		if(FAILED(pMesh->LockIndexBuffer( 0, (void**) &pIndex ) )){
			safe_release(pMesh);
			return false;
		}
		memcpy(pIndex, m_pRenderMesh->GetIndices(), sizeof(Triangle) * numFaces);
		pMesh->UnlockIndexBuffer();
		//
		DWORD* pSubset;
		if(FAILED(pMesh->LockAttributeBuffer( 0, &pSubset ))){
			safe_release(pMesh);
			return false;
		}
		for(int i=0;i<numFaces;i++){
			pSubset[i]	=	m_pRenderMesh->GetAttributePtr()[i];
		}
		pMesh->UnlockAttributeBuffer();
		DWORD* aAdjacency = new DWORD[pMesh->GetNumFaces() * 3];
		if( aAdjacency == NULL ){
			safe_release(pMesh);
			return false;
		}
		memset(aAdjacency, 0, sizeof(DWORD) * pMesh->GetNumFaces() * 3);
		if(FAILED(pMesh->ConvertPointRepsToAdjacency(NULL, aAdjacency))){
			safe_release(pMesh);
			return false;
		}
		if(FAILED(pMesh->GenerateAdjacency(1e-6f,aAdjacency))){
			safe_release(pMesh);
			return false;
		}
		if(bGenLod){
			this->m_lodMesh.GenericLod( pMesh, aAdjacency );
		}
		if(FAILED(pMesh->OptimizeInplace(D3DXMESHOPT_ATTRSORT /*| D3DXMESHOPT_VERTEXCACHE*/, aAdjacency, NULL, NULL, NULL))){
			safe_release(pMesh);
			return false;
		}
		delete[] aAdjacency;
		//
		m_pRenderMesh = 0;
		D3DXMeshToMyMesh(m_pRenderMesh, pMesh);
		safe_release(pMesh);
		return true;
	}
	BOOL					DeviceMesh_Device9::OptimizeMesh()
	{CPUTime(DeviceMesh_Device9);
		D3DVERTEXELEMENT9 ele[30];
		if(GetNumElements()<=0)
			return false;
		int count = CreateElements(ele, GetElementPtr(), GetNumElements());
		ID3DXMesh* pMesh	=	0;
		int numFaces = GetNumFaces();
		if(FAILED(D3DXCreateMesh(numFaces, GetNumVertices(), 0, ele, GetDevice9(), &pMesh)))
			return false;
		void* pVertex;
		if(FAILED(pMesh->LockVertexBuffer( 0, (void**) &pVertex ) )){
			safe_release(pMesh);
			return false;
		}
		assert(pMesh->GetNumBytesPerVertex() * pMesh->GetNumVertices() == GetElementWidth() * GetNumVertices());
		memcpy(pVertex, GetVertexPtr(), GetElementWidth() * GetNumVertices());
		pMesh->UnlockVertexBuffer();
		void* pIndex;
		if(FAILED(pMesh->LockIndexBuffer( 0, (void**) &pIndex ) )){
			safe_release(pMesh);
			return false;
		}
		memcpy(pIndex, GetIndices(), sizeof(Triangle) * numFaces);
		pMesh->UnlockIndexBuffer();
		//
		DWORD* pSubset;
		if(FAILED(pMesh->LockAttributeBuffer( 0, &pSubset ))){
			safe_release(pMesh);
			return false;
		}
		if(this->m_attributes.size()>0){
			for(int i=0;i<numFaces;i++){
				pSubset[i]	=	GetAttributePtr()[i];
			}
		}
		else
			memset(pSubset, 0, sizeof(DWORD)*numFaces);
		pMesh->UnlockAttributeBuffer();
		DWORD* aAdjacency = new DWORD[pMesh->GetNumFaces() * 3];
		if( aAdjacency == NULL ){
			safe_release(pMesh);
			return false;
		}
		memset(aAdjacency, 0, sizeof(DWORD) * pMesh->GetNumFaces() * 3);
		if(FAILED(pMesh->ConvertPointRepsToAdjacency(NULL, aAdjacency))){
			safe_release(pMesh);
			return false;
		}
		if(FAILED(pMesh->GenerateAdjacency(1e-6f,aAdjacency))){
			safe_release(pMesh);
			return false;
		}
		if(FAILED(pMesh->OptimizeInplace(D3DXMESHOPT_ATTRSORT /*| D3DXMESHOPT_VERTEXCACHE*/, aAdjacency, NULL, NULL, NULL))){
			safe_release(pMesh);
			return false;
		}
		delete[] aAdjacency;
		//
		if(FAILED(pMesh->LockVertexBuffer( 0, (void**) &pVertex ) )){
			safe_release(pMesh);
			return false;
		}
		assert(pMesh->GetNumBytesPerVertex() * pMesh->GetNumVertices() == GetElementWidth() * GetNumVertices());
		memcpy(GetVertexPtr(), pVertex, GetElementWidth() * GetNumVertices());
		pMesh->UnlockVertexBuffer();
		if(FAILED(pMesh->LockIndexBuffer( 0, (void**) &pIndex ) )){
			safe_release(pMesh);
			return false;
		}
		unsigned char* attr = 0;
		if(m_attributes.size()>0){
			attr = new unsigned char[GetNumFaces()];
			if(FAILED(pMesh->LockAttributeBuffer( 0, &pSubset ))){
				safe_release(pMesh);
				return false;
			}
			for(int i=0;i<GetNumFaces();i++){
				attr[i] = pSubset[i];
			}
			pMesh->UnlockAttributeBuffer();
		}
		WriteIndexBuffer((unsigned short*)pIndex, GetNumFaces()*3, attr);
		if(attr)
			delete[] attr;
		pMesh->UnlockIndexBuffer();
		safe_release(pMesh);
		return true;
	}
	VOID					SubMeshData::ConvertToPng(const char* szTexturePath, const char* srcFile, const char* destFile)//tga,bmp,dds 转换成 png
	{CPUTimeNone();
	}
	void	ConvertToPng(const char* szFile, BOOL bRemoveOld)
	{
		FILE* f = fopen(szFile, "rb");
		if(!f)
			return;
		int nLength = filelength(_fileno(f));
		char* pBuffer = new char[nLength];
		fread(pBuffer, nLength, 1, f);
		fclose(f);
		//
		IDirect3DTexture9* pTexture = 0;
		D3DXIMAGE_INFO info;
		if(FAILED(D3DXGetImageInfoFromFileInMemory(pBuffer, nLength, &info)))
			return;
		UINT uMipmap	=	info.MipLevels;
		HRESULT hr = D3DXCreateTextureFromFileInMemoryEx(GetDevice9(), pBuffer, nLength, info.Width,info.Height,uMipmap,0,
			(D3DFORMAT)info.Format, (D3DPOOL)D3DPOOL_MANAGED,D3DX_DEFAULT,D3DX_DEFAULT, 0, NULL,NULL,&pTexture);
		if(FAILED(hr))
			return;
		IDirect3DSurface9* surf = 0;
		pTexture->GetSurfaceLevel(0, &surf);
		char szPngFile[512], szDir[512], szTemp[256];
		_splitpath(szFile, szPngFile, szDir, szTemp, 0);
		strcat(szPngFile, szDir);
		strcat(szPngFile, szTemp);
		strcat(szPngFile, ".png");
		strlwr(szPngFile);
		D3DXSaveSurfaceToFile(szPngFile,D3DXIFF_PNG,surf,NULL,NULL);
		surf->Release();
		pTexture->Release();
		if(bRemoveOld)
			DeleteFileA(szFile);
	}
	BOOL	SaveToJpg(const char* szDestFile, const char* szFile, int w, int h)
	{
		FILE* f = fopen(szFile, "rb");
		if(!f)
			return false;
		int nLength = filelength(_fileno(f));
		char* pBuffer = new char[nLength];
		fread(pBuffer, nLength, 1, f);
		fclose(f);
		//
		IDirect3DTexture9* pTexture = 0;
		D3DXIMAGE_INFO info;
		if(FAILED(D3DXGetImageInfoFromFileInMemory(pBuffer, nLength, &info)))
			return false;
		UINT uMipmap	=	info.MipLevels;
		info.Width = w;
		info.Height = h;
		HRESULT hr = D3DXCreateTextureFromFileInMemoryEx(GetDevice9(), pBuffer, nLength, info.Width,info.Height,uMipmap,0,
			(D3DFORMAT)info.Format, (D3DPOOL)D3DPOOL_MANAGED,D3DX_DEFAULT,D3DX_DEFAULT, 0, NULL,NULL,&pTexture);
		if(FAILED(hr))
			return false;
		IDirect3DSurface9* surf = 0;
		pTexture->GetSurfaceLevel(0, &surf);
		char szPngFile[512], szDir[512], szTemp[256];
		_splitpath(szDestFile, szPngFile, szDir, szTemp, 0);
		strcat(szPngFile, szDir);
		strcat(szPngFile, szTemp);
		strcat(szPngFile, ".jpg");
		strlwr(szPngFile);
		D3DXSaveSurfaceToFile(szPngFile,D3DXIFF_JPG,surf,NULL,NULL);
		surf->Release();
		pTexture->Release();
		return true;
	}
	RenderTarget*			Device_Device9::CreateShadowMap()
	{CPUTime(Device_Device9);
		return new DeviceShadowMap_Device9();
	}
	DeviceShadowMap_Device9::DeviceShadowMap_Device9()
	{
		m_pRenderTarget	=	0;
		m_pRenderSurface=	0;
		m_pZBuffer		=	0;
		m_pSaveRenderTargetBuffer	=	0;
		m_pSaveZBuffer	=	0;
	}
	DeviceShadowMap_Device9::~DeviceShadowMap_Device9()
	{
		OnLostDevice();
	}
	BOOL			DeviceShadowMap_Device9::Begin()
	{CPUTime(DeviceShadowMap_Device9);
		if(this->m_pSaveRenderTargetBuffer)
		{
			assert(m_pSaveRenderTargetBuffer==NULL);
			return false;
		}
		if(!m_pRenderTarget)
			return false;
		//
		IDirect3DDevice9* pd3dDevice = GetDevice9();
		pd3dDevice->GetRenderTarget(0, &m_pSaveRenderTargetBuffer);
		//
		if(FAILED(pd3dDevice->SetRenderTarget(0, m_pRenderSurface)))
			return false;
		//
		if(m_pZBuffer)
		{
			pd3dDevice->GetDepthStencilSurface(&m_pSaveZBuffer);
			if(FAILED(pd3dDevice->SetDepthStencilSurface(m_pZBuffer)))
				return false;
		}
		GetDevice9()->Clear(0, 0, CLEAR_TARGET | CLEAR_ZBUFFER, 0x8f800000, 1.0f, 0);
		return true;
	}
	VOID			DeviceShadowMap_Device9::End()
	{CPUTime(DeviceShadowMap_Device9);
		if(!this->m_pSaveRenderTargetBuffer)
			return;
		//
		IDirect3DDevice9* pd3dDevice = GetDevice9();
		//
		pd3dDevice->SetRenderTarget(0, m_pSaveRenderTargetBuffer);
		if(m_pSaveZBuffer)
			pd3dDevice->SetDepthStencilSurface(m_pSaveZBuffer);
		//
		safe_release(m_pSaveRenderTargetBuffer);
		safe_release(m_pSaveZBuffer);
	}
	void			DeviceShadowMap_Device9::OnLostDevice()
	{CPUTime(DeviceShadowMap_Device9);
		End();
		safe_release(m_pRenderSurface);
		safe_release(m_pRenderTarget);
		safe_release(m_pZBuffer);
	}
	VOID			DeviceShadowMap_Device9::OnResetDevice()
	{CPUTime(DeviceShadowMap_Device9);
		OnLostDevice();
#ifndef EXPORT_ENGINE
		IDirect3DDevice9* pd3dDevice = GetDevice9();
		HRESULT hr;
		D3DFORMAT uFormat = D3DFMT_R32F;
		if(m_rtFormat == RTFormat_ShadowMap)
			uFormat = D3DFMT_R32F;
		else
			return;
		if(FAILED(hr = ::D3DXCreateTexture(pd3dDevice,m_imgBuffer.width,m_imgBuffer.height, 1, D3DUSAGE_RENDERTARGET,uFormat,D3DPOOL_DEFAULT,&m_pRenderTarget)))
		{
			LogInfo("system->error: render target fail");
			return;
		}
		m_pRenderTarget->GetSurfaceLevel(0, &m_pRenderSurface);
		if( FAILED( pd3dDevice->CreateDepthStencilSurface( m_imgBuffer.width,m_imgBuffer.height, GetDepthStencilFormat(), D3DMULTISAMPLE_NONE, 0, TRUE, &m_pZBuffer, 0 ) ) )
		{
			LogInfo("system->error: render target zbuffer fail");
			return;
		}
#endif
	}
	extern	VOID	LoadPng(const char* s);
	BOOL	DxErrorOutput(HRESULT hr)
	{
		//LogError("\nglGetError() = %i (0x%.8x) at line %i\n", glError, glError, __LINE__);
		return false;
	}
	Device_Device9*	g_dev9 = 0;
	Device_Device9::Device_Device9()
	{
		m_declColor		=	0;
		m_declColorUV	=	0;
		m_declNormalColorUV	=	0;
		m_declUV		=	0;
		m_lastProgDecl	=	0;
		m_lastDecl		=	-1;
		g_dev9		=	this;
		m_declColor = this->AddDecl(FVF_XYZ | FVF_DIFFUSE);
		m_declColorUV = this->AddDecl(FVF_XYZ | FVF_DIFFUSE | FVF_TEX1);
		m_declNormalColorUV = this->AddDecl(FVF_XYZ | FVF_NORMAL | FVF_DIFFUSE | FVF_TEX1);
		m_declUV = this->AddDecl(FVF_XYZ | FVF_TEX1);
	}
	Device_Device9::~Device_Device9()
	{
	}

	DeviceRenderText_Device9::DeviceRenderText_Device9()
	{
		m_texturePtr	=	0;
	}
	DeviceRenderText_Device9::~DeviceRenderText_Device9()
	{
		if(g_manager.IsDeviceSetup())
			OnLostDevice();
	}
	extern int	GetNearestSize(int n);
	VOID				DeviceRenderText_Device9::SetText(const char* text)
	{CPUTime(DeviceRenderText_Device9);
        m_textW.SetArrayCount(0);
		int len = text?strlen(text):0;
		m_text.SetArrayCount(len);
		if(len>0)
		{
			memcpy(m_text.GetPtr(), text, len*sizeof(char));
			m_textW.SetArrayCount(len);
			int ret = ::MultiByteToWideChar(CP_UTF8, 0, m_text.GetPtr(), len+1, (wchar_t*)m_textW.GetPtr(), m_textW.size());
			//m_textW.SetArrayCount(ret);
		}
		
		if(g_manager.IsDeviceSetup())
			OnResetDevice();
	}

	VOID					DeviceRenderText_Device9::OnResetDevice()
	{CPUTime(DeviceRenderText_Device9);
		if(m_text.size()==0)
		{
			m_imgWidth = 0;
			m_imgHeight = 0;
			return;
		}
		static BOOL bInit = false;
		if(!bInit)
		{
			bInit = true;
			Gdiplus::GdiplusStartupInput input;
			Gdiplus::GdiplusStartupOutput output;
			ULONG_PTR dwToken = 0;
			Gdiplus::Status status = Gdiplus::GdiplusStartup( &dwToken, &input, &output );
			if( status != Gdiplus::Ok )
				return;
		}
		wchar_t wname[128];
		int len = this->m_name.size();
		len = ::MultiByteToWideChar(CP_UTF8, 0, m_name.t, len+1, wname, 128);
		wname[len] = 0;
		Gdiplus::FontFamily fontFamily(wname);//L"宋体");
		Gdiplus::Font       font(&fontFamily, (m_nFontSize>0?m_nFontSize:-m_nFontSize)*2, m_nFontStyle, Gdiplus::Unit::UnitPixel);
		if(!font.IsAvailable())
			return;
		Gdiplus::SolidBrush solidBrush(0xffffffff);
		Gdiplus::Graphics   graphics(GetDeviceWindow());
		static int nA = Gdiplus::TextRenderingHint::TextRenderingHintSystemDefault;//TextRenderingHintClearTypeGridFit;//TextRenderingHintSystemDefault;
		graphics.SetTextRenderingHint((Gdiplus::TextRenderingHint)nA);
		Gdiplus::RectF boundingBox(0, 0, 0, 0);
		Gdiplus::StringFormat fmt;
		//fmt.SetLineAlignment(Gdiplus::StringAlignmentCenter);
		if(this->m_nTextAlignment & Phantom::DrawTextAlign_LEFT)
			fmt.SetAlignment(Gdiplus::StringAlignmentNear);
		else if(this->m_nTextAlignment & Phantom::DrawTextAlign_CENTER)
			fmt.SetAlignment(Gdiplus::StringAlignmentCenter);
		else if(this->m_nTextAlignment & Phantom::DrawTextAlign_RIGHT)
			fmt.SetAlignment(Gdiplus::StringAlignmentFar);
		graphics.MeasureString((const wchar_t*)m_textW.GetPtr(), -1, &font, Gdiplus::PointF(0, 0), &boundingBox);
		Gdiplus::SizeF  size;
		boundingBox.GetSize(&size);
		Gdiplus::Bitmap bitmap((int)size.Width, (int)size.Height, PixelFormat32bppARGB);
		Gdiplus::Graphics g(&bitmap);
		if(m_clip.cx!=0&&m_clip.cy!=0){
			if(size.Width>m_clip.cx)
				size.Width=m_clip.cx;
			if(size.Height>m_clip.cy)
				size.Height=m_clip.cy;
		}
		g.DrawString((const wchar_t*)m_textW.GetPtr(), -1, &font, Gdiplus::RectF(0, 0, size.Width, size.Height), &fmt, &solidBrush);
		Gdiplus::BitmapData bitmapData;
		Gdiplus::Rect rect(0, 0, (int)size.Width, (int)size.Height);
		bitmap.LockBits(&rect,Gdiplus::ImageLockModeRead,PixelFormat32bppARGB,&bitmapData);
		m_imgWidth = bitmapData.Width;
		m_imgHeight = bitmapData.Height;
		int newWidth = GetNearestSize(bitmapData.Width);
		int newHeight = GetNearestSize(bitmapData.Height);
		if(!m_texturePtr || (newWidth!=m_realWidth) || (newHeight != m_realHeight)){
			m_realWidth = newWidth;
			m_realHeight = newHeight;
			OnLostDevice();
			if(FAILED(::D3DXCreateTexture(GetDevice9(), m_realWidth, m_realHeight, 1, D3DUSAGE_DYNAMIC, D3DFMT_A4R4G4B4, D3DPOOL_DEFAULT, &m_texturePtr)))
			{
				bitmap.UnlockBits(&bitmapData);
				return;
			}
			Texture::s_maxImageBuffer += (m_realWidth*m_realHeight*2);
		}

		D3DLOCKED_RECT lock;
		if(FAILED(m_texturePtr->LockRect(0, &lock, 0, 0)))
		{
			bitmap.UnlockBits(&bitmapData);
			return;
		}

		unsigned char* src = (unsigned char*)bitmapData.Scan0;
		for(int y = 0; y < bitmapData.Height; ++y){
			for(int x = 0; x < bitmapData.Width; ++x){
				unsigned char* pixel = src + (y * bitmapData.Stride + x*4);
				if(x>=1024||y>=1024)continue;
				int r = ((int)pixel[0])/16;
				int g = ((int)pixel[1])/16;
				int b = ((int)pixel[2])/16;
				int a = ((int)pixel[3])/16;
				unsigned char* dest = (unsigned char*)((const char*)lock.pBits + y * lock.Pitch + x * 2);
				dest[0] = b + (g*16);
				dest[1] = r + (a*16);
			}
		}
		bitmap.UnlockBits(&bitmapData);
		m_texturePtr->UnlockRect(0);
	}
	VOID					DeviceRenderText_Device9::OnLostDevice()
	{CPUTime(DeviceRenderText_Device9);
		if(m_texturePtr)
			Texture::s_maxImageBuffer -= (m_realWidth*m_realHeight*2);
		safe_release(m_texturePtr);
	}

	DeviceTexture_Device9::DeviceTexture_Device9()
	{
		m_texturePtr	=	0;
		memset(m_samplerFilter, 0, sizeof(m_samplerFilter));
		memset(m_dxSamplerFilter, 0, sizeof(m_dxSamplerFilter));
		m_dxSamplerFilter[SAMP_MAGFILTER]	=	D3DTEXF_LINEAR;
		m_dxSamplerFilter[SAMP_MINFILTER]	=	D3DTEXF_LINEAR;
		m_dxSamplerFilter[SAMP_MIPFILTER]	=	D3DTEXF_LINEAR;
	}
	DeviceTexture_Device9::~DeviceTexture_Device9()
	{
		m_imgBuffer.FreePixels();
		if(g_manager.IsDeviceSetup())
			OnLostDevice();
	}
	VOID					DeviceTexture_Device9::Destroy()
	{CPUTime(DeviceTexture_Device9);
		Texture::Destroy();
		this->OnLostDevice();
	}
	VOID					DeviceTexture_Device9::OnLoadDevice()
	{CPUTime(DeviceTexture_Device9);
		m_imgBuffer.AutoResize(this->m_fileName.t);
		Texture::s_maxImageBuffer += m_imgBuffer.GetBufferSize();
		if((m_nFilter&0x00020000)>0)
			m_imgBuffer.ToSmallRGB();
		int numPixel = 3;
		D3DFORMAT format = D3DFMT_R8G8B8;
		if(m_imgBuffer.bpp == m_imgBuffer.PixelFormat_RGBA)
		{
			numPixel = 4;
			format	=	D3DFMT_A8R8G8B8;
		}
		else if(m_imgBuffer.bpp == m_imgBuffer.PixelFormat_RGBA_4444)
		{
			numPixel = 2;
			format	=	D3DFMT_A4R4G4B4;
		}
		else if(m_imgBuffer.bpp == m_imgBuffer.PixelFormat_RGB_565)
		{
			numPixel = 2;
			format	=	D3DFMT_R5G6B5;
		}
		if(m_bGenMipmap)
			::D3DXCreateTexture(GetDevice9(), m_imgBuffer.width, m_imgBuffer.height, D3DX_DEFAULT, D3DUSAGE_DYNAMIC|D3DUSAGE_AUTOGENMIPMAP, format, D3DPOOL_DEFAULT,&m_texturePtr);
		else
			::D3DXCreateTexture(GetDevice9(), m_imgBuffer.width, m_imgBuffer.height, 1, D3DUSAGE_DYNAMIC, format, D3DPOOL_DEFAULT,&m_texturePtr);
		if(!m_texturePtr)
		{
			LogInfo("system->can't create texture %s", this->m_fileName.t);
			return;
		}
		//
		D3DLOCKED_RECT lock;
		if(FAILED(m_texturePtr->LockRect(0, &lock, 0, 0)))
		{
			LogInfo("system->can't lock texture %s", this->m_fileName.t);
			return;
		}
		if(m_imgBuffer.bpp == m_imgBuffer.PixelFormat_RGBA_4444 || m_imgBuffer.bpp == m_imgBuffer.PixelFormat_RGB_565)
		{
			for(int y = 0; y < m_imgBuffer.height; ++y){
				unsigned char* src = (m_imgBuffer.pixels + (y * m_imgBuffer.width)*2);
				unsigned char* dest = ((unsigned char*)lock.pBits + y * lock.Pitch );
				memcpy(dest, src, m_imgBuffer.width * 2);
			}
		}
		else
		{
			for(int y = 0; y < m_imgBuffer.height; ++y){
				for(int x = 0; x < m_imgBuffer.width; ++x){
					unsigned char* src = (m_imgBuffer.pixels + (y * m_imgBuffer.width+x)*numPixel);
					unsigned char* dest = ((unsigned char*)lock.pBits + y * lock.Pitch + x * 4);
					if(numPixel == 3){
						dest[0] = src[2];
						dest[1] = src[1];
						dest[2] = src[0];
						dest[3] = 255;
					}
					else
					{
						dest[0] = src[2];
						dest[1] = src[1];
						dest[2] = src[0];
						dest[3] = src[3];
					}
				}
			}
		}
		m_texturePtr->UnlockRect(0);
		if(m_bGenMipmap)
			m_texturePtr->GenerateMipSubLevels();
		if((m_nFilter&0x0000ffff)==1){
			m_dxSamplerFilter[SAMP_MINFILTER]	=	D3DTEXF_POINT;
			m_dxSamplerFilter[SAMP_MAGFILTER]	=	D3DTEXF_POINT;
			m_dxSamplerFilter[SAMP_MIPFILTER]	=	D3DTEXF_LINEAR;
		}
		else
		{
			m_dxSamplerFilter[SAMP_MINFILTER]	=	D3DTEXF_LINEAR;
			m_dxSamplerFilter[SAMP_MAGFILTER]	=	D3DTEXF_LINEAR;
			m_dxSamplerFilter[SAMP_MIPFILTER]	=	D3DTEXF_LINEAR;
		}
		m_imgBuffer.FreePixels(false);
		m_loadThread = false;
	}
	VOID					DeviceTexture_Device9::OnResetDevice()
	{CPUTime(DeviceTexture_Device9);
		OnLostDevice();
		if(!LoadTextureData())
			return;
		if(!this->m_imgBuffer.pixels)
			return;
		if(!this->m_loadThread)
			this->OnLoadDevice();
	}
	VOID					DeviceTexture_Device9::OnLostDevice()
	{CPUTime(DeviceTexture_Device9);
		if(m_texturePtr)
			Texture::s_maxImageBuffer -= m_imgBuffer.GetBufferSize();
		safe_release(m_texturePtr);
	}
	VOID					DeviceTexture_Device9::SetSampler(SamplerState sampler, TextureFilter filter)
	{CPUTime(DeviceTexture_Device9);
		if(m_samplerFilter[sampler] == filter)
			return;
		m_samplerFilter[sampler]	=	filter;
		if(filter == Phantom::TextureFilter_Point)
			m_dxSamplerFilter[sampler]	=	D3DTEXF_POINT;
		else
			m_dxSamplerFilter[sampler]	=	D3DTEXF_LINEAR;
	}

	DeviceProgram_Device9::DeviceProgram_Device9(int mode)
		:Program(mode)
	{
		m_hMain		=	0;
		m_shaderPtr	=	0;
		m_bFail		=	false;
		m_bCreated	=	false;
		/*m_varTex[TextureIndex_0].SetName("texture1", _shader_var_name::S_T_Uniform);
		m_varTex[TextureIndex_1].SetName("texture2", _shader_var_name::S_T_Uniform);
		m_varTex[TextureIndex_2].SetName("texture3", _shader_var_name::S_T_Uniform);
		m_varTex[TextureIndex_3].SetName("texture4", _shader_var_name::S_T_Uniform);
		m_varTex[TextureIndex_4].SetName("texture5", _shader_var_name::S_T_Uniform);*/
	}
	DeviceProgram_Device9::~DeviceProgram_Device9()
	{
		if(g_manager.IsDeviceSetup())
			OnLostDevice();
	}
	VOID				DeviceProgram_Device9::OnCreateShader()
	{CPUTime(DeviceProgram_Device9);
		BOOL bToShadowMap	=	IsProgram(Program::Type_ToShadowMap);
		BOOL bMaterial	=	IsProgram(Program::Type_Material);
		BOOL bUV		=	IsProgram(Program::Type_UV);
		BOOL bNormal	=	IsProgram(Program::Type_Normal);
		BOOL bLight		=	(IsProgram(Program::Type_Light) && bNormal);
		BOOL bColor		=	IsProgram(Program::Type_Color);
		BOOL bBlend		=	this->m_bBlendMode;
		BOOL bAlphaTest	=	IsProgram(Program::Type_AlphaTest);
		BOOL bContrast	=	IsProgram(Program::Type_Contrast);
		BOOL bStaticShadowMap	=	IsProgram(Program::Type_StaticShadowMap);
		BOOL bWithShadowMap		=	IsProgram(Program::Type_WithShadowMap);
		BOOL bUV2		=	bStaticShadowMap;
		BOOL bUseColor	=	(bColor||bUV||bMaterial);
		BOOL bFog		=	IsProgram(Program::Type_Fog)||IsProgram(Program::Type_Fog2D)||IsProgram(Program::Type_Fog2DGray)||IsProgram(Program::Type_Fog3DGray)
			||IsProgram(Program::Type_Fog3DSub)||IsProgram(Program::Type_Fog2DSub);
		BOOL bBrightness = IsProgram(Program::Type_Brightness);
		//if(bStaticShadowMap)
		//	bLight		=	false;
		Text	def;
		def >> "matrix	sWorld;";
		def >> "matrix	sViewProj;";
		if(bFog)
		{
			def >> "float3 sFogPos;";
			def >> "float3 sFogColor;";
			def >> "float2 sFogRange;";//near,far
		}
		if(bMaterial){
			def >> "float4	sDiffuse;";
			def >> "float3	sEmissive;";
		}
		if(bToShadowMap){
			def >> "float m_shadowEpsilon = 0.00001;";
			//def >> "float sShadowEpsilon = 1.003;";
		}
		if(bWithShadowMap){
			def >> "matrix sViewToLightProj;";
		}
		const int maxLight = MAX_LIGHT_COUNT;
		char buf[256];
		if(bLight&&bUseColor){
			def >> "int		bLightDirect;";//是否存在全局方向光
			def >> "float3	sLightDirect;";//[5];";//一个模型最多承受5个灯光的影响
			def >> "float3	sLightDirectC;";
			def >> "float3	sAmbient;";
			sprintf(buf, "float4	sLightPos[%d];", maxLight);//一个模型最多承受5个灯光的影响
			def >> buf;
			sprintf(buf, "float3	sLightColor[%d];", maxLight);//一个模型最多承受5个灯光的影响
			def >> buf;
			sprintf(buf, "float4	sLightRangeA[%d];", maxLight);//一个模型最多承受5个灯光的影响
			def >> buf;
			def >> "int		sLightCount = 0;";//灯光数量
		}
		if(bBrightness)
			def >> "float	sBrightness = 1;";
		if(bContrast)
			def >> "float	sContrast	= 1;";
		if(bAlphaTest)
			def >> "float	sAlphaConstrast	= 1;";
		if(bBlend)
			def.add("matrix sBones[%d];\r\n", PROGRAM_BONE_COUNT);
		if(bUV){
			def >> "texture texture1;";
			def >> "sampler sampler1 =	sampler_state{";
			def >> "Texture		= <texture1>;";
			def >> "MinFilter	=  LINEAR;";
			def >> "MagFilter	=  LINEAR;";
			def >> "MipFilter	=  LINEAR;";
			def >> "};";
		}
		if(bStaticShadowMap||bWithShadowMap){
			def >> "float2		shadowTextureSize;";
			def >> "float		shadowDensity = 0.5;";
		}
		if(bStaticShadowMap){//TextureIndex_3
			def >> "texture texture4;";
			def >> "sampler sampler4 =	sampler_state{";
			def >> "Texture		= <texture4>;";
			def >> "MinFilter	=  LINEAR;";
			def >> "MagFilter	=  LINEAR;";
			def >> "MipFilter	=  LINEAR;";
			//def >> "BORDERCOLOR	=	0x8f800000;";
			//def >> "AddressU	=	BORDER;";
			//def >> "AddressV	=	BORDER;";
			def >> "};";
		}
		if(bWithShadowMap){//TextureIndex_3
			def >> "texture texture6;";
			def >> "sampler sampler6 =	sampler_state{";
			def >> "Texture		= <texture6>;";
			def >> "MinFilter	=  LINEAR;";
			def >> "MagFilter	=  LINEAR;";
			def >> "MipFilter	=  LINEAR;";
			def >> "BORDERCOLOR	=	0x8f800000;";
			def >> "AddressU	=	BORDER;";
			def >> "AddressV	=	BORDER;";
			def >> "};";
		}
		def >> "struct VInput{";
		def >> "float4 Pos		: POSITION;";
		if(bNormal)
			def >> "float3 Normal	: NORMAL;";
		if(bUV)
			def >> "float2 uv		: TEXCOORD0;";
		if(bUV2)
			def >> "float2 uv2		: TEXCOORD1;";
		if(bColor)
			def >> "float4 color: COLOR0;";
		if(bBlend){
			def >> "float4 vIdx	: COLOR1;";
			def >> "float2 vW0	: TEXCOORD2;";
			def >> "float2 vW1	: TEXCOORD3;";
		}
		def >> "};";
		//vertex shader

		if(bToShadowMap){
			def >> "struct VOutput{";
			def >> "float4 Pos		: POSITION;";
			if(bAlphaTest&&bUV)
				def >> "float2 uv		: TEXCOORD0;";
			def >> "float2 depth	: TEXCOORD1;";
			def >> "};";
			def >> "VOutput Vshader(VInput input){";
			def >> "VOutput output;";
			if(bBlend){
				def >> "float4  fBI		=	D3DCOLORtoUBYTE4(input.vIdx);";
				def >> "float4x4 m0 	= 	sBones[	fBI[0] ]	*	input.vW0.x;";
				def >> "float4x4 m1 	= 	sBones[	fBI[1] ]	*	input.vW0.y;";
				def >> "float4x4 m2 	= 	sBones[	fBI[2] ]	*	input.vW1.x;";
				def >> "float4x4 m3 	= 	sBones[	fBI[3] ]	*	input.vW1.y;";
				def >> "float4 fm1		=	float4(m0[0][0] + m1[0][0] + m2[0][0] + m3[0][0] ,	m0[1][0] + m1[1][0] + m2[1][0] + m3[1][0] ,	m0[2][0] + m1[2][0] + m2[2][0] + m3[2][0] ,	m0[3][0] + m1[3][0] + m2[3][0] + m3[3][0] );";
				def >> "float4 fm2		=	float4(m0[0][1] + m1[0][1] + m2[0][1] + m3[0][1] ,	m0[1][1] + m1[1][1] + m2[1][1] + m3[1][1] ,	m0[2][1] + m1[2][1] + m2[2][1] + m3[2][1] ,	m0[3][1] + m1[3][1] + m2[3][1] + m3[3][1] );";
				def >> "float4 fm3		=	float4(m0[0][2] + m1[0][2] + m2[0][2] + m3[0][2] ,	m0[1][2] + m1[1][2] + m2[1][2] + m3[1][2] ,	m0[2][2] + m1[2][2] + m2[2][2] + m3[2][2] ,	m0[3][2] + m1[3][2] + m2[3][2] + m3[3][2] );";
				def >> "input.Pos		=	float4(dot(input.Pos.xyz,	fm1.xyz)	+	fm1.w,	dot(input.Pos.xyz,	fm2.xyz)	+	fm2.w,	dot(input.Pos.xyz,	fm3.xyz)	+	fm3.w, 1);";
			}
			def >> "float4 pos				=	mul(input.Pos, sWorld);";
			def >> "output.Pos				=	mul(pos, sViewProj);";
			def >> "output.depth.xy			=	output.Pos.zw;";
			if(bAlphaTest&&bUV)
				def >> "output.uv		=	input.uv;";
			def >> "return output;";
			def >> "}";
			//pixel shader
			def >> "float4	Pshader(VOutput output) : COLOR0{";
			if(bAlphaTest&&bUV)
				def >> "if(tex2D(sampler1, output.uv).a < 0.5f)discard;";
			def >> "return output.depth.x / output.depth.y * m_shadowEpsilon;";//sShadowEpsilon;";
			def >> "}";
		}
		else{
			def >> "struct VOutput{";
			def >> "float4 Pos		: POSITION;";
			if(bUseColor)
				def >> "float4 Color	: TEXCOORD2;";
			if(bUV)
				def >> "float2 uv		: TEXCOORD0;";
			if(bUV2)
				def >> "float2 uv2		: TEXCOORD1;";
			if(bFog)
				def >> "float3 dir		: TEXCOORD3;";
			if(bWithShadowMap)
				def >> "float4 shadowMap : TEXCOORD4;";
			def >> "};";
			def >> "VOutput Vshader(VInput input){";
			def >> "VOutput output;";
			if(bBlend){
				def >> "float4  fBI		=	D3DCOLORtoUBYTE4(input.vIdx);";
				def >> "float4x4 m0 	= 	sBones[	fBI[0] ]	*	input.vW0.x;";
				def >> "float4x4 m1 	= 	sBones[	fBI[1] ]	*	input.vW0.y;";
				def >> "float4x4 m2 	= 	sBones[	fBI[2] ]	*	input.vW1.x;";
				def >> "float4x4 m3 	= 	sBones[	fBI[3] ]	*	input.vW1.y;";
				def >> "float4 fm1		=	float4(m0[0][0] + m1[0][0] + m2[0][0] + m3[0][0] ,	m0[1][0] + m1[1][0] + m2[1][0] + m3[1][0] ,	m0[2][0] + m1[2][0] + m2[2][0] + m3[2][0] ,	m0[3][0] + m1[3][0] + m2[3][0] + m3[3][0] );";
				def >> "float4 fm2		=	float4(m0[0][1] + m1[0][1] + m2[0][1] + m3[0][1] ,	m0[1][1] + m1[1][1] + m2[1][1] + m3[1][1] ,	m0[2][1] + m1[2][1] + m2[2][1] + m3[2][1] ,	m0[3][1] + m1[3][1] + m2[3][1] + m3[3][1] );";
				def >> "float4 fm3		=	float4(m0[0][2] + m1[0][2] + m2[0][2] + m3[0][2] ,	m0[1][2] + m1[1][2] + m2[1][2] + m3[1][2] ,	m0[2][2] + m1[2][2] + m2[2][2] + m3[2][2] ,	m0[3][2] + m1[3][2] + m2[3][2] + m3[3][2] );";
				def >> "input.Pos		=	float4(dot(input.Pos.xyz,	fm1.xyz)	+	fm1.w,	dot(input.Pos.xyz,	fm2.xyz)	+	fm2.w,	dot(input.Pos.xyz,	fm3.xyz)	+	fm3.w, 1);";
				if(bNormal)
					def >> "float3 Normal	=	float3(dot(input.Normal,	fm1.xyz),	dot(input.Normal,	fm2.xyz),	dot(input.Normal,	fm3.xyz));";
			}
			else if(bNormal)
				def >> "float3 Normal		=	input.Normal;";
			def >> "float4 pos				=	mul(input.Pos, sWorld);";
			if(bNormal)
				def >> "Normal		=	mul(Normal, (float3x3)sWorld);";
			if(bFog)
				def >> "output.dir			=	pos.xyz - sFogPos;";
			def >> "output.Pos				=	mul(pos, sViewProj);";
			if(bUV)
				def >> "output.uv				=	input.uv;";
			if(bUV2)
				def >> "output.uv2				=	input.uv2;";
			if(bWithShadowMap){
				def >> "output.shadowMap = mul(pos, sViewToLightProj);";
				//def >> "shadowPos.xyz			=	shadowPos.xyz / shadowPos.w;";
				//def >> "output.shadowMap		=	float3( 0.5f * shadowPos.x + 0.5f, 1.0f - (0.5f * shadowPos.y + 0.5f), shadowPos.z);";
			}
			if(bUseColor){
				if(bColor)
					def >> "output.Color.rgba     	=	input.color.rgba;";
				else
					def >> "output.Color.rgba     	=	1;";
				if(bLight)
				{
					def >> "float3 flight = 0;";
					//directional
					def >> "if(bLightDirect){";
					def >> "float fldir = clamp(dot(sLightDirect, Normal),0,10);";
					def >> "flight += fldir*sLightDirectC + sAmbient.xyz;";
					def >> "}";
					//for(int i=0;i<maxLight;i++){
					//	sprintf(buf, "if(sLightCount>%d){", i);
					//	def >> buf;
					//	sprintf(buf, "%s light = sLightPos[%d].xyz - pos.xyz;", (i==0)?"float3":"", i);
					//	def >> buf;
					//	sprintf(buf, "flight	+=	clamp( dot( normalize(light), Normal) * (1-clamp((length(light)-sLightRangeA[%d].x)/sLightRangeA[%d].y,0,1)),0,1) * sLightColor[%d];",
					//		i,i,i);
					//	def >> buf;
					//}
					//for(int i=0;i<maxLight;i++){
					//	def >> "}";
					//}
					def >> "output.Color.rgb *= flight.xyz;";
				}
				if(bMaterial)
				{
					def >> "output.Color.rgb	=	output.Color.rgb * sDiffuse.rgb + sEmissive.rgb;";
					def >> "output.Color.a		*=	sDiffuse.a;";
				}
				if(bBrightness)
					def >> "output.Color.rgb	+=	sBrightness;";
			}
			def >> "return output;";
			def >> "}";
			//pixel shader
			def >> "float4	Pshader(VOutput output) : COLOR0{";
			if(bUseColor){
				def >> "float4 ret = output.Color;";
				if(bUV)
					def >> "ret	*= tex2D(sampler1, output.uv);";
			}
			else if(bUV)
				def >> "ret	= tex2D(sampler1, output.uv);";
			if(bAlphaTest){
				def >> "ret.a *= sAlphaConstrast;";
				def >> "if(ret.a < 0.99)discard;";
			}
			if(bStaticShadowMap)
			{
				if(!(IsProgram(Type_ShadowUsePCF3)||IsProgram(Type_ShadowUsePCF5)))//g_manager.m_shadowPCF==Phantom::PCF_0||!IsProgram(Type_ShadowUsePCF))
					def >> "ret.rgb	*= tex2D(sampler4, output.uv2).r;";// * shadowDensity+(1-shadowDensity);";
				else{
					def >> "float shadow = 0;";
					int size = 1;
					if(IsProgram(Type_ShadowUsePCF3))//g_manager.m_shadowPCF==Phantom::PCF_3x3)
						size = 1;
					else if(IsProgram(Type_ShadowUsePCF5))//g_manager.m_shadowPCF==Phantom::PCF_5x5)
						size = 2;
					//else if(g_manager.m_shadowPCF==Phantom::PCF_7x7)
						//size = 3;
					def.add("for(int y=-%d;y<=%d;y++){\r\n",size,size);
					def.add("for(int x=-%d;x<=%d;x++){\r\n",size,size);
					def >> "shadow	+=	tex2D( sampler4, output.uv2 + float2(x, y) * shadowTextureSize ).r;";
					def >> "}}";
					def.add("ret.rgb *= shadow / %.01f * shadowDensity+(1-shadowDensity);\r\n", (float)((size*2+1)*(size*2+1)));
				}
			}
			if(bWithShadowMap){
				def >> "float3 shadowPos	=	output.shadowMap.xyz / output.shadowMap.w;";
				def >> "float2 shadowUV		=	float2( 0.5f * shadowPos.x + 0.5f, 1.0f - (0.5f * shadowPos.y + 0.5f));";
				if(!(IsProgram(Type_ShadowUsePCF3)||IsProgram(Type_ShadowUsePCF5))){//if(g_manager.m_shadowPCF==Phantom::PCF_0||!IsProgram(Type_ShadowUsePCF)){
					def >> "float shadow		=	1;";
					def >> "float textureShadow	=	tex2D( sampler6, shadowUV ).r;";
					def >> "if(shadowPos.z > textureShadow)shadow = 1-shadowDensity;";
					def >> "ret.rgb	*= shadow;";// * shadowDensity+(1-shadowDensity);";
				}
				else{
					int size = 1;
					if(IsProgram(Type_ShadowUsePCF3))//g_manager.m_shadowPCF==Phantom::PCF_3x3)
						size = 1;
					else if(IsProgram(Type_ShadowUsePCF5))//g_manager.m_shadowPCF==Phantom::PCF_5x5)
						size = 2;
					//else if(g_manager.m_shadowPCF==Phantom::PCF_7x7)
						//size = 3;
					float pcfcount = (float)((size*2+1)*(size*2+1));
					def.add("float shadow = %.0f;\r\n", pcfcount);
					def.add("for(int y=-%d;y<=%d;y++){\r\n",size,size);
					def.add("for(int x=-%d;x<=%d;x++){\r\n",size,size);
					def >> "if(shadowPos.z > tex2D( sampler6, shadowUV + float2(x, y) * shadowTextureSize ).r)shadow-=1;";
					def >> "}}";
					def.add("ret.rgb *= shadow / %.01f * shadowDensity+(1-shadowDensity);\r\n", pcfcount);
				}
			}
			if(bFog){
				if(IsProgram(Program::Type_Fog2D))
					def >> "ret.rgb = lerp(ret.rgb, sFogColor.xyz, clamp((length(output.dir.xy)-sFogRange.x)/sFogRange.y, 0, 1));";
				else if(IsProgram(Program::Type_Fog2DGray))
					def >> "ret.rgb = lerp(ret.rgb, sFogColor.xyz*(ret.r*0.11 + ret.g * 0.59 + ret.b * 0.30), clamp((length(output.dir.xy)-sFogRange.x)/sFogRange.y, 0, 1));";
				else if(IsProgram(Program::Type_Fog3DGray))
					def >> "ret.rgb = lerp(ret.rgb, sFogColor.xyz*(ret.r*0.11 + ret.g * 0.59 + ret.b * 0.30), clamp((length(output.dir)-sFogRange.x)/sFogRange.y, 0, 1));";
				else if(IsProgram(Program::Type_Fog3DSub))
					def >> "ret.rgb -= sFogColor.xyz * clamp((length(output.dir)-sFogRange.x)/sFogRange.y, 0, 1);";
				else if(IsProgram(Program::Type_Fog2DSub))
					def >> "ret.rgb -= sFogColor.xyz * clamp((length(output.dir.xy)-sFogRange.x)/sFogRange.y, 0, 1);";//*= (1+3*sFogColor.xyz*(1-clamp((length(output.dir.xy)-sFogRange.x)/sFogRange.y,0,1)));";
				else
					def >> "ret.rgb = lerp(ret.rgb, sFogColor.xyz, clamp((length(output.dir)-sFogRange.x)/sFogRange.y, 0, 1));";
			}
			if(bContrast)
				def >> "ret.rgb = ((ret.rgb - 0.5f) * sContrast) + 0.5f;";
			//if(bBrightness)
			//	def >> "ret.rgb += sBrightness;";
			def >> "return ret;";
			def >> "}";
		}
		//main
		def >> "technique main{";
		def >> "pass P{";
		if(IsProgram(Type_ShadowUsePCF5)){
			def >> "VertexShader = compile vs_3_0 Vshader();";
			def >> "PixelShader  = compile ps_3_0 Pshader();";
		}
		else{
			def >> "VertexShader = compile vs_2_0 Vshader();";
			def >> "PixelShader  = compile ps_2_0 Pshader();";
		}
		def >> "}";
		def >> "}";
		m_vsps_directx9.resize(def.size()+1);
		strcpy(m_vsps_directx9.GetPtr(), def.str());
	}
	VOID				DeviceProgram_Device9::OnResetDevice()	//根据渲染模式创建shader
	{CPUTime(DeviceProgram_Device9);
		Program::OnResetDevice();
		if(m_vsps_directx9.size() == 0)
			return;
		OnLostDevice();
		m_resetID++;
		m_bFail		=	true;
		LPD3DXBUFFER pBufferErrors = NULL;
		HRESULT hr = S_OK;
		if(FAILED(hr = D3DXCreateEffect(GetDevice9(), this->m_vsps_directx9.GetPtr(), this->m_vsps_directx9.size(), 0, 0, D3DXFX_NOT_CLONEABLE, 0, &m_shaderPtr, &pBufferErrors)))
		{
			if(pBufferErrors)
			{
				char buf[4096];
				sprintf(buf, "error:%s\n%s", m_name.t, (const char*)pBufferErrors->GetBufferPointer(), m_vsps_directx9.GetPtr());
				MessageBox(0, buf, "Shader Error", MB_OK);
				pBufferErrors->Release();
			}
			return;
		}
		//
		m_hMain		=	0;
		int index	=	0;
		D3DXHANDLE h = this->m_shaderPtr->GetTechnique(index);
		m_hMain	=	h;
		while(h)
		{
			D3DXTECHNIQUE_DESC desc;
			m_shaderPtr->GetTechniqueDesc(h, &desc);
			if(stricmp(desc.Name, "main") == 0)
			{
				m_hMain	=	h;
				break;
			}
			index++;
			h = m_shaderPtr->GetTechnique(index);
		}
		//
		m_bFail		=	false;
		m_bCreated	=	true;
#ifndef WIN32_OS
		m_vsps_directx9.clear();
		m_vsps_opengles_v.clear();
		m_vsps_opengles_p.clear();
#endif
	}
	INT					g_nMinFilter = 0;
	INT					g_nMagFilter = 0;
	INT					g_nMipFilter = 0;
	VOID				DeviceProgram_Device9::SetTextTexture(TextureIndex index, RenderText* tex)//渲染文本的时候被调用
	{CPUTime(DeviceProgram_Device9);
		static const char* varTex[TextureIndex_Max] = {"texture1","texture2","texture3","texture4","texture5","texture6"};
		if(!FindVariant(this->m_varTex[index], varTex[index]))
			return;
		if(tex)
		{
			DeviceRenderText_Device9* tex9 = static_cast<DeviceRenderText_Device9*>(tex);
			if(D3DTEXF_POINT != g_nMinFilter)
				this->SetInt(m_h_minFilter,g_nMinFilter = D3DTEXF_POINT, "filterMin");
			if(D3DTEXF_POINT != g_nMagFilter)
				this->SetInt(m_h_magFilter,g_nMagFilter = D3DTEXF_POINT, "filterMag");
			m_shaderPtr->SetTexture((D3DXHANDLE)m_varTex[index].handle, static_cast<DeviceRenderText_Device9*>(tex)->m_texturePtr);
		}
		else
			m_shaderPtr->SetTexture((D3DXHANDLE)m_varTex[index].handle, 0);
	}
	VOID				DeviceProgram_Device9::SetTexture(TextureIndex index, Texture* tex)
	{CPUTimeNone();
		static const char* varTex[TextureIndex_Max] = {"texture1","texture2","texture3","texture4","texture5","texture6"};
		if(!FindVariant(this->m_varTex[index], varTex[index]))
			return;
		m_varTextPtr[index]	=	tex;
		if(tex)
		{
			int nMinFilter = 0;
			int nMagFilter = 0;
			if(tex->GetType() == 0)
			{
				DeviceTexture_Device9* tex9 = static_cast<DeviceTexture_Device9*>(tex);
				int nFilter = static_cast<DeviceTexture_Device9*>(tex)->m_nFilter&0x0000ffff;
				if(nFilter==1){
					nMinFilter = D3DTEXF_POINT;
					nMagFilter = D3DTEXF_POINT;
				}
				else{//if(m_nFilter==1){
					nMinFilter = D3DTEXF_LINEAR;
					nMagFilter = D3DTEXF_LINEAR;
				}
				if(nMinFilter != g_nMinFilter)
					this->SetInt(m_h_minFilter,g_nMinFilter = nMinFilter, "filterMin");
				if(nMagFilter != g_nMagFilter)
					this->SetInt(m_h_magFilter,g_nMagFilter = nMagFilter, "filterMag");
				//if(tex9->m_dxSamplerFilter[SAMP_MIPFILTER] != g_nMipFilter)
				//	this->SetInt(m_h_mipFilter,g_nMipFilter = tex9->m_dxSamplerFilter[SAMP_MIPFILTER], "filterMip");
				if((static_cast<DeviceTexture_Device9*>(tex)->m_nFilter&0x00010000)>0)
				{
					GetDevice9()->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
					GetDevice9()->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
				}
				else
				{
					GetDevice9()->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
					GetDevice9()->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
				}
				m_shaderPtr->SetTexture((D3DXHANDLE)m_varTex[index].handle, static_cast<DeviceTexture_Device9*>(tex)->m_texturePtr);
			}
			else
			{
				nMinFilter = D3DTEXF_LINEAR;
				nMagFilter = D3DTEXF_LINEAR;
				if(nMinFilter != g_nMinFilter)
					this->SetInt(m_h_minFilter,g_nMinFilter = nMinFilter, "filterMin");
				if(nMagFilter != g_nMagFilter)
					this->SetInt(m_h_magFilter,g_nMagFilter = nMagFilter, "filterMag");
				DeviceShadowMap_Device9* tex9 = static_cast<DeviceShadowMap_Device9*>(tex);
				m_shaderPtr->SetTexture((D3DXHANDLE)m_varTex[index].handle, static_cast<DeviceShadowMap_Device9*>(tex)->m_pRenderTarget);
			}
		}
		else
			m_shaderPtr->SetTexture((D3DXHANDLE)m_varTex[index].handle, 0);
	}
	BOOL					DeviceProgram_Device9::SetMatrixPtr(_shader_var& var, const Phantom::matrix4x4* v, int nCount, const char* szName)
	{CPUTime(DeviceProgram_Device9);
		if(!FindVariant(var, szName))
			return false;
		BASE_CHECK_RET(m_shaderPtr->SetMatrixArray((D3DXHANDLE)var.handle, (D3DXMATRIX*)v, nCount));
		return true;
	}
	BOOL					DeviceProgram_Device9::SetFloat(_shader_var& var, float f, const char* szName)
	{CPUTime(DeviceProgram_Device9);
		if(!FindVariant(var, szName))
			return false;
		BASE_CHECK_RET(m_shaderPtr->SetFloat((D3DXHANDLE)var.handle, f));
		return true;
	}
	BOOL					DeviceProgram_Device9::SetFloat2(_shader_var& var, const Phantom::float2& v, const char* szName)
	{CPUTime(DeviceProgram_Device9);
		if(!FindVariant(var, szName))
			return false;
		BASE_CHECK_RET(m_shaderPtr->SetFloatArray((D3DXHANDLE)var.handle, (float*)&v, 2));
		return true;
	}
	BOOL					DeviceProgram_Device9::SetFloat3(_shader_var& var, const Phantom::float3& v, const char* szName)
	{CPUTime(DeviceProgram_Device9);
		if(!FindVariant(var, szName))
			return false;
		BASE_CHECK_RET(m_shaderPtr->SetFloatArray((D3DXHANDLE)var.handle, (float*)&v, 3));
		return true;
	}
	BOOL					DeviceProgram_Device9::SetFloat4(_shader_var& var, const Phantom::float4& v, const char* szName)
	{CPUTime(DeviceProgram_Device9);
		if(!FindVariant(var, szName))
			return false;
		BASE_CHECK_RET(m_shaderPtr->SetVector((D3DXHANDLE)var.handle, (D3DXVECTOR4*)&v));
		return true;
	}
	BOOL					DeviceProgram_Device9::SetFloatPtr(_shader_var& var, const float* f, int nCount, const char* szName)
	{CPUTime(DeviceProgram_Device9);
		if(!FindVariant(var, szName))
			return false;
		BASE_CHECK_RET(m_shaderPtr->SetFloatArray((D3DXHANDLE)var.handle, f, nCount));
		return true;
	}
	BOOL					DeviceProgram_Device9::SetFloat2Ptr(_shader_var& var, const Phantom::float2* v, int nCount, const char* szName)
	{CPUTime(DeviceProgram_Device9);
		if(!FindVariant(var, szName))
			return false;
		BASE_CHECK_RET(m_shaderPtr->SetFloatArray((D3DXHANDLE)var.handle, (float*)v, nCount * 2));
		return true;
	}
	BOOL					DeviceProgram_Device9::SetFloat3Ptr(_shader_var& var, const Phantom::float3* v, int nCount, const char* szName)
	{CPUTime(DeviceProgram_Device9);
		if(!FindVariant(var, szName))
			return false;
		BASE_CHECK_RET(m_shaderPtr->SetFloatArray((D3DXHANDLE)var.handle, (float*)v, nCount * 3));
		return true;
	}
	BOOL					DeviceProgram_Device9::SetFloat4Ptr(_shader_var& var, const Phantom::float4* v, int nCount, const char* szName)
	{CPUTime(DeviceProgram_Device9);
		if(!FindVariant(var, szName))
			return false;
		BASE_CHECK_RET(m_shaderPtr->SetVectorArray((D3DXHANDLE)var.handle, (D3DXVECTOR4*)v, nCount));
		return true;
	}

	BOOL			DeviceProgram_Device9::FindVariant(_shader_var& out, const char* szVariantName)//查找attribute shader元素
	{CPUTimeNone();
		if(m_shaderPtr == 0 || out.is_findFail())
			return false;
		if(out.programID == (int)m_shaderPtr && out.programID > 0 && out.is_findOK() && out.resetID == m_resetID)
			return true;
		out.resetID		=	m_resetID;
		out.programID	=	(int)m_shaderPtr;
		D3DXEFFECT_DESC	desc;
		D3DXPARAMETER_DESC ParamDesc;
		BASE_CHECK_RET(m_shaderPtr->GetDesc(&desc));
		for( unsigned int iParam = 0; iParam < desc.Parameters; iParam++ )
		{
			D3DXHANDLE hParam = m_shaderPtr->GetParameter ( NULL, iParam );
			m_shaderPtr->GetParameterDesc( hParam, &ParamDesc );
			if(ParamDesc.Name)
			{
				if(stricmp(ParamDesc.Name, szVariantName) == 0)
				{
					out.set_handle((unsigned int)hParam);
					return true;
				}
			}
		}
		out.set_null();
		return false;
	}
	BOOL			DeviceProgram_Device9::Active()
	{CPUTime(DeviceProgram_Device9);
		return true;
	}
	BOOL			DeviceProgram_Device9::SetInt(_shader_var& var, int f, const char* szName)
	{CPUTime(DeviceProgram_Device9);
		if(!FindVariant(var, szName))
			return false;
		BASE_CHECK_RET(m_shaderPtr->SetInt((D3DXHANDLE)var.handle, f));
		return true;
	}
	BOOL			DeviceProgram_Device9::SetInt(_shader_var_name& var, int i)
	{CPUTime(DeviceProgram_Device9);
		if(!FindVariant(var, var.name))
			return false;
		BASE_CHECK_RET(m_shaderPtr->SetInt((D3DXHANDLE)var.handle, i));
		return true;
	}
	BOOL			DeviceProgram_Device9::SetMatrix(_shader_var& var, const Phantom::matrix4x4* mat, const char* szName)
	{CPUTime(DeviceProgram_Device9);
		if(!FindVariant(var, szName))
			return false;
		BASE_CHECK_RET(m_shaderPtr->SetMatrix((D3DXHANDLE)var.handle, (D3DXMATRIX*)mat));
		return true;
	}
	BOOL			DeviceProgram_Device9::SetMatrix(_shader_var_name& var, const Phantom::matrix4x4* mat)
	{CPUTime(DeviceProgram_Device9);
		if(!FindVariant(var, var.name))
			return false;
		BASE_CHECK_RET(m_shaderPtr->SetMatrix((D3DXHANDLE)var.handle, (D3DXMATRIX*)mat));
		return true;
	}

	void			DeviceProgram_Device9::OnLostDevice()
	{CPUTime(DeviceProgram_Device9);
		for(int i=0;i<TextureIndex_Max;i++)
			m_varTextPtr[i]	=	0;
		//
		m_bFail		=	false;
		m_bCreated	=	false;
		safe_release(m_shaderPtr);
	}

	VOID	AddElement(D3DVERTEXELEMENT9* ele, int& eleIndex, int& offset, int type, int usage, int usageIndex)
	{
		D3DVERTEXELEMENT9& el = ele[eleIndex];
		eleIndex++;
		//
		el.Stream		=	0;
		el.Offset		=	offset;
		el.Type			=	type;
		el.Method		=	0;
		el.Usage		=	usage;
		el.UsageIndex	=	usageIndex;
		switch(el.Type)
		{
		case D3DDECLTYPE_FLOAT1:
			offset	+=	sizeof(float);
			break;
		case D3DDECLTYPE_FLOAT2:
			offset	+=	sizeof(float) * 2;
			break;
		case D3DDECLTYPE_FLOAT3:
			offset	+=	sizeof(float) * 3;
			break;
		case D3DDECLTYPE_FLOAT4:
			offset	+=	sizeof(float) * 4;
			break;
		case D3DDECLTYPE_D3DCOLOR:
			offset	+=	sizeof(unsigned int);
			break;
		case D3DDECLTYPE_UBYTE4:
			offset	+=	sizeof(char) * 4;
			break;
		case D3DDECLTYPE_SHORT2:
			offset	+=	sizeof(short) * 2;
			break;
		case D3DDECLTYPE_SHORT4:
			offset	+=	sizeof(short) * 4;
			break;
		}
	}
	int	FillElemenetByFVF(D3DVERTEXELEMENT9* ele, INT nFVF)
	{
		int	eleIndex	=	0;
		int offset		=	0;
		if(nFVF & FVF_XYZ)
			AddElement(ele, eleIndex, offset, D3DDECLTYPE_FLOAT3, VertexElement::VE_USAGE_POSITION, 0);
		if(nFVF & FVF_NORMAL)
			AddElement(ele, eleIndex, offset, D3DDECLTYPE_FLOAT3, VertexElement::VE_USAGE_NORMAL, 0);
		if(nFVF & FVF_DIFFUSE)
			AddElement(ele, eleIndex, offset, VertexElement::VE_TYPE_D3DCOLOR, VertexElement::VE_USAGE_COLOR, 0);
		//
		BOOL t1 = false;
		BOOL t2 = false;
		BOOL t3 = false;
		BOOL t4 = false;
		BOOL t5 = false;
		if((nFVF & FVF_TEX5) == FVF_TEX5)
			t1 = t2 = t3 = t4=t5=true;
		else if((nFVF & FVF_TEX4) == FVF_TEX4)
			t1 = t2 = t3 = t4=true;
		else if((nFVF & FVF_TEX3) == FVF_TEX3)
			t1 = t2 = t3 = true;
		else if((nFVF & FVF_TEX2) == FVF_TEX2)
			t1 = t2 = true;
		else if((nFVF & FVF_TEX1) == FVF_TEX1)
			t1	=	true;
		if(t1)
			AddElement(ele, eleIndex, offset, D3DDECLTYPE_FLOAT2, VertexElement::VE_USAGE_TEXCOORD, 0);
		if(t2)
			AddElement(ele, eleIndex, offset, D3DDECLTYPE_FLOAT2, VertexElement::VE_USAGE_TEXCOORD, 1);
		if(t3)
			AddElement(ele, eleIndex, offset, D3DDECLTYPE_FLOAT2, VertexElement::VE_USAGE_TEXCOORD, 2);
		if(t4)
			AddElement(ele, eleIndex, offset, D3DDECLTYPE_FLOAT2, VertexElement::VE_USAGE_TEXCOORD, 3);
		if(t5)
			AddElement(ele, eleIndex, offset, D3DDECLTYPE_FLOAT2, VertexElement::VE_USAGE_TEXCOORD, 4);
		D3DVERTEXELEMENT9 el = D3DDECL_END();
		ele[eleIndex]	=	el;
		return (eleIndex+1);
	}

	int		CreateElements(D3DVERTEXELEMENT9* ret, VertexElement* elements, int elementCount)
	{
		int index			=	0;
		WORD offset			=	0;
		for(int i=0;i<elementCount;i++)
		{
			D3DVERTEXELEMENT9 el;
			el.Stream		=	0;
			el.Offset		=	offset;
			el.Type			=	elements[i].type;
			el.Method		=	elements[i].method;
			el.Usage		=	elements[i].usage;
			el.UsageIndex	=	elements[i].usageIndex;
			switch(el.Type)
			{
			case VertexElement::VE_TYPE_FLOAT1:
				offset	+=	sizeof(float);
				break;
			case VertexElement::VE_TYPE_FLOAT2:
				offset	+=	sizeof(float) * 2;
				break;
			case VertexElement::VE_TYPE_FLOAT3:
				offset	+=	sizeof(float) * 3;
				break;
			case VertexElement::VE_TYPE_FLOAT4:
				offset	+=	sizeof(float) * 4;
				break;
			case VertexElement::VE_TYPE_D3DCOLOR:
				offset	+=	sizeof(unsigned int);
				break;
			case VertexElement::VE_TYPE_UBYTE4:
				offset	+=	sizeof(char) * 4;
				break;
			case VertexElement::VE_TYPE_SHORT2:
				offset	+=	sizeof(short) * 2;
				break;
			case VertexElement::VE_TYPE_SHORT4:
				offset	+=	sizeof(short) * 4;
				break;
			default:
				{
					D3DVERTEXELEMENT9 el2 = D3DDECL_END();
					memcpy_s(&ret[index], &el2, sizeof(D3DVERTEXELEMENT9));
					return elementCount;
				}
				break;
			}
			memcpy_s(&ret[index], &el, sizeof(D3DVERTEXELEMENT9));
			index++;
		}
		//if(bHasTBN)
		//{
		//	D3DVERTEXELEMENT9 el;
		//	el.Stream		=	0;
		//	el.Offset		=	offset;
		//	el.Type			=	VertexElement::VE_TYPE_FLOAT3;
		//	el.Method		=	D3DDECLMETHOD_DEFAULT;
		//	el.Usage		=	VertexElement::VE_USAGE_TANGENT;
		//	el.UsageIndex	=	0;
		//	memcpy_s(&ret[index], &el, sizeof(D3DVERTEXELEMENT9));
		//	//
		//	offset	+=	sizeof(float) * 3;
		//	index++;
		//	//
		//	el.Stream		=	0;
		//	el.Offset		=	offset;
		//	el.Type			=	VertexElement::VE_TYPE_FLOAT3;
		//	el.Method		=	D3DDECLMETHOD_DEFAULT;
		//	el.Usage		=	VertexElement::VE_USAGE_BINORMAL;
		//	el.UsageIndex	=	0;
		//	memcpy_s(&ret[index], &el, sizeof(D3DVERTEXELEMENT9));
		//	//
		//	offset	+=	sizeof(float) * 3;
		//	index++;
		//}
		//
		D3DVERTEXELEMENT9 el = D3DDECL_END();
		memcpy_s(&ret[index], &el, sizeof(D3DVERTEXELEMENT9));
		return index + 1;
	}
	DeviceMesh_Device9::DeviceMesh_Device9()
	{
		m_nStrideSize	=	0;	//顶点宽度
		m_nNumIndices	=	0;
//		m_meshPtr		=	0;
		m_decl			=	-1;
	}
	DeviceMesh_Device9::~DeviceMesh_Device9()
	{
		if(g_manager.IsDeviceSetup())
			OnLostDevice();
		for(int i=0;i<m_meshAttribs.size();i++)
		{
			if(m_meshAttribs[i])
				delete m_meshAttribs[i];
		}
		m_meshAttribs.clear();
	}
	VOID							DeviceMesh_Device9::WriteIndexBuffer(const unsigned short* indexPtr, int indexCount, unsigned char* attrib)
	{CPUTime(DeviceMesh_Device9);
		if(m_nNumIndices==0)return;
		DeviceMesh::WriteIndexBuffer(indexPtr, indexCount, attrib);
		for(int i=0;i<m_meshAttribs.size();i++)
		{
			if(m_meshAttribs[i])
				delete m_meshAttribs[i];
		}
		this->m_nNumIndices		=	indexCount;
		m_meshAttribs.clear();
		//
		if(!attrib)
		{
			DirectXMeshAttribute* attr	=	new DirectXMeshAttribute();
			m_meshAttribs.push_back(attr);
			attr->faces.SetArrayCount(indexCount / 3);
			memcpy(attr->faces.GetPtr(), indexPtr, sizeof(Triangle) * attr->faces.size());
			attr->numFaces = attr->faces.size();
		}
		else
		{
			int faceCount		=	indexCount / 3;
			Triangle* triSrc	=	(Triangle*)indexPtr;
			for(int i=0;i<faceCount;i++)
			{
				int index	=	attrib[i];
				if(m_meshAttribs.size() <= index)
					m_meshAttribs.SetArrayCount(index + 1);
				if(!m_meshAttribs[index])
					m_meshAttribs[index]	=	new DirectXMeshAttribute();
				DirectXMeshAttribute* attr	=	m_meshAttribs[index];
				attr->faces.push_back(triSrc[i]);
			}
			for(int i=0;i<m_meshAttribs.size();i++)
			{
				if(m_meshAttribs[i])
					m_meshAttribs[i]->numFaces = m_meshAttribs[i]->faces.size();
			}
		}
	}

	BOOL					DeviceMesh_Device9::IsLoaded()
	{CPUTime(DeviceMesh_Device9);
		return (m_meshAttribs.size()>0 || this->m_nNumVertices>0);//m_meshPtr != 0);
	}
	VOID					DeviceMesh_Device9::Destroy()
	{CPUTime(DeviceMesh_Device9);
		DeviceMesh::Destroy();
		this->OnLostDevice();
	}
	BOOL					DeviceMesh_Device9::RenderMesh(Program* prog, Texture* tex, const Phantom::matrix4x4* mat)	//渲染
	{CPUTime(DeviceMesh_Device9);
//		if(!m_meshPtr)
//			return false;
		if(mat)
			g_manager.SetWorldMatrix(mat);
		prog->SetTexture(TextureIndex_0, tex);
		RenderSubset(prog, 0);
		return true;
	}
	VOID					DeviceMesh_Device9::GenElements()
	{CPUTime(DeviceMesh_Device9);
		if(m_decl>=0)return;
		D3DVERTEXELEMENT9 ele[30];
		if(this->m_elements.size() > 0)
		{
			int count = CreateElements(ele, this->m_elements.GetPtr(), this->m_elements.size());
			m_decl = g_dev9->AddDecl(ele, count);
		}
	}
	BOOL					DeviceMesh_Device9::RenderSubset(Program* p, DWORD dwAttrib)	//渲染
	{CPUTime(DeviceMesh_Device9);
//		if(m_meshPtr == 0)return false;
		//
		DeviceProgram_Device9* prog	=	static_cast<DeviceProgram_Device9*>(p);
		if(!prog->m_shaderPtr || prog->m_hMain == 0 || this->m_nNumVertices == 0)
			return false;
		if(m_decl==-1)
			GenElements();
		else if(m_decl<-1)
			return false;
		//
		assert(m_decl>=0);
		g_dev9->ActiveDecl(p, m_decl);
		//prog->SetInt(prog->m_varAlphaTest, (g_dev9->GetAlphaMode() == AlphaMode_Test));
		BASE_CHECK_RET(prog->m_shaderPtr->SetTechnique(prog->m_hMain));
		unsigned int pass = 0;
		BASE_CHECK_RET(prog->m_shaderPtr->Begin(&pass, 0));
		BASE_CHECK_RET(prog->m_shaderPtr->BeginPass(0));
		//
		if(this->m_nNumIndices>0){
			if(dwAttrib < m_meshAttribs.size())
			{
				DirectXMeshAttribute* attr	=	m_meshAttribs[dwAttrib];
				if(attr){
					GetDevice9()->DrawIndexedPrimitiveUP((D3DPRIMITIVETYPE)Phantom::DrawMode_Triangles, 0, m_nNumVertices, attr->numFaces, attr->faces.GetPtr(), D3DFMT_INDEX16, m_vertices.GetPtr(), this->m_nStrideSize);
					GetDevicePtr()->AddRenderTriangle(attr->numFaces);
					if(g_dev9->GetCullMode() == CullMode_Double)
						GetDevicePtr()->AddRenderTriangle(attr->numFaces);
				}
			}
		}
		else
		{
			GetDevice9()->DrawPrimitiveUP((D3DPRIMITIVETYPE)Phantom::DrawMode_Triangles, m_nNumVertices/3, m_vertices.GetPtr(), this->m_nStrideSize);
			GetDevicePtr()->AddRenderTriangle(m_nNumVertices/3);
			if(g_dev9->GetCullMode() == CullMode_Double)
				GetDevicePtr()->AddRenderTriangle(m_nNumVertices/3);
		}
		//m_meshPtr->DrawSubset(dwAttrib);
		//
		BASE_CHECK_RET(prog->m_shaderPtr->EndPass());
		BASE_CHECK_RET(prog->m_shaderPtr->End());
		return true;
	}
	VOID					DeviceMesh_Device9::SetVertexFVF(INT nFVF)
	{CPUTime(DeviceMesh_Device9);
		DeviceMesh::SetVertexFVF(nFVF);
		this->m_decl = -1;
	}
	VOID					DeviceMesh_Device9::UpdateVertexBuffer(const void* vertexPtr, int vertexCount)
	{CPUTime(DeviceMesh_Device9);
		if(vertexPtr){
			m_nNumVertices = vertexCount;
			assert(vertexCount * m_nStrideSize <= m_vertices.size());
			memcpy_s(this->m_vertices.GetPtr(), vertexPtr, m_nNumVertices * this->m_nStrideSize);
		}
	}
	BOOL					DeviceMesh_Device9::RenderIndexed(Program* p, int numTriangles)
	{CPUTime(DeviceMesh_Device9);
		DeviceProgram_Device9* prog	=	static_cast<DeviceProgram_Device9*>(p);
		if(!prog->m_shaderPtr || prog->m_hMain == 0 || this->m_nNumVertices == 0 || this->m_nNumIndices == 0)
			return false;
		if(m_decl==-1)
			GenElements();
		else if(m_decl<-1)
			return false;
		assert(m_decl>=0);
		g_dev9->ActiveDecl(p, m_decl);
		//prog->SetInt(prog->m_varAlphaTest, (g_dev9->GetAlphaMode() == AlphaMode_Test));
		BASE_CHECK_RET(prog->m_shaderPtr->SetTechnique(prog->m_hMain));
		unsigned int pass = 0;
		BASE_CHECK_RET(prog->m_shaderPtr->Begin(&pass, 0));
		BASE_CHECK_RET(prog->m_shaderPtr->BeginPass(0));
		//
		assert(m_meshAttribs.size() == 1);
		DirectXMeshAttribute* attr	=	m_meshAttribs[0];
		GetDevice9()->DrawIndexedPrimitiveUP((D3DPRIMITIVETYPE)Phantom::DrawMode_Triangles, 0, m_nNumVertices, numTriangles, attr->faces.GetPtr(), D3DFMT_INDEX16, m_vertices.GetPtr(), this->m_nStrideSize);
		GetDevicePtr()->AddRenderTriangle(numTriangles);
		if(g_dev9->GetCullMode() == CullMode_Double)
			GetDevicePtr()->AddRenderTriangle(attr->numFaces);
		BASE_CHECK_RET(prog->m_shaderPtr->EndPass());
		BASE_CHECK_RET(prog->m_shaderPtr->End());
		return true;
	}
	VOID					DeviceMesh_Device9::UpdateIndexBuffer(const unsigned short* indexPtr, int indexCount)
	{CPUTime(DeviceMesh_Device9);
		if(m_meshAttribs.size() == 0)return;
		if(indexCount > (this->m_meshAttribs[0]->faces.size() * 3))//(m_meshPtr->GetNumFaces() * 3))
			indexCount = this->m_meshAttribs[0]->faces.size() * 3;//(m_meshPtr->GetNumFaces() * 3);
		memcpy(m_meshAttribs[0]->faces.GetPtr(), indexPtr, indexCount * sizeof(unsigned short));
		m_meshAttribs[0]->numFaces = indexCount / 3;
	}
	VOID					DeviceMesh_Device9::OnResetDevice()
	{CPUTime(DeviceMesh_Device9);
		OnLostDevice();
		GenElements();
	}
	VOID					DeviceMesh_Device9::OnLostDevice()
	{CPUTime(DeviceMesh_Device9);
//		safe_release(m_meshPtr);
	}
	VOID							Device_Device9::Clear(UINT flags)
	{CPUTime(Device_Device9);
		BASE_CHECK_NR(GetDevice9()->Clear(0, 0, flags, this->m_clearColor, this->m_fDepth, 0));
		g_nMinFilter = 0;
		g_nMagFilter = 0;
		g_nMipFilter = 0;
	}
	VOID					Device_Device9::SetCullMode(CullMode mode)
	{CPUTime(Device_Device9);
		//if(mode == CullMode_CCW)
		//	mode = CullMode_CW;
		//else if(mode == CullMode_CW)
		//	mode = CullMode_CCW;
		if(m_nCullMode == mode)
			return;
		m_nCullMode	=	mode;
		GetDevice9()->SetRenderState(D3DRS_CULLMODE, m_nCullMode);
	}
	VOID					Device_Device9::SetBlendMode(BlendMode src, BlendMode dest)
	{CPUTime(Device_Device9);
		if(m_nSrcBlend == src && m_nDestBlend == dest)
			return;
		m_nSrcBlend		=	src;
		m_nDestBlend	=	dest;
		GetDevice9()->SetRenderState(D3DRS_SRCBLEND, m_nSrcBlend);
		GetDevice9()->SetRenderState(D3DRS_DESTBLEND, m_nDestBlend);
	}
	VOID					Device_Device9::SetViewport(int x, int y, int w, int h)
	{CPUTime(Device_Device9);
		D3DVIEWPORT9 vp;
		vp.MinZ	=	0;
		vp.MaxZ	=	1;
		vp.X	=	x;
		vp.Y	=	y;
		vp.Width	=	w;
		vp.Height	=	h;
		GetDevice9()->SetViewport(&vp);
	}

	VOID					Device_Device9::SetSampler(SamplerState sampler, TextureFilter filter)
	{CPUTime(Device_Device9);
		if(m_samplerFilter[sampler] == filter)
			return;
		m_samplerFilter[sampler]	=	filter;
		switch(sampler)
		{
		case SAMP_MAGFILTER:
			GetDevice9()->SetSamplerState(0, D3DSAMP_MAGFILTER, filter);
			break;
		case SAMP_MINFILTER:
			GetDevice9()->SetSamplerState(0, D3DSAMP_MINFILTER, filter);
			break;
		case SAMP_MIPFILTER:
			GetDevice9()->SetSamplerState(0, D3DSAMP_MIPFILTER, filter);
			break;
		}
	}
	VOID					Device_Device9::ResetAllDefault()
	{CPUTime(Device_Device9);
		Device::ResetAllDefault();
		this->SetAlphaMode(AlphaMode_None);
		this->SetCullMode(CullMode_CCW);
		SetEnableZ(m_bEnableZ);
		SetZWriteEnable(true);
		GetDevice9()->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		GetDevice9()->SetTextureStageState( 0, D3DTSS_COLORARG0, D3DTA_DIFFUSE );
		GetDevice9()->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		GetDevice9()->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		GetDevice9()->SetTextureStageState( 0, D3DTSS_ALPHAARG0, D3DTA_DIFFUSE );
		GetDevice9()->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		GetDevice9()->SetRenderState(D3DRS_LIGHTING, FALSE);
		GetDevice9()->SetRenderState(D3DRS_FOGENABLE, D3DZB_FALSE);
		GetDevice9()->SetRenderState(D3DRS_FOGCOLOR, D3DXCOLOR(1,1,1,1));
	}
	VOID					Device_Device9::SetZWriteEnable(BOOL b)
	{CPUTime(Device_Device9);
		if(m_bZWriteEnable == b)
			return;
		m_bZWriteEnable	=	b;
		if(m_bZWriteEnable)
			GetDevice9()->SetRenderState(D3DRS_ZWRITEENABLE, true);
		else
			GetDevice9()->SetRenderState(D3DRS_ZWRITEENABLE, false);
	}
	VOID					Device_Device9::SetEnableZ(BOOL b)
	{CPUTime(Device_Device9);
		if(m_bEnableZ == b)
			return;
		m_bEnableZ	=	b;
		if(m_bEnableZ)
			GetDevice9()->SetRenderState(D3DRS_ZENABLE, true);
		else
			GetDevice9()->SetRenderState(D3DRS_ZENABLE, false);
	}
	//VOID					Device_Device9::SetSampler(SamplerState sampler, TextureFilter filter)
	//{
	//	if(m_samplerFilter[sampler] == filter)
	//		return;
	//	m_samplerFilter[sampler]	=	filter;
	//	switch(sampler)
	//	{
	//	case SAMP_MAGFILTER:
	//		GetDevice9()->SetSamplerState(0, D3DSAMP_MAGFILTER, filter);
	//		break;
	//	case SAMP_MINFILTER:
	//		GetDevice9()->SetSamplerState(0, D3DSAMP_MINFILTER, filter);
	//		break;
	//	case SAMP_MIPFILTER:
	//		GetDevice9()->SetSamplerState(0, D3DSAMP_MIPFILTER, filter);
	//		break;
	//	}
	//}
	VOID					Device_Device9::SetAlphaMode(AlphaMode mode)
	{CPUTime(Device_Device9);
		if(this->m_nAlphaMode == mode)
			return;
		m_nAlphaMode	=	mode;
		if(m_nAlphaMode > AlphaMode_None)
		{
			GetDevice9()->SetRenderState(D3DRS_ALPHATESTENABLE, m_nAlphaMode == AlphaMode_Test);
			GetDevice9()->SetRenderState(D3DRS_ALPHABLENDENABLE, m_nAlphaMode != AlphaMode_Test);
		}
		else
		{
			GetDevice9()->SetRenderState(D3DRS_ALPHATESTENABLE, false);
			GetDevice9()->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		}
		if(m_nAlphaMode == Phantom::AlphaMode_Blend)
			SetBlendMode(Phantom::BlendMode_SRCALPHA, Phantom::BlendMode_INVSRCALPHA);
	}
	const void*						Device_Device9::SetDrawInfo(Program* prog, const DrawVInfos& info, int& strideSize)
	{CPUTime(Device_Device9);
		static CDynamicArray<PosNormalColorUV>		posNormalColorUV;
		static CDynamicArray<PosColor>		posColor;
		static CDynamicArray<PosColorUV>	posColorUV;
		static CDynamicArray<PosUV>			posUV;
		const void*	vertices	=	0;
		strideSize	=	0;
		if(info.colorList && info.uvList && info.normalList)
		{
			posNormalColorUV.SetArrayCount(info.m_numVertices);
			for(int i=0;i<info.m_numVertices;i++)
			{
				posNormalColorUV[i].p		=	info.posList[i];
				posNormalColorUV[i].n		=	info.normalList[i];
				posNormalColorUV[i].diffuse	=	info.colorList[i];
				posNormalColorUV[i].tu		=	info.uvList[i].x;
				posNormalColorUV[i].tv		=	info.uvList[i].y;
			}
			ActiveDecl(prog, m_declNormalColorUV);
			vertices	=	posNormalColorUV.GetPtr();
			strideSize	=	sizeof(PosNormalColorUV);
		}
		else if(info.colorList && info.uvList)
		{
			posColorUV.SetArrayCount(info.m_numVertices);
			for(int i=0;i<info.m_numVertices;i++)
			{
				posColorUV[i].pos	=	info.posList[i];
				posColorUV[i].color	=	info.colorList[i];
				posColorUV[i].uv	=	info.uvList[i];
			}
			ActiveDecl(prog, m_declColorUV);
			vertices	=	posColorUV.GetPtr();
			strideSize	=	sizeof(PosColorUV);
		}
		else if(info.uvList)
		{
			posUV.SetArrayCount(info.m_numVertices);
			for(int i=0;i<info.m_numVertices;i++)
			{
				posUV[i].pos	=	info.posList[i];
				posUV[i].uv		=	info.uvList[i];
			}
			ActiveDecl(prog, m_declUV);
			vertices	=	posUV.GetPtr();
			strideSize	=	sizeof(PosUV);
		}
		else// if(info.colorList)
		{
			posColor.SetArrayCount(info.m_numVertices);
			for(int i=0;i<info.m_numVertices;i++)
			{
				posColor[i].pos		=	info.posList[i];
				if(info.colorList)
					posColor[i].color	=	info.colorList[i];
				else
					posColor[i].color	=	0xffffffff;//.setRGBA(1,1,1,1);
			}
			ActiveDecl(prog, m_declColor);
			vertices	=	posColor.GetPtr();
			strideSize	=	sizeof(PosColor);
		}
		return vertices;
	}
	//VOID							Device_Device9::DrawIndexed(Program* prog, DrawMode mode, const void* vertexData,const void* indices, int numVertex, int numDrawFaces, unsigned int vertexTypeSize, unsigned int uFVF)
	//{
	//	if(!prog)
	//		prog	=	g_manager.GetCurrProgram();
	//	DeviceProgram_Device9* shader	=	static_cast<DeviceProgram_Device9*>(prog);
	//	if(!shader->m_shaderPtr)
	//		return;
	//	IDirect3DDevice9* pDevice = GetDevice9();
	//	//
	//	pDevice->SetFVF(uFVF);
	//	shader->m_shaderPtr->SetTechnique(shader->m_hMain);
	//	unsigned int pass = 0;
	//	shader->m_shaderPtr->Begin(&pass, 0);
	//	shader->m_shaderPtr->BeginPass(0);
	//	GetDevice9()->DrawIndexedPrimitiveUP((D3DPRIMITIVETYPE)mode, 0,(UINT)numVertex,(UINT)numDrawFaces,indices,D3DFMT_INDEX16,vertexData,(UINT)vertexTypeSize);
	//	shader->m_shaderPtr->EndPass();
	//	shader->m_shaderPtr->End();
	//	AddRenderTriangle(numDrawFaces);
	//	if(g_dev9->GetCullMode() == CullMode_Double)
	//		GetDevicePtr()->AddRenderTriangle(numDrawFaces);
	//}
	//VOID							DynamicMesh::DrawOnly()	//渲染
	//{
	//	Program* prog	=	g_manager.GetCurrProgram();
	//	if(m_nNumVertices <= 0)return;
	//	DeviceProgram_Device9* shader	=	static_cast<DeviceProgram_Device9*>(prog);
	//	if(!shader->m_shaderPtr)
	//		return;
	//	//
	//	GetDevicePtr()->SetFVF(this->m_nFVF);
	//	int nStrideSize	=	0;
	//	int numFace = 0;
	//	int numIndices	=	m_triangles.size() * 3;
	//	if(numIndices <= 0)
	//		numIndices	=	m_nNumVertices;
	//	//
	//	switch(this->m_drawMode)
	//	{
	//	case DrawMode_Points:
	//		numFace	=	numIndices;
	//		break;
	//	case DrawMode_Lines:
	//		numFace	=	numIndices / 2;
	//		break;
	//	case DrawMode_LineStrip:
	//		numFace	=	numIndices - 1;
	//		break;
	//	case DrawMode_Triangles:
	//		numFace	=	numIndices / 3;
	//		break;
	//	case DrawMode_TriangleStrip:
	//		numFace	=	numIndices - 2;
	//		break;
	//	case DrawMode_TriangleFan:
	//		numFace	=	numIndices - 2;
	//		break;
	//	}
	//	shader->m_shaderPtr->SetTechnique(shader->m_hMain);
	//	unsigned int pass = 0;
	//	shader->m_shaderPtr->Begin(&pass, 0);
	//	shader->m_shaderPtr->BeginPass(0);
	//	if(m_triangles.size() > 0)
	//	{
	//		if(m_attributes.size() == 0)//m_trianglesRenders.size() != m_triangles.size())
	//		{
	//			g_manager.GetCurrProgram()->SetTexture(TextureIndex_0, 0);
	//			shader->m_shaderPtr->CommitChanges();
	//			GetDevice9()->DrawIndexedPrimitiveUP((D3DPRIMITIVETYPE)m_drawMode, 0, this->m_nNumVertices, numFace, m_triangles.GetPtr(), D3DFMT_INDEX16, m_vertices.GetPtr(), m_nStrideSize);
	//		}
	//		else
	//		{
	//			for(int i=0;i<m_attributes.size();i++)
	//			{
	//				MeshAttribute& m = m_attributes[i];
	//				g_manager.GetCurrProgram()->SetTexture(TextureIndex_0, m.texPtr);
	//				shader->m_shaderPtr->CommitChanges();
	//				if(m.count == 0)
	//					continue;
	//				GetDevice9()->DrawIndexedPrimitiveUP((D3DPRIMITIVETYPE)m_drawMode, 0, this->m_nNumVertices, m.count, m_trianglesRenders.GetPtr() + m.beginIndex, D3DFMT_INDEX16, m_vertices.GetPtr(), m_nStrideSize);
	//			}
	//		}
	//	}
	//	else
	//	{
	//		if(m_attributes.size() == 0)//m_trianglesRenders.size() != m_triangles.size())
	//		{
	//			g_manager.GetCurrProgram()->SetTexture(TextureIndex_0, 0);
	//			shader->m_shaderPtr->CommitChanges();
	//			GetDevice9()->DrawPrimitiveUP((D3DPRIMITIVETYPE)m_drawMode, numFace, this->m_vertices.GetPtr(), this->m_nStrideSize);
	//		}
	//		else
	//		{
	//			for(int i=0;i<m_attributes.size();i++)
	//			{
	//				MeshAttribute& m = m_attributes[i];
	//				g_manager.GetCurrProgram()->SetTexture(TextureIndex_0, m.texPtr);
	//				shader->m_shaderPtr->CommitChanges();
	//				if(m.count == 0)
	//					continue;
	//				GetDevice9()->DrawPrimitiveUP((D3DPRIMITIVETYPE)m_drawMode, m.count, this->m_vertices.GetPtr() + m.beginIndex, this->m_nStrideSize);
	//			}
	//		}
	//		//GetDevice9()->DrawPrimitiveUP((D3DPRIMITIVETYPE)m_drawMode, numFace, this->m_vertices.GetPtr(), this->m_nStrideSize);
	//	}
	//	//
	//	shader->m_shaderPtr->EndPass();
	//	shader->m_shaderPtr->End();
	//	GetDevicePtr()->AddRenderTriangle(numFace);
	//	if(g_dev9->GetCullMode() == CullMode_Double)
	//		GetDevicePtr()->AddRenderTriangle(numFace);
	//}
	int								Device_Device9::AddDecl(UINT nFVF)
	{CPUTime(Device_Device9);
		D3DVERTEXELEMENT9 ele[30];
		int count = FillElemenetByFVF(ele, nFVF);
		return AddDecl(ele, count);
	}
	int								Device_Device9::AddDecl(D3DVERTEXELEMENT9* ele, int numElement)
	{CPUTime(Device_Device9);
		for(int i=0;i<m_decls.size();i++)
		{
			if(m_decls[i].ndecl != numElement)
				continue;
			Decl& d = m_decls[i];
			BOOL bFind = true;
			for(int j=0;j<numElement;j++)
			{
				D3DVERTEXELEMENT9& v1 = d.vdecl[j];
				D3DVERTEXELEMENT9& v2 = ele[j];
				if(v1.Method != v2.Method || v1.Offset != v2.Offset || v1.Stream != v2.Stream ||
					v1.Type != v2.Type || v1.Usage != v2.Usage || v1.UsageIndex != v2.UsageIndex)
				{
					bFind = false;
					break;
				}
			}
			if(!bFind)
				continue;
			if(GetDevice9()&&d.decl == 0)
				GetDevice9()->CreateVertexDeclaration(d.vdecl, &d.decl);
			return i;
		}
		assert(numElement < 16);
		int newsize = m_decls.size();
		m_decls.SetArrayCount(newsize+1);
		Decl decl;
		decl.decl = 0;
		decl.ndecl = numElement;
		memcpy(decl.vdecl, ele, sizeof(D3DVERTEXELEMENT9)*numElement);
		if(GetDevice9())
			GetDevice9()->CreateVertexDeclaration(decl.vdecl, &decl.decl);
		m_decls[newsize] = decl;
		return newsize;
	}
	VOID							Device_Device9::ActiveDecl(Program* prog, int index)
	{CPUTimeNone();
		if(index<0||index>=m_decls.size())
			return;
		if(prog == this->m_lastProgDecl&&index == this->m_lastDecl)
			return;
		m_lastProgDecl = prog;
		m_lastDecl = index;
		GetDevice9()->SetVertexDeclaration(m_decls[index].decl);
	}
	VOID							Device_Device9::DrawIndexed(Program* prog, const DrawVInfos& info, const unsigned short* tList, int numIndices, DrawMode mode)
	{CPUTime(Device_Device9);
		if(!prog)
			prog	=	g_manager.GetCurrProgram();
		if(!info.posList || !prog)return;
		DeviceProgram_Device9* shader	=	static_cast<DeviceProgram_Device9*>(prog);
		if(!shader->m_shaderPtr)
			return;
		//
		int nStrideSize	=	0;
		const void* vertices	=	SetDrawInfo(prog, info, nStrideSize);
		int numFace = 0;
		switch(mode)
		{
		case DrawMode_Points:
			numFace	=	numIndices;
			break;
		case DrawMode_Lines:
			numFace	=	numIndices / 2;
			break;
		case DrawMode_LineStrip:
			numFace	=	numIndices - 1;
			break;
		case DrawMode_Triangles:
			numFace	=	numIndices / 3;
			break;
		case DrawMode_TriangleStrip:
			numFace	=	numIndices - 2;
			break;
		case DrawMode_TriangleFan:
			numFace	=	numIndices - 2;
			break;
		}
		shader->m_shaderPtr->SetTechnique(shader->m_hMain);
		unsigned int pass = 0;
		shader->m_shaderPtr->Begin(&pass, 0);
		shader->m_shaderPtr->BeginPass(0);
		GetDevice9()->DrawIndexedPrimitiveUP((D3DPRIMITIVETYPE)mode, 0, info.m_numVertices, numFace, tList, D3DFMT_INDEX16, vertices, nStrideSize);
		shader->m_shaderPtr->EndPass();
		shader->m_shaderPtr->End();
		AddRenderTriangle(numFace);
		if(g_dev9->GetCullMode() == CullMode_Double)
			GetDevicePtr()->AddRenderTriangle(numFace);
	}
	VOID							Device_Device9::DrawNoIndexed(Program* prog, const DrawVInfos& info, int numVertices, DrawMode mode)
	{CPUTime(Device_Device9);
		if(!prog)
			prog	=	g_manager.GetCurrProgram();
		if(!info.posList || !prog)return;
		DeviceProgram_Device9* shader	=	static_cast<DeviceProgram_Device9*>(prog);
		if(!shader->m_shaderPtr)
			return;
		//
		int nStrideSize	=	0;
		const void* vertices	=	SetDrawInfo(prog, info, nStrideSize);
		int numFace = 0;
		switch(mode)
		{
		case DrawMode_Points:
			numFace	=	numVertices;
			break;
		case DrawMode_Lines:
			numFace	=	numVertices / 2;
			break;
		case DrawMode_LineStrip:
			numFace	=	numVertices - 1;
			break;
		case DrawMode_Triangles:
			numFace	=	numVertices / 3;
			break;
		case DrawMode_TriangleStrip:
			numFace	=	numVertices - 2;
			break;
		case DrawMode_TriangleFan:
			numFace	=	numVertices - 2;
			break;
		}

		shader->m_shaderPtr->SetTechnique(shader->m_hMain);
		unsigned int pass = 0;
		shader->m_shaderPtr->Begin(&pass, 0);
		shader->m_shaderPtr->BeginPass(0);
		GetDevice9()->DrawPrimitiveUP((D3DPRIMITIVETYPE)mode, numFace, vertices, nStrideSize);
		shader->m_shaderPtr->EndPass();
		shader->m_shaderPtr->End();
		AddRenderTriangle(numFace);
		if(g_dev9->GetCullMode() == CullMode_Double)
			GetDevicePtr()->AddRenderTriangle(numFace);
	}

	VOID					Device_Device9::OnLostDevice()
	{CPUTime(Device_Device9);
		m_lastProgDecl = 0;
		for(int i=0;i<m_decls.size();i++)
		{
			safe_release(m_decls[i].decl);
		}
	}
	VOID					Device_Device9::OnResetDevice()
	{CPUTime(Device_Device9);
		OnLostDevice();
		for(int i=0;i<m_decls.size();i++){
			GetDevice9()->CreateVertexDeclaration(m_decls[i].vdecl, &m_decls[i].decl);
		}
	}
	RenderText*									Device_Device9::CreateRenderText()
	{CPUTime(Device_Device9);
		return new DeviceRenderText_Device9();
	}
	VOID										Device_Device9::SetUIClipRect(const Rect* rect)//设置UI裁剪,如果是0则不做任何裁剪
	{CPUTime(Device_Device9);
		if(rect)
		{
			GetDevice9()->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
			GetDevice9()->SetScissorRect((RECT*)rect);
		}
		else
		{
			GetDevice9()->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
		}
	}
	Program*									Device_Device9::CreateProgram(int mode){CPUTime(Device_Device9);return new DeviceProgram_Device9(mode);}
	classT<Texture, classMgr<Texture> >*		Device_Device9::CreateTexture(){return new DeviceTexture_Device9();}
	classT<DeviceMesh, classMgr<DeviceMesh> >*	Device_Device9::CreateMesh(){return new DeviceMesh_Device9();}
	Device*			CreateDevice()
	{
		Device_Device9* dev	=	new Device_Device9();
		return dev;
	}
	BOOL	SaveScreenTo(const char* szFile){
		IDirect3DSurface9* surf = 0;
		if(FAILED(GetDevice9()->GetRenderTarget(0, &surf)))
			return false;
		D3DXSaveSurfaceToFileA(szFile,D3DXIFF_JPG,surf,0,0);
		surf->Release();
		return true;
	}
};

#endif
