#include "QuadTree.h"
#include "../Core/Dx11Debug.h"
#include "../Component/Dx11Camera.h"
#include "Dx11SceneManager.h"
#include "Dx11Scene.h"
#include "Dx11Layer.h"
#include "../GameObject/Dx11GameObject.h"
#include "../Component/Dx11ColliderOBB.h"
#include "../Component/Dx11Transform.h"
#include "../Component/Dx11ColliderAABB.h"
#include "../Core/Dx11CollisionManager.h"
#include "../Component/Dx11Renderer.h"
#include "../Component/Dx11UIBackground.h"

DX11_USING

QuadTree::QuadTree() :
	m_vLeftTop(Vec3Zero),
	m_vRightTop(Vec3Zero),
	m_vLeftBot(Vec3Zero),
	m_vRightBot(Vec3Zero),
	m_vCenter(Vec3Zero),
	m_fXLen(0),
	m_fZLen(0),
	m_bCulling(true),
	m_pParent(NULL),
	m_pQTRender(NULL)
{
	for (int i = 0; i < 4; ++i)
	{
		m_pChild[i] = NULL;
	}
}

QuadTree::QuadTree(QuadTree * pParent)
{
	// 자식 노드 생성자
	for (int i = 0; i < 4; ++i)
	{
		m_pChild[i] = NULL;
	}

	m_bCulling = true;
	m_fRadius = 0.f;
	m_pParent = pParent;

}

QuadTree::~QuadTree()
{
	for (int i = 0; i < 4; ++i)
	{
		SAFE_DELETE(m_pChild[i]);
	}

	m_StaticObjList.clear();
	m_DynamicObjList.clear();
	SAFE_RELEASE(m_pQTRender);
}

Vec3 QuadTree::GetCenter() const
{
	return m_vCenter;
}

float QuadTree::GetRadius() const
{
	return m_fRadius;
}

QuadTree * QuadTree::GetChild(int i) const
{
	return m_pChild[i];
}

bool QuadTree::GetisCulling() const
{
	return m_bCulling;
}

CDx11GameObject * QuadTree::GetQTRender() const
{
	m_pQTRender->AddRef();
	return m_pQTRender;
}


void QuadTree::SetNodeInfo(const Vec3 & vLeftTop, const Vec3& vRightTop,
	const Vec3& vLeftBot, const Vec3 & vRightBot)
{
	m_vLeftTop = vLeftTop;
	m_vRightTop = vRightTop;
	m_vLeftBot = vLeftBot;
	m_vRightBot = vRightBot;

	m_vCenter.x = (m_vLeftTop.x + m_vRightBot.x) / 2;
	m_vCenter.y = 0.f;
	m_vCenter.z = (m_vLeftTop.z + m_vRightBot.z) / 2;

	m_bCulling = false;
	m_fRadius = (m_vRightBot.x - m_vLeftTop.x) / 2;

	MakeQTRender();
}

void QuadTree::SetParent(const Vec3 & vLeftTop, const Vec3& vRightTop, 
	const Vec3& vLeftBot, const Vec3 & vRightBot)
{
	// 루트를 정의하고 쿼드트리를 재귀적으로 구축
	SetNodeInfo(vLeftTop, vRightTop, vLeftBot, vRightBot);
	BuildQuadTree();
}

QuadTree * QuadTree::AddChild(const Vec3 & vLeftTop, const Vec3& vRightTop, const Vec3& vLeftBot, 
	const Vec3 & vRightBot)
{
	QuadTree*	pChild = NULL;

	pChild = new QuadTree(this);
	pChild->SetNodeInfo(vLeftTop, vRightTop, vLeftBot, vRightBot);

	return pChild;
}

