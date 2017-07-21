#include "Dx11Animation3D.h"
#include "Dx11AnimationClip.h"
#include "../Rendering/Dx11Shader.h"
#include "Dx11Renderer.h"
#include "../GameObject/Dx11GameObject.h"
#include "../Resources/Dx11Texture.h"
#include "../Resources/Dx11ResourcesManager.h"
#include "Dx11FSM.h"
#include "Dx11State.h"

DX11_USING

CDx11Animation3D::CDx11Animation3D()
{
	m_eComType = CT_ANIMATION3D;
	SetTypeName<CDx11Animation3D>();
	m_iCurrentClip = 0;
	m_pBoneTexture = NULL;
	memset(&m_tBoneCBuffer, 0, sizeof(BONECBUFFER));
}


CDx11Animation3D::CDx11Animation3D(const CDx11Animation3D & anim)	:
	CDx11Component(anim)
{
	*this = anim;

	m_vecAnimationClip.clear();
	m_vecAnimationClip.reserve(anim.m_vecAnimationClip.size());
	
	for (size_t i = 0; i < anim.m_vecAnimationClip.size(); ++i)
	{
		CDx11AnimationClip* pClip = anim.m_vecAnimationClip[i]->Clone();
		m_vecAnimationClip.push_back(pClip);
	}
	
	if (m_pBoneTexture)
		m_pBoneTexture->AddRef();
}

CDx11Animation3D::~CDx11Animation3D()
{
	Safe_Release_VecList(m_vecAnimationClip);
}

bool CDx11Animation3D::Init()
{
	m_fGlobalTime = 9.f;
	m_fStartTime = 0.f;
	m_fEndTime = 34.167f;
	m_eState = STATE_IDLE;
	  
	return true;
}

int CDx11Animation3D::Update(float fTime)
{
	UpdateAnimationState(fTime);

	m_fGlobalTime += fTime;

	if (m_fGlobalTime < m_fStartTime || m_fGlobalTime > m_fEndTime)
	{
		m_fGlobalTime = m_fStartTime;
		if (m_pGameObject->GetTag() == "PlayerObject")
		{
			m_pGameObject->SetStateFlag(STATE_IDLE);
			m_pGameObject->SetControl(true);
		}
	}

	if (m_fGlobalTime < 0.f || m_fGlobalTime > 34.167f)
	{
		m_fGlobalTime = 9.f;
	}

	//if (m_fGlobalTime >= m_vecAnimationClip[m_iCurrentClip]->m_tClip.dEndTime)
	//	m_fGlobalTime = m_vecAnimationClip[m_iCurrentClip]->m_tClip.dStartTime;

	vector<BONE>::iterator	iterB;
	vector<BONE>::iterator	iterBEnd = m_vecBoneHierarchy.end();
	int		iBoneIndex = 0;

	for (iterB = m_vecBoneHierarchy.begin(); iterB != iterBEnd; ++iterB, ++iBoneIndex)
	{
		(*iterB).matBone = XMMatrixIdentity();
		(*iterB).matTransform = XMMatrixIdentity();
		if ((*iterB).vecKeyFrame.empty())
			continue;

		vector<ANIMATIONKEYFRAME>::iterator	iterA;
		vector<ANIMATIONKEYFRAME>::iterator	iterAEnd = (*iterB).vecKeyFrame.end();
		--iterAEnd;
		for (iterA = (*iterB).vecKeyFrame.begin(); iterA != iterAEnd; ++iterA)
		{
			vector<ANIMATIONKEYFRAME>::iterator	iterANext = iterA + 1;
			if (m_fGlobalTime >= (*iterA).fAnimationTime &&
				m_fGlobalTime <= (*iterANext).fAnimationTime)
			{
				// 진행되고 있는 현재 프레임 시간과 
				// 다음 프레임 시간 사이의 비율을 구한다.
				float	fPercent = (m_fGlobalTime - (*iterA).fAnimationTime) /
					((*iterANext).fAnimationTime - (*iterA).fAnimationTime);

				// 현재 프레임과 다음 프레임의 
				// Scale, Translate, Rotate 정보를 저장한다.
				Vec	vS1 = (*iterA).vScale.Convert();
				Vec	vS2 = (*iterANext).vScale.Convert();

				Vec	vT1 = (*iterA).vTranslate.Convert();
				Vec vT2 = (*iterANext).vTranslate.Convert();

				Vec vR1 = (*iterA).qRot.Convert();
				Vec	vR2 = (*iterANext).qRot.Convert();

				// 이를 이용해 보간한다.
				Vec	vS = XMVectorLerp(vS1, vS2, fPercent);
				Vec vT = XMVectorLerp(vT1, vT2, fPercent);
				Vec	vR = XMQuaternionSlerp(vR1, vR2, fPercent);

				Vec	vZero = XMVectorSet(0.f, 0.f, 0.f, 1.f);
				(*iterB).matTransform = XMMatrixAffineTransformation(vS, vZero, vR, vT);
				break;
			}
		}	
	}
	// 각 본에 부모 행렬을 곱해둔다.
	//vector<Matrix>	vecParent(m_vecBoneHierarchy.size());
	//vecParent[0] = m_vecBoneHierarchy[0].matTransform;

	//for (size_t i = 1; i < m_vecBoneHierarchy.size(); ++i)
	//{
	//	Matrix	matBone = m_vecBoneHierarchy[i].matTransform;

	//	int iParent = m_vecBoneHierarchy[i].iParentIndex;
	//	Matrix matParent = vecParent[iParent];

	//	vecParent[i] = matBone * matParent;
	//}

	iBoneIndex = 0;
	vector<Matrix>	vecBones;
	vecBones.reserve(m_vecBoneHierarchy.size());

	for (iterB = m_vecBoneHierarchy.begin(); iterB != iterBEnd; ++iterB, ++iBoneIndex)
	{
		// 본 행렬을 구해주었으면 본 행렬에 offset 행렬을 곱해준다.
		// 최종 본 행렬을 만들어준다.
		(*iterB).matBone = (*iterB).matOffset * (*iterB).matTransform;
		//(*iterB).matBone = (*iterB).matOffset * vecParent[iBoneIndex];
		if (!m_pBoneTexture)
			m_tBoneCBuffer.matBones[iBoneIndex] = XMMatrixTranspose((*iterB).matBone);

		else
			vecBones.push_back((*iterB).matBone);
		//m_tBoneCBuffer.matBones[iBoneIndex] = (*iterB).matBone;
	}
	
	// 렌더러를 갱신한다.
	CDx11Renderer*	pRenderer = (CDx11Renderer*)m_pGameObject->FindComponentFromType(CT_RENDERER);

	if (pRenderer)
	{
		pRenderer->UpdateCBuffer("BoneArray", &m_tBoneCBuffer, sizeof(BONECBUFFER));
	}

	// 렌더러를 갱신한다.
	if (m_pBoneTexture)
	{
		m_pBoneTexture->SetData(&vecBones[0], sizeof(Matrix) * m_vecBoneHierarchy.size());
		pRenderer->SetBoneTexture(m_pBoneTexture);
	}
	SAFE_RELEASE(pRenderer);
	return 0;
}

