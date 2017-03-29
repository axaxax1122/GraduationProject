#include "Dx11ColliderHexahedron.h"
#include "Dx11Transform.h"
#if defined(_DEBUG) | defined(DEBUG)
#include "../Resources/Dx11Mesh.h"
#include "../Resources/Dx11ResourcesManager.h"
#include "../Rendering/Dx11Shader.h"
#include "../Scene/Dx11Scene.h"
#include "Dx11Camera.h"
#include "../GameObject/Dx11GameObject.h"
#endif


DX11_USING

Dx11ColliderHexahedron::Dx11ColliderHexahedron()
{
	m_eSubType = CST_HEXAHEDRON;
	SetTypeName<Dx11ColliderHexahedron>();
}
Dx11ColliderHexahedron::Dx11ColliderHexahedron(const Dx11ColliderHexahedron & collider) :
	CDx11Collider(collider)
{
	ObbInfo = collider.ObbInfo;	
}

Dx11ColliderHexahedron::~Dx11ColliderHexahedron()
{
#if defined(_DEBUG) | defined(DEBUG)	
	SAFE_RELEASE(m_pMesh);
#endif
}

void Dx11ColliderHexahedron::SetInfo(float fLenth)
{
	ObbInfo.fAxisLen[0] = fLenth;
	ObbInfo.fAxisLen[1] = fLenth;
	ObbInfo.fAxisLen[2] = fLenth;
}

void Dx11ColliderHexahedron::SetInfo(const Vec3& vCenter, float fLenth)
{
	ObbInfo.vCenterPos = vCenter;
	ObbInfo.fAxisLen[0] = fLenth;
	ObbInfo.fAxisLen[1] = fLenth;
	ObbInfo.fAxisLen[2] = fLenth;
}

void Dx11ColliderHexahedron::SetInfo(const Vec3 & vCenter, Vec3 & fRot, Vec3 & fScale)
{
	ObbInfo.vCenterPos = vCenter;

	ObbInfo.vRot.x = fRot.x;
	ObbInfo.vRot.y = fRot.y;
	ObbInfo.vRot.z = fRot.z;

	ObbInfo.fAxisLen[0] = fScale.x;
	ObbInfo.fAxisLen[1] = fScale.y;
	ObbInfo.fAxisLen[2] = fScale.z;
}

void Dx11ColliderHexahedron::MoveWorld(const Vec3 & vMove)
{

	ObbInfo.vCenterPos += vMove;
	//m_vWorldPos += vMove;
	//m_vWorldFinalPos = m_vLocalPos + m_vWorldPos;

	////부모 존재하면 영향 ㄱ
	//ComputeRotationPos();

	//m_matWorldTranslate = XMMatrixTranslation(m_vWorldFinalPos.x, m_vWorldFinalPos.y,
	//	m_vWorldFinalPos.z);

	////자식있으면 돌리게!
	//list<CDx11GameObject*>*	pChildList = m_pGameObject->GetChildList();
	//list<CDx11GameObject*>::iterator	iter;
	//list<CDx11GameObject*>::iterator	iterEnd = pChildList->end();

	//for (iter = pChildList->begin(); iter != iterEnd; ++iter)
	//{
	//	CDx11Transform*	pTr = (*iter)->GetTransform();

	//	//초기화해서 재귀
	//	pTr->MoveWorld(Vec3Zero);

	//	SAFE_RELEASE(pTr);
	//}

	//EnableUpdate();
}

bool Dx11ColliderHexahedron::Init()
{
	CDx11Collider::Init();

#if defined(_DEBUG) | defined(DEBUG)
	//3. 메쉬랑 셰이더 결정
	m_pMesh = DX11_GET_SINGLE(CDx11ResourcesManager)->FindMesh(HEXAGON_MESH);
	
#endif
	return true;
}

void Dx11ColliderHexahedron::Input(float fTime)
{
}

int Dx11ColliderHexahedron::Update(float fTime)
{
	return 0;
}

int Dx11ColliderHexahedron::LateUpdate(float fTime)
{
	Vec3	vPos = m_pTransform->GetWorldPos();
	Vec3	vDist = vPos - m_vPrevPos;
	ObbInfo.vCenterPos += vDist;
	m_vPrevPos = vPos;

	ObbInfo.vAxisDir[0] = m_pTransform->GetWorldAxis()[0];
	ObbInfo.vAxisDir[1] = m_pTransform->GetWorldAxis()[1];
	ObbInfo.vAxisDir[2] = m_pTransform->GetWorldAxis()[2];
	

#if defined(_DEBUG) | defined(DEBUG)
	Matrix matScale,matRotX, matRotY, matRotZ, matRotXYZ, matTrans;
	matScale = XMMatrixScaling(ObbInfo.fAxisLen[0] , ObbInfo.fAxisLen[1] ,
		ObbInfo.fAxisLen[2]);

	matRotX = XMMatrixRotationX(ObbInfo.vRot.x* DX11_PI / 180.f);
	matRotY = XMMatrixRotationY(ObbInfo.vRot.y* DX11_PI / 180.f);
	matRotZ = XMMatrixRotationZ(ObbInfo.vRot.z* DX11_PI / 180.f);
	matRotXYZ = matRotX * matRotY * matRotZ;

	matTrans = XMMatrixTranslation(ObbInfo.vCenterPos.x, ObbInfo.vCenterPos.y, ObbInfo.vCenterPos.z);

	m_matWorld = matScale *matRotXYZ* matTrans;
#endif
	return 0;
}

void Dx11ColliderHexahedron::Render(float fTime)
{
#if defined(_DEBUG) | defined(DEBUG)
	CDx11GameObject *pObj = m_pScene->GetMainCamera();

	CDx11Camera	    *pCamera = (CDx11Camera*)pObj->FindComponentFromType(CT_CAMERA);

	TRANSFORMCBUFFER tBuffer = {};
	tBuffer.matView = pCamera->GetViewMatrix();
	tBuffer.matProj = pCamera->GetProjMatrix();

	SAFE_RELEASE(pCamera);
	SAFE_RELEASE(pObj);

	tBuffer.matWorld = m_matWorld;

	tBuffer.matWV = tBuffer.matWorld * tBuffer.matView;
	tBuffer.matWVP = tBuffer.matWV * tBuffer.matProj;

	tBuffer.matWorld = XMMatrixTranspose(tBuffer.matWorld);
	tBuffer.matView = XMMatrixTranspose(tBuffer.matView);
	tBuffer.matProj = XMMatrixTranspose(tBuffer.matProj);

	tBuffer.matWV = XMMatrixTranspose(tBuffer.matWV);
	tBuffer.matWVP = XMMatrixTranspose(tBuffer.matWVP);
	//5. 상수버퍼 업뎃
	m_pShader->UpdateConstantBuffer("Transform", &tBuffer, ST_VERTEX);

	
	

#endif
	//렌더러 역할(렌더러 컴포넌트 부착할수없기에.. ) + 4. 리소스랑 메쉬 설정

	CDx11Collider::Render(fTime);

}

Dx11ColliderHexahedron * Dx11ColliderHexahedron::Clone()
{
	return new Dx11ColliderHexahedron(*this);
}

bool Dx11ColliderHexahedron::Collision(CDx11Collider * pColl)
{
	switch (pColl->GetColliderType())
	{	
	case CST_HEXAHEDRON:
		return CollisionOBB(ObbInfo, 
			((Dx11ColliderHexahedron*)pColl)->ObbInfo);
	}

	return false;
}
