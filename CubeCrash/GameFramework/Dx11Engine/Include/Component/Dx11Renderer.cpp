#include "Dx11Renderer.h"
#include "../Resources/Dx11Mesh.h"
#include "../Resources/Dx11ResourcesManager.h"
#include "../Rendering/Dx11Shader.h"
#include "../Rendering/Dx11ShaderManager.h"
#include "Dx11Transform.h"
#include "Dx11Camera.h"
#include "../GameObject/Dx11GameObject.h"
#include "../Scene/Dx11Scene.h"
#include "Dx11Material.h"
#include "Dx11Light.h"
#include "../Rendering/Dx11RenderState.h"
#include "../Rendering/Dx11RenderManager.h"
#include "Dx11Animation2D.h"
#include "Dx11Animation3D.h"
#include "../Resources/Dx11Texture.h"

DX11_USING

CDx11Renderer::CDx11Renderer() :
	m_pMesh(NULL),
	m_pShader(NULL),
	m_pBoneTexture(NULL)
{
	m_eComType = CT_RENDERER;
	m_eRenderType = RTF_NORMAL;
	memset(m_pRenderState, 0, sizeof(CDx11RenderState*) * RST_END);
}


CDx11Renderer::CDx11Renderer(const CDx11Renderer & renderer)	:
	CDx11Component(renderer)
{
	*this = renderer;
	m_iRefCount = 1;


//	사용한다면 공유해서 사용하므로 참조카운트를 증가시켜줘야한다.
	if (m_pShader)
		m_pShader->AddRef();

	if (m_pMesh)
		m_pMesh->AddRef();

	for (int i = 0; i < RST_END; ++i)
	{
		if (m_pRenderState[i])
			m_pRenderState[i]->AddRef();
	}


//	재질 복사
//	m_vecMaterial.clear();
	for (size_t i = 0; i < m_vecMaterial.size(); ++i)
	{
		for (size_t j = 0; j < m_vecMaterial[i].size(); ++j)
		{
			m_vecMaterial[i][j]->AddRef();
		}
	}

//	애니메이션 복사
	m_mapCBuffer.clear();
	unordered_map<string, PCBUFFER>::const_iterator	iter;
	unordered_map<string, PCBUFFER>::const_iterator iterEnd = m_mapCBuffer.end();

	for (iter = m_mapCBuffer.begin(); iter != iterEnd; ++iter)
	{
		PCBUFFER	pBuffer = new CBUFFER;
		pBuffer->strKey = iter->second->strKey;
		pBuffer->iSize = iter->second->iSize;
		pBuffer->pData = new char[pBuffer->iSize];
		pBuffer->iShaderType = iter->second->iShaderType;
		memcpy(pBuffer->pData, iter->second->pData, pBuffer->iSize);

		m_mapCBuffer.insert(make_pair(iter->first, pBuffer));
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

	unordered_map<string, PCBUFFER>::iterator	iter;
	unordered_map<string, PCBUFFER>::iterator	iterEnd = m_mapCBuffer.end();
	for (iter = m_mapCBuffer.begin(); iter != iterEnd; ++iter)
	{
		SAFE_DELETE_ARRAY(iter->second->pData);
	}
	Safe_Delete_Map(m_mapCBuffer);


	SAFE_RELEASE(m_pMesh);
	SAFE_RELEASE(m_pShader);
}

RENDER_TYPE_FLAG CDx11Renderer::GetRenderType() const
{
	return m_eRenderType;
}

void CDx11Renderer::SetRenderType(RENDER_TYPE_FLAG eRenderType)
{
	m_eRenderType = eRenderType;
}

CDx11Material * CDx11Renderer::GetMaterial(int iContainer, int iSubset) const
{
	if (m_vecMaterial.size() <= iContainer)
		return NULL;

	else if (m_vecMaterial[iContainer].size() <= iSubset)
		return NULL;

	m_vecMaterial[iContainer][iSubset]->AddRef();
	return m_vecMaterial[iContainer][iSubset];
}

bool CDx11Renderer::AddCBuffer(const string & strKey,
	const string & strCBufferKey, int iSize, int iShaderType)
{
	if (FindCBuffer(strKey))
		return false;

	PCBUFFER	pBuffer = new CBUFFER;

	pBuffer->strKey = strCBufferKey;
	pBuffer->iSize = iSize;
	pBuffer->pData = new char[iSize];
	pBuffer->iShaderType = iShaderType;

	m_mapCBuffer.insert(make_pair(strKey, pBuffer));

	return true;
}

PCBUFFER CDx11Renderer::FindCBuffer(const string & strKey)
{
	unordered_map<string, PCBUFFER>::const_iterator	iter = m_mapCBuffer.find(strKey);

	if (iter == m_mapCBuffer.end())
		return NULL;

	return iter->second;
}

bool CDx11Renderer::UpdateCBuffer(const string & strKey, 
	void * pData, int iSize)
{
	PCBUFFER	pBuffer = FindCBuffer(strKey);

	if (!pBuffer)
		return false;
	memcpy(pBuffer->pData, pData, iSize);

	return true;
}

void CDx11Renderer::SetAnimCBuffer()
{
	unordered_map<string, PCBUFFER>::iterator iter;
	unordered_map<string, PCBUFFER>::iterator iterEnd = m_mapCBuffer.end();

	for (iter = m_mapCBuffer.begin(); iter != iterEnd; ++iter)
	{
		m_pShader->UpdateConstantBuffer(iter->second->strKey,
			iter->second->pData, (SHADER_TYPE)iter->second->iShaderType);
	}
}

void CDx11Renderer::SetBoneTexture(CDx11Texture * pBoneTex)
{
	m_pBoneTexture = pBoneTex;
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
	SetTransformCBuffer();
	SetLightCBuffer(m_pGameObject);
	SetAnimationCBuffer();
	SetAnimCBuffer();

	// 렌더 상태 설정
	for (int i = 0; i < RST_END; ++i)
	{
		if (m_pRenderState[i])
			m_pRenderState[i]->SetState();
	}

	m_pShader->SetShader();

	if (m_pBoneTexture)
		m_pBoneTexture->SetTexture(3);

	// 재질 설정 후 렌더링
	if (m_vecMaterial.size() == 0)
	{
		m_pMesh->Render();
	}
	for (int i = 0; i < m_vecMaterial.size(); ++i)
	{
		for (int j = 0; j < m_vecMaterial[i].size(); ++j)
		{
			m_vecMaterial[i][j]->SetMaterial(m_pShader);
			m_pMesh->Render(i, j);
		}
	}

	// 렌더 상태 재설정(초기화)
	for (int i = 0; i < RST_END; ++i)
	{
		if (m_pRenderState[i])
			m_pRenderState[i]->ResetState();
	}

	//쉐이더 다 썼으면 End() 호출.
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

	if (m_pMesh)
	{
		// 애니메이션이 있을 경우 복사해준다.
		CDx11Animation3D*	pAnimation = m_pMesh->CloneAnimationController();

		if (pAnimation)
		{
			m_pGameObject->AddComponent(pAnimation);
			SAFE_RELEASE(pAnimation);

			AddCBuffer("BoneArray", "BoneArray", sizeof(BONECBUFFER),
				ST_VERTEX);
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


void CDx11Renderer::CreateMaterial(const string & strDiffuseTexKey, 
	int iContainer, const string & strDiffuseSampKey, const string& strPointSampKey)
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
	pMtrl->SetPointSampler(strPointSampKey);

	m_vecMaterial[iContainer].push_back(pMtrl);

}

void CDx11Renderer::CreateMaterial(const string & strDiffuseTexKey, 
	wchar_t * pFileName, int iContainer, const string & strPathKey, 
	const string & strDiffuseSampKey, const string& strPointSampKey)
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
	pMtrl->SetPointSampler(strPointSampKey);

	m_vecMaterial[iContainer].push_back(pMtrl);
}

void CDx11Renderer::SetRenderState(const string & strKey)
{
	CDx11RenderState*	pRS = DX11_GET_SINGLE(CDx11RenderManager)->FindRenderState(strKey);

	if (!pRS)
		return;

	SAFE_RELEASE(m_pRenderState[pRS->GetStateType()]);
	m_pRenderState[pRS->GetStateType()] = pRS;

	// Blend 상태이면서 알파가 아닐 때 알파로 설정.
	if (pRS->GetStateType() == RST_BLEND && m_eRenderType == RTF_NORMAL)
	{
		m_eRenderType = RTF_ALPHA1;
	}
	
}

void CDx11Renderer::SetTransformCBuffer()
{
	TRANSFORMCBUFFER	tBuffer = {};

	//카메라의 뷰 행렬, 투영 행렬을 얻어온다.
	CDx11GameObject*	pCameraObj = m_pScene->GetMainCamera();
	CDx11Camera*	pCamera = (CDx11Camera*)pCameraObj->FindComponentFromTypeName("CDx11Camera");
	tBuffer.matView = pCamera->GetViewMatrix();
	tBuffer.matProj = pCamera->GetProjMatrix();

	SAFE_RELEASE(pCamera);
	SAFE_RELEASE(pCameraObj);

	//월드 행렬을 얻어온다.
	tBuffer.matWorld = m_pTransform->GetWorldMatrix();
	tBuffer.matWV = tBuffer.matWorld * tBuffer.matView;
	tBuffer.matWVP = tBuffer.matWV * tBuffer.matProj;

	//상수버퍼로 갱신하기전에 행렬을 모두 전치해주어야한다.
	tBuffer.matWorld = XMMatrixTranspose(tBuffer.matWorld);
	tBuffer.matView = XMMatrixTranspose(tBuffer.matView);
	tBuffer.matProj = XMMatrixTranspose(tBuffer.matProj);
	tBuffer.matWV = XMMatrixTranspose(tBuffer.matWV);
	tBuffer.matWVP = XMMatrixTranspose(tBuffer.matWVP);

	//상수버퍼를 업데이트한다.
	m_pShader->UpdateConstantBuffer("Transform", &tBuffer, ST_VERTEX);
	m_pShader->UpdateConstantBuffer("Transform", &tBuffer, ST_PIXEL);

}

void CDx11Renderer::SetLightCBuffer(CDx11GameObject *Obj)
{
	list<CDx11GameObject*>* pLightList = m_pScene->GetLightList();
	list<CDx11GameObject*>::iterator	iter;
	list<CDx11GameObject*>::iterator	iterEnd = pLightList->end();

	for (iter = pLightList->begin(); iter != iterEnd; ++iter)
	{
		//string name = (*iter)->GetTag();
		CDx11Light* pLight = (CDx11Light*)(*iter)->FindComponentFromType(CT_LIGHT);

		pLight->SetObj(Obj);
		pLight->SetLight(m_pShader);

		SAFE_RELEASE(pLight);
	}
}

void CDx11Renderer::SetMaterialCBuffer()
{
	//재질이 있을 경우 Set해준다.
	//if (m_pMaterial)
	//	m_pMaterial->SetMaterial(m_pShader);
}

void CDx11Renderer::SetAnimationCBuffer()
{
	CDx11Animation2D*	pAnimation2D = (CDx11Animation2D*)m_pGameObject->FindComponentFromType(CT_ANIMATION2D);
	
	if (pAnimation2D)
	{
		UVFRAME		tFrame = {};

		tFrame.iFrameX = pAnimation2D->GetFrameX();
		tFrame.iFrameY = pAnimation2D->GetFrameY();
		tFrame.iMaxFrameX = pAnimation2D->GetMaxFrameX();
		tFrame.iMaxFrameY = pAnimation2D->GetMaxFrameY();
		tFrame.iAnimType = pAnimation2D->GetAnimationType();

		m_pShader->UpdateConstantBuffer("UVFrame", &tFrame, ST_VERTEX);
	}

	SAFE_RELEASE(pAnimation2D);
}