int CDx11Animation3D::LateUpdate(float fTime)
{
	return 0;
}

void CDx11Animation3D::Render(float fTime)
{
}

CDx11Animation3D * CDx11Animation3D::Clone()
{
	return new CDx11Animation3D(*this);
}

void CDx11Animation3D::SetMeshName(const string & strName)
{
	m_strName = strName;
}

void CDx11Animation3D::CreateBones(vector<BONE>& vecBone)
{
	m_vecBoneHierarchy = vecBone;
	m_tBoneCBuffer.iBoneCount = vecBone.size();

	if (vecBone.size() > 90)
	{
		string strBoneName = m_strName + "Bone";
		m_pBoneTexture = DX11_GET_SINGLE(CDx11ResourcesManager)->CreateTexture(
			strBoneName, vecBone.size() * 4, 1, 1,
			DXGI_FORMAT_R32G32B32A32_FLOAT, D3D11_USAGE_DYNAMIC,
			D3D11_BIND_SHADER_RESOURCE,
			D3D11_CPU_ACCESS_WRITE);
	}
}

void CDx11Animation3D::AddAnimationClip(const ANIMATIONCLIP & tClip)
{
	CDx11AnimationClip*	pAnimationClip = new CDx11AnimationClip;
	pAnimationClip->m_tClip = tClip;

	m_vecAnimationClip.push_back(pAnimationClip);
}

void CDx11Animation3D::UpdateConstantBuffer(CDx11Shader * pShader)
{
	pShader->UpdateConstantBuffer("BoneArray", &m_tBoneCBuffer, ST_VERTEX);
	if (m_pBoneTexture)
		m_pBoneTexture->SetTexture(3);
}

void CDx11Animation3D::UpdateAnimationState(float fTime)
{
	CDx11FSM*	pFSM = (CDx11FSM*)m_pGameObject->FindComponentFromType(CT_FSM);
	if (!pFSM)
		return;

	CDx11State* pState = pFSM->GetCurrentState();
	
	m_fStartTime = pState->GetStartTime();
	m_fEndTime = pState->GetEndTime();

	SAFE_RELEASE(pState);
	SAFE_RELEASE(pFSM);
}