bool QuadTree::SubDivide()
{
	Vec3	TopCenter;
	Vec3	BotCenter;
	Vec3	LeftCenter;
	Vec3	RightCenter;

	TopCenter.x = m_vCenter.x;
	TopCenter.z = m_vLeftTop.z;

	BotCenter.x = m_vCenter.x;
	BotCenter.z = m_vRightBot.z;

	LeftCenter.x = m_vLeftTop.x;
	LeftCenter.z = m_vCenter.z;

	RightCenter.x = m_vRightBot.x;
	RightCenter.z = m_vCenter.z;


	if (m_fRadius <= 16.f)
		return false;

	
	m_pChild[LEFT_TOP] = AddChild(m_vLeftTop, TopCenter, LeftCenter, m_vCenter);
	m_pChild[RIGHT_TOP] = AddChild(TopCenter, m_vRightTop, m_vCenter, RightCenter);
	m_pChild[LEFT_BOT] = AddChild(LeftCenter, m_vCenter, m_vLeftBot, BotCenter);
	m_pChild[RIGHT_BOT] = AddChild(m_vCenter, RightCenter, BotCenter, m_vRightBot);

	return true;
}

bool QuadTree::BuildQuadTree()
{
	if (SubDivide())
	{
		m_pChild[LEFT_TOP]->BuildQuadTree();
		m_pChild[RIGHT_TOP]->BuildQuadTree();
		m_pChild[LEFT_BOT]->BuildQuadTree();
		m_pChild[RIGHT_BOT]->BuildQuadTree();

		//char pStr[50] = {};
		//sprintf_s(pStr, "CenterPos // x : %lf, z : %lf\n", m_vCenter.x, m_vCenter.z);
		//DX11_GET_SINGLE(CDx11Debug)->OutputConsole(pStr);
	}
	return true;
}

int QuadTree::IsInFrustrum()
{
	Frustrum fs = GetFrustrum();

	// 쿼드트리의 경계 구 안에 절두체가 존재하는 지 확인
	if (!fs.IsInSphereXZ(m_vCenter, m_fRadius * 2))
		return FRUSTRUM_OUT;

	// 쿼드트리의 각 좌표가 절두체에 포함되는지 확인
	bool	b[4];
	b[0] = fs.IsIn(m_vLeftTop);
	b[1] = fs.IsIn(m_vRightTop);
	b[2] = fs.IsIn(m_vLeftBot);
	b[3] = fs.IsIn(m_vRightBot);

	// 전부 포함되는 경우
	if (b[0] + b[1] + b[2] + b[3] == 4)
		return FRUSTRUM_COMPLETELY_IN;

	// 일부만 포함되는 경우
	return FRUSTRUM_PARTIALLY_IN;
}

Frustrum QuadTree::GetFrustrum() const
{
	CDx11Scene*	pScene = DX11_GET_SINGLE(CDx11SceneManager)->GetCurrentScene();
	CDx11GameObject* pCamObj = pScene->GetMainCamera();
	CDx11Camera*	pCamera = (CDx11Camera*)pCamObj->FindComponentFromType(CT_CAMERA);

	Frustrum fs = pCamera->GetFrustrum();

	SAFE_RELEASE(pCamera);
	SAFE_RELEASE(pCamObj);
	SAFE_RELEASE(pScene);

	return fs;
}


void QuadTree::FrustrumCulling()
{
	int ret = IsInFrustrum();

	switch (ret)
	{
		case FRUSTRUM_OUT:
			SetCulling(this, true);
			return;
		case FRUSTRUM_PARTIALLY_IN:
			// 일부만 포함되있으므로 하위노드 검색이 필요.
			m_bCulling = false;
			SetRenderFlag(this);
			break;
		case FRUSTRUM_COMPLETELY_IN:
			SetCulling(this, false);
			return;
	}

	// 하위노드 검사 수행
	for (int i = 0; i < 4; ++i)
	{
		if (m_pChild[i])
			m_pChild[i]->FrustrumCulling();
	}
}

void QuadTree::PrintNode()
{
	char pStr[80] = {};
	sprintf_s(pStr, "Center x : %lf, Center z : %lf, Radius : %lf\n", m_vCenter.x, m_vCenter.z, m_fRadius);
	DX11_GET_SINGLE(CDx11Debug)->OutputConsole(pStr);

	for (int i = 0; i < 4; ++i)
	{
		if(m_pChild[i]) m_pChild[i]->PrintNode();
	}
}

