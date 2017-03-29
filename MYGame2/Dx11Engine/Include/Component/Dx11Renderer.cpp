#include "Dx11Renderer.h"
#include "../Resources/Dx11Mesh.h"
#include "../Rendering/Dx11Shader.h"
#include "../Resources/Dx11ResourcesManager.h"
#include "../Rendering/Dx11ShaderManager.h"
#include "../Scene/Dx11Scene.h"
#include "Dx11Camera.h"
#include "Dx11Transform.h"
#include "../GameObject/Dx11GameObject.h"
#include "Dx11Material.h"
#include "Dx11Light.h"
#include "../Rendering/Dx11RenderState.h"
#include "../Rendering/Dx11RenderManager.h"
#include "Dx11Animation2D.h"

DX11_USING

CDx11Renderer::CDx11Renderer()	:
	m_pMesh(NULL),
	m_pShader(NULL)	
{
	m_eComType = CT_RENDERER;
	m_eRenderType = RT_NORMAL;
	memset(m_pRenderState, 0, sizeof(CDx11RenderState*) * RST_END);
}

CDx11Renderer::CDx11Renderer(const CDx11Renderer & renderer)	:
	CDx11Component(renderer)
{
	*this = renderer;
	m_iRefCount = 1;

	if (m_pShader)
		m_pShader->AddRef();

	if (m_pMesh)
		m_pMesh->AddRef();

	for (size_t i = 0; i < m_vecMaterial.size(); ++i)
	{
		for (size_t j = 0; j < m_vecMaterial[i].size(); ++j)
		{
			m_vecMaterial[i][j]->AddRef();
		}
	}

	for (int i = 0; i < RST_END; ++i)
	{
		if (m_pRenderState[i])
			m_pRenderState[i]->AddRef();
	}
}

CDx11Renderer::~CDx11Renderer()
{
	for (int i = 0; i < RST_END; ++i)
	{
		SAFE_RELEASE(m_pRenderState[i]);
	}

	for (size_t i = 0; i < m_vecMaterial.size(); ++i)
	{
		Safe_Release_VecList(m_vecMaterial[i]);
	}

	SAFE_RELEASE(m_pMesh);
	SAFE_RELEASE(m_pShader);
}

RENDER_TYPE CDx11Renderer::GetRenderType() const
{
	return m_eRenderType;
}

void CDx11Renderer::SetRenderType(RENDER_TYPE eRenderType)
{
	m_eRenderType = eRenderType;
}

CDx11Material * CDx11Renderer::GetMaterial(int iContainer, int iSubset)	const
{
	if (m_vecMaterial.size() <= iContainer)
		return NULL;

	else if (m_vecMaterial[iContainer].size() <= iSubset)
		return NULL;

	m_vecMaterial[iContainer][iSubset]->AddRef();
	return m_vecMaterial[iContainer][iSubset];
}

bool CDx11Renderer::Init()
{
	return true;
}

int CDx11Renderer::Update(float fTime)
{
	return 0;
}

int CDx11Renderer::LateUpdate(float fTime)
{
	return 0;
}

void CDx11Renderer::Render(float fTime)
{
	//트랜스폼 상수버퍼 업뎃
	SetTransformCBuffer();

	//빛 상수버퍼 업뎃
	SetLightCBuffer();

	//물질 상수버퍼 업뎃
	//SetMaterialCBuffer();

	//애니메이션(현재 2D)
	UpdateAnimation();

	//안개 상수버퍼 업뎃
	SetFogCBuffer();

	// 렌더상태 설정
	for (int i = 0; i < RST_END; ++i)
	{
		if (m_pRenderState[i])
			m_pRenderState[i]->SetState();
	}

	m_pShader->SetShader();

	for (int i = 0; i < m_vecMaterial.size(); ++i)
	{
		for (int j = 0; j < m_vecMaterial[i].size(); ++j)
		{
			m_vecMaterial[i][j]->SetMaterial(m_pShader);
			m_pMesh->Render(i, j);
		}
	}

	for (int i = 0; i < RST_END; ++i)
	{
		if (m_pRenderState[i])
			m_pRenderState[i]->ResetState();
	}

	m_pShader->End();
}