bool QuadTree::IsInSquare(const Vec3& vMin, const Vec3& vMax)
{
	// 오브젝트가 현재 노드에 완전히 포함되어있는지 확인한다.

	if (vMin.x < m_vLeftBot.x || vMin.x > m_vRightTop.x)
		return false;
	if (vMin.z < m_vLeftBot.z || vMin.z > m_vRightTop.z)
		return false;
	if (vMax.x < m_vLeftBot.x || vMax.x > m_vRightTop.x)
		return false;
	if (vMax.z < m_vLeftBot.z || vMax.z > m_vRightTop.z)
		return false;

	return true;
}

void QuadTree::AddStaticObj(CDx11GameObject * pObj)
{
	CDx11ColliderOBB*	pOBB = (CDx11ColliderOBB*)pObj->FindComponentFromTypeName("CDx11ColliderOBB");

	Vec3	vCenter;
	float	fXLen;
	float	fZLen;

	if (!pOBB)
	{
		CDx11Transform* pTr = pObj->GetTransform();
		vCenter = pTr->GetWorldPos();
		fXLen = 1.f;
		fZLen = 1.f;
		SAFE_RELEASE(pTr);
	}
	else
	{
		vCenter = pOBB->GetCenter();
		fXLen = pOBB->GetAxisLen(AXIS_X);
		fZLen = pOBB->GetAxisLen(AXIS_Z);
	}
	SAFE_RELEASE(pOBB);


	Vec3	vMin = Vec3Zero;
	Vec3	vMax = Vec3Zero;

	vMin.x = vCenter.x - fXLen;
	vMin.z = vCenter.z - fZLen;

	vMax.x = vCenter.x + fXLen;
	vMax.z = vCenter.z + fZLen;

	// 루트 노드에는 당연히 포함될 것이므로 바로 CheckNode함수를 호출
	_StaticCheckNode(this, pObj, vMin, vMax);
}

void QuadTree::AddDynamicObj(CDx11GameObject * pObj)
{
	CDx11ColliderOBB*	pOBB = (CDx11ColliderOBB*)pObj->FindComponentFromTypeName("CDx11ColliderOBB");
	Vec3	vCenter;
	float	fXLen;
	float	fZLen;

	if (!pOBB)
	{
		CDx11Transform* pTr = pObj->GetTransform();
		vCenter = pTr->GetWorldPos();
		fXLen = 1.f;
		fZLen = 1.f;
		SAFE_RELEASE(pTr);
	}
	else
	{
		vCenter = pOBB->GetCenter();
		fXLen = pOBB->GetAxisLen(AXIS_X);
		fZLen = pOBB->GetAxisLen(AXIS_Z);
	}
	SAFE_RELEASE(pOBB);


	Vec3	vMin = Vec3Zero;
	Vec3	vMax = Vec3Zero;

	vMin.x = vCenter.x - fXLen;
	vMin.z = vCenter.z - fZLen;

	vMax.x = vCenter.x + fXLen;
	vMax.z = vCenter.z + fZLen;

	// 루트 노드에는 당연히 포함될 것이므로 바로 CheckNode함수를 호출
	_DynamicCheckNode(this, pObj, vMin, vMax);
}

// 정적 오브젝트
// 종료 전까지 벡터를 클리어 하지 않음
void QuadTree::_StaticCheckNode(QuadTree* pNode, CDx11GameObject* pObj, const Vec3 & vMin, const Vec3 & vMax)
{
	for (int i = 0; i < 4; ++i)
	{
		if (pNode->m_pChild[i])
		{
			if (pNode->m_pChild[i]->IsInSquare(vMin, vMax))
			{
				// 하나라도 true일 경우
				// 해당 차일드 노드의 차일드를 검사
				_StaticCheckNode(pNode->m_pChild[i], pObj, vMin, vMax);
				return;
			}
		}
		else
			break;
	}

	// 모두 false 일 경우
	// 부모 노드에 푸쉬백
	pNode->m_StaticObjList.push_back(pObj);
}