CDx11Renderer * CDx11Renderer::Clone()
{
	return new CDx11Renderer(*this);
}

void CDx11Renderer::SetMesh(const string & strKey)
{
	SAFE_RELEASE(m_pMesh);
	m_pMesh = DX11_GET_SINGLE(CDx11ResourcesManager)->FindMesh(strKey);
	
	if (m_pMesh->IsMaterial())
	{
		for (int i = 0; i < m_vecMaterial.size(); ++i)
		{
			Safe_Release_VecList(m_vecMaterial[i]);
		}

		m_vecMaterial.clear();

		for (int i = 0; i < m_pMesh->GetContainerCount(); ++i)
		{
			vector<CDx11Material*>	vecMtrl;

			for (int j = 0; j < m_pMesh->GetSubsetCount(i); ++j)
			{
				CDx11Material*	pMtrl = m_pMesh->CloneMaterial(i, j);

				vecMtrl.push_back(pMtrl);
			}

			m_vecMaterial.push_back(vecMtrl);
		}
	}
	
	
}

void CDx11Renderer::SetShader(const string & strKey)
{
	SAFE_RELEASE(m_pShader);
	m_pShader = DX11_GET_SINGLE(CDx11ShaderManager)->FindShader(strKey);
}

void CDx11Renderer::SetMaterial(CDx11Material * pMtrl, int iContainer)
{
	if (m_vecMaterial.size() <= iContainer)
		return;

	m_vecMaterial[iContainer].push_back(pMtrl);
}

void CDx11Renderer::CreateMaterial(const string & strDiffuseTexKey, int iContainer,
	const string & strDiffuseSampKey)
{
	if (m_vecMaterial.size() <= iContainer)
	{
		vector<CDx11Material*>	vecMtrl;
		m_vecMaterial.push_back(vecMtrl);
	}

	CDx11Material*	pMtrl = new CDx11Material;

	pMtrl->Init();

	pMtrl->SetDiffuseTexture(strDiffuseTexKey);
	pMtrl->SetDiffuseSampler(strDiffuseSampKey);

	m_vecMaterial[iContainer].push_back(pMtrl);
}

void CDx11Renderer::CreateMaterial(const string & strDiffuseTexKey,
	TCHAR * pFileName, int iContainer, const string & strPathKey,
	const string & strDiffuseSampKey)
{
	if (m_vecMaterial.size() <= iContainer)
	{
		vector<CDx11Material*>	vecMtrl;
		m_vecMaterial.push_back(vecMtrl);
	}

	CDx11Material*	pMtrl = new CDx11Material;

	pMtrl->Init();

	pMtrl->SetDiffuseTexture(strDiffuseTexKey, pFileName, strPathKey);
	pMtrl->SetDiffuseSampler(strDiffuseSampKey);

	m_vecMaterial[iContainer].push_back(pMtrl);
}

void CDx11Renderer::SetRenderState(const string & strKey)
{
	CDx11RenderState*	pRS = DX11_GET_SINGLE(CDx11RenderManager)->FindRenderState(strKey);

	if (!pRS)
		return;

	SAFE_RELEASE(m_pRenderState[pRS->GetStateType()]);
	m_pRenderState[pRS->GetStateType()] = pRS;

	if (pRS->GetStateType() == RST_BLEND && m_eRenderType == RT_NORMAL)
	{
		m_eRenderType = RT_ALPHA1;
	}
}

void CDx11Renderer::SetRenderState(const string & strKey,RENDER_STATE_TYPE eFlag)
{
	SAFE_RELEASE(m_pRenderState[eFlag]);
	m_pRenderState[eFlag] = DX11_GET_SINGLE(CDx11RenderManager)->FindRenderState(strKey);
}