// 동적 오브젝트
// 매 프레임마다 벡터를 클리어
void QuadTree::_DynamicCheckNode(QuadTree * pNode, CDx11GameObject * pObj, const Vec3 & vMin, const Vec3 & vMax)
{
	for (int i = 0; i < 4; ++i)
	{
		if (pNode->m_pChild[i])
		{
			if (pNode->m_pChild[i]->IsInSquare(vMin, vMax))
			{
				// 하나라도 true일 경우
				// 해당 차일드 노드의 차일드를 검사
				_DynamicCheckNode(pNode->m_pChild[i], pObj, vMin, vMax);
				return;
			}
		}
		else
			break;
	}

	// 모두 false 일 경우
	// 부모 노드에 푸쉬백
	pNode->m_DynamicObjList.push_back(pObj);
}

void QuadTree::Collision(QuadTree * pNode, float fTime)
{
	DX11_GET_SINGLE(CDx11CollisionManager)->Collision(fTime);
	
	// 자식이 있으면
	if (pNode->m_pChild[0])
	{
		for (int i = 0; i < 4; ++i)
		{
			Collision(pNode->m_pChild[i], fTime);
		}
	}
}

void QuadTree::MakeTreeLoose(QuadTree* pNode)
{
	float fXLen = pNode->m_vCenter.x - pNode->m_vLeftTop.x;
	float fZLen = pNode->m_vCenter.z - pNode->m_vLeftBot.z;

	pNode->m_vLeftTop.x -= fXLen;
	pNode->m_vLeftTop.z += fZLen;

	pNode->m_vRightTop.x += fXLen;
	pNode->m_vRightTop.z += fZLen;

	pNode->m_vLeftBot.x -= fXLen;
	pNode->m_vLeftBot.z -= fZLen;

	pNode->m_vRightBot.x += fXLen;
	pNode->m_vRightBot.z -= fZLen;

	pNode->m_fRadius *= 2;

	if (pNode->m_pChild[0])
	{
		for (int i = 0; i < 4; ++i)
		{
			MakeTreeLoose(pNode->m_pChild[i]);
		}
	}
}

void QuadTree::MakeQTRender()
{
	m_pQTRender = CDx11GameObject::Create("QT");
	CDx11Transform*		pTr = m_pQTRender->GetTransform();
	pTr->SetWorldPos((m_vCenter.x + 400) / 4, (m_vCenter.z + 400) / 4, 0.f);
	pTr->SetWorldScale(m_fRadius, m_fRadius, 1.f);
	SAFE_RELEASE(pTr);

	CDx11Renderer*	pRenderer = m_pQTRender->AddComponent<CDx11Renderer>("QTRenderer");
	pRenderer->SetMesh(RECT_UI_MESH);
	pRenderer->SetShader(COLLIDER_SHADER);
	pRenderer->SetRenderState(DX11_RS_DEPTH_DISABLE);
	pRenderer->SetRenderState(DX11_RS_WIREFRAME);
	SAFE_RELEASE(pRenderer);

	CDx11UIBackground*	pQTUI = m_pQTRender->AddComponent<CDx11UIBackground>("QTUI");
	SAFE_RELEASE(pQTUI);
}

void QuadTree::SetRenderFlag(QuadTree* pNode)
{
	CDx11Renderer*	pRenderer = (CDx11Renderer*)pNode->m_pQTRender->FindComponentFromType(CT_RENDERER);
	if (pNode->m_bCulling)
	{
		pRenderer->SetRenderState(DX11_RS_WIREFRAME);
	}
	else
	{
		pRenderer->SetRenderState(DX11_RS_SOLID);
	}
	SAFE_RELEASE(pRenderer);
}

void QuadTree::SetCulling(QuadTree* pNode, bool bCulling)
{
	pNode->m_bCulling = bCulling;
	SetRenderFlag(pNode);

	if (pNode->m_pChild[0])
	{
		for (int i = 0; i < 4; ++i)
		{
			SetCulling(pNode->m_pChild[i], bCulling);
		}
	}
}