void CDx11Renderer::UpdateAnimation()
{
	CDx11Animation2D*	pAnimation = (CDx11Animation2D*)m_pGameObject->FindComponentFromType(CT_ANIMATION2D);

	if (pAnimation)
	{
		UVFRAME		tFrame = {};

		tFrame.iFrameX = pAnimation->GetFrameX();
		tFrame.iFrameY = pAnimation->GetFrameY();
		tFrame.iFrameMaxX = pAnimation->GetMaxFrameX();
		tFrame.iFrameMaxY = pAnimation->GetMaxFrameY();
		tFrame.iAniType = pAnimation->GetAnimationType();

		m_pShader->UpdateConstantBuffer("UVFrame", &tFrame, ST_VERTEX);

		SAFE_RELEASE(pAnimation);
	}
}

void CDx11Renderer::SetTransformCBuffer()
{
	TRANSFORMCBUFFER	tBuffer = {};

	// 카메라의 뷰와 투영 행렬을 얻어온다.
	CDx11GameObject*	pCameraObj = m_pScene->GetMainCamera();
	CDx11Camera*	pCamera = (CDx11Camera*)pCameraObj->FindComponentFromTypeName("CDx11Camera");
	tBuffer.matView = pCamera->GetViewMatrix();
	tBuffer.matProj = pCamera->GetProjMatrix();

	SAFE_RELEASE(pCamera);
	SAFE_RELEASE(pCameraObj);

	tBuffer.matWorld = m_pTransform->GetWorldMatrix();

	tBuffer.matWV = tBuffer.matWorld * tBuffer.matView;
	tBuffer.matWVP = tBuffer.matWV * tBuffer.matProj;

	tBuffer.matWorld = XMMatrixTranspose(tBuffer.matWorld);
	tBuffer.matView = XMMatrixTranspose(tBuffer.matView);
	tBuffer.matProj = XMMatrixTranspose(tBuffer.matProj);

	tBuffer.matWV = XMMatrixTranspose(tBuffer.matWV);
	tBuffer.matWVP = XMMatrixTranspose(tBuffer.matWVP);

	m_pShader->UpdateConstantBuffer("Transform", &tBuffer, ST_VERTEX);
	m_pShader->UpdateConstantBuffer("Transform", &tBuffer, ST_PIXEL);
}

void CDx11Renderer::SetLightCBuffer()
{
	list<CDx11GameObject*>*	pLightList = m_pScene->GetLightList();

	list<CDx11GameObject*>::iterator	iter;
	list<CDx11GameObject*>::iterator	iterEnd = pLightList->end();

	for (iter = pLightList->begin(); iter != iterEnd; ++iter)
	{
		CDx11Light*	pLight = (CDx11Light*)(*iter)->FindComponentFromType(CT_LIGHT);

		pLight->SetLight(m_pShader);

		SAFE_RELEASE(pLight);
	}
}

void CDx11Renderer::SetMaterialCBuffer()
{
	//if (m_pMaterial)
	//	m_pMaterial->SetMaterial(m_pShader);
}

void CDx11Renderer::SetFogCBuffer()
{	
	// 카메라의 뷰와 투영 행렬을 얻어온다.
	//CDx11GameObject* PlayerObj = m_pScene->FindObjectFromTag("PlayerObject");
	//CDx11Transform *PlayerTr = PlayerObj->GetTransform();
		
	FogINFO	tFogBuffer = {};
	Vec3 TempPos = m_pTransform->GetWorldPos();

	tFogBuffer.gFogStart = 0.00f;
	tFogBuffer.distToEye = TempPos;
	tFogBuffer.gFogRange = 8.f;
	tFogBuffer.gFogColor = Vec3(0.5f, 0.5f, 0.5f);

	//Vec4 asdf = Vec4(tFogBuffer.gFogColor.x, tFogBuffer.gFogColor.y, tFogBuffer.gFogColor.z, 1.f);
	//float qwer = ((tFogBuffer.distToEye.z - tFogBuffer.gFogStart) / tFogBuffer.gFogRange);	
	//int a = 0;

	m_pShader->UpdateConstantBuffer("FogInfo", &tFogBuffer, ST_PIXEL);	
}
