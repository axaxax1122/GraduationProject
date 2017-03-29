#include "Dx11Collider.h"
#include "../Core/Dx11Device.h"
#include "../Dx11Types.h"
#if defined(_DEBUG) | defined(DEBUG)
#include "../Resources/Dx11Mesh.h"
#include "../Rendering/Dx11Shader.h"
#include "../Rendering/Dx11ShaderManager.h"
#include "../Rendering/Dx11RenderState.h"
#include "../Rendering/Dx11RenderManager.h"
#endif

DX11_USING

CDx11Collider::CDx11Collider()
{
	m_eComType = CT_COLLIDER;
#if defined(_DEBUG) | defined(DEBUG)
	m_pMesh = NULL;
	m_pShader = NULL;
	m_pWireFrame = NULL;
#endif
}

CDx11Collider::CDx11Collider(const CDx11Collider & collider) :
	CDx11Component(collider)
{
	m_eSubType = collider.m_eSubType;
	m_vPrevPos = collider.m_vPrevPos;
	m_CollList.clear();
#if defined(_DEBUG) | defined(DEBUG)
	m_pMesh = collider.m_pMesh;
	m_pMesh->AddRef();

	m_pShader = collider.m_pShader;
	m_pShader->AddRef();

	m_pWireFrame = collider.m_pWireFrame;
	m_pWireFrame->AddRef();

	m_vColor = collider.m_vColor;
#endif
}

CDx11Collider::~CDx11Collider()
{
#if defined(_DEBUG) | defined(DEBUG)
	SAFE_RELEASE(m_pMesh);
	SAFE_RELEASE(m_pShader);
	SAFE_RELEASE(m_pWireFrame);
#endif

	list<CDx11Collider*>::iterator	iter;
	list<CDx11Collider*>::iterator	iterEnd = m_CollList.end();

	for (iter = m_CollList.begin(); iter != iterEnd; ++iter)
	{
		(*iter)->EraseCollider(this);
	}

	m_CollList.clear();
}

COLLIDER_SUB_TYPE CDx11Collider::GetColliderType()
{
	return m_eSubType;
}

void CDx11Collider::AddCollider(CDx11Collider * pColl)
{
	m_CollList.push_back(pColl);
}

void CDx11Collider::EraseCollider(CDx11Collider * pColl)
{
	list<CDx11Collider*>::iterator	iter;
	list<CDx11Collider*>::iterator	iterEnd = m_CollList.end();

	for (iter = m_CollList.begin(); iter != iterEnd; ++iter)
	{
		if (*iter == pColl)
		{
			m_CollList.erase(iter);
			return;
		}
	}
}

bool CDx11Collider::CheckCollider(CDx11Collider * pColl)
{
	list<CDx11Collider*>::iterator	iter;
	list<CDx11Collider*>::iterator	iterEnd = m_CollList.end();

	for (iter = m_CollList.begin(); iter != iterEnd; ++iter)
	{
		if (*iter == pColl)
			return true;
	}

	return false;
}

void CDx11Collider::SetColor(const Vec4 & vColor)
{
#if defined(_DEBUG) | defined(DEBUG)
	m_vColor = vColor;
#endif
}

bool CDx11Collider::Init()
{
#if defined(_DEBUG) | defined(DEBUG)
	//3. 메쉬랑 셰이더 결정
	m_matWorld = XMMatrixIdentity();
	m_pShader = DX11_GET_SINGLE(CDx11ShaderManager)->FindShader(COLLIDER_SHADER);
	m_vColor = White;
	m_vCollisionColor = Red;

	m_pWireFrame = DX11_GET_SINGLE(CDx11RenderManager)->FindRenderState(DX11_RS_WIREFRAME);
#endif

	return true;
}

void CDx11Collider::Input(float fTime)
{
}

int CDx11Collider::Update(float fTime)
{
	return 0;
}

int CDx11Collider::LateUpdate(float fTime)
{
	return 0;
}

void CDx11Collider::Render(float fTime)
{
#if defined(_DEBUG) | defined(DEBUG)
	// 이거 순서 때문에 안되는데 이유 알아보기!!!!

	//5.상수버퍼 업뎃.
	m_pShader->UpdateConstantBuffer("Color", &m_vColor, ST_PIXEL);

	//4. 리소스랑 메쉬 설정.
	m_pWireFrame->SetState();
	m_pShader->SetShader();
	m_pMesh->Render();
	m_pWireFrame->ResetState();//하고 리셋

	
#endif
}

bool CDx11Collider::CollisionSphereToSphere(const Vec3 & vC1, float fR1,
	const Vec3 & vC2, float fR2)
{
	float fDist = vC1.Distance(vC2);

	return fDist <= fR1 + fR2;
}

bool CDx11Collider::CollisionOBB(const OBBINFO &tInfo1, const OBBINFO &tInfo2)
{
	double c[3][3];
	double absC[3][3];
	double d[3];
	double r0, r1, r;
	int i;
	const double cutoff = 0.999999;
	bool existsParallelPair = false;
	Vec3 diff = tInfo1.vCenterPos - tInfo2.vCenterPos;

	for (i = 0; i < 3; ++i)
	{
		c[0][i] = tInfo1.vAxisDir[0].Dot(tInfo2.vAxisDir[i]);//XMVector3Dot
		absC[0][i] = abs(c[0][i]);
		if (absC[0][i] > cutoff)
			existsParallelPair = true;
	}
	d[0] = diff.Dot(tInfo1.vAxisDir[0]);
	r = abs(d[0]);
	r0 = tInfo1.fAxisLen[0];
	r1 = tInfo2.fAxisLen[0] * absC[0][0] + tInfo2.fAxisLen[1] * absC[0][1] + tInfo2.fAxisLen[2] * absC[0][2];
	if (r > r0 + r1)
		return FALSE;

	for (i = 0; i < 3; ++i)
	{
		c[1][i] = tInfo1.vAxisDir[1].Dot( tInfo2.vAxisDir[i]);
		absC[1][i] = abs(c[1][i]);
		if (absC[1][i] > cutoff)
			existsParallelPair = true;
	}
	d[1] = diff.Dot(tInfo1.vAxisDir[1]);
	r = abs(d[1]);
	r0 = tInfo1.fAxisLen[1];
	r1 = tInfo2.fAxisLen[0] * absC[1][0] + tInfo2.fAxisLen[1] * absC[1][1] + tInfo2.fAxisLen[2] * absC[1][2];
	if (r > r0 + r1)
		return FALSE;

	for (i = 0; i < 3; ++i)
	{
		c[2][i] = tInfo1.vAxisDir[2].Dot(tInfo2.vAxisDir[i]);
		absC[2][i] = abs(c[2][i]);
		if (absC[2][i] > cutoff)
			existsParallelPair = true;
	}
	d[2] = diff.Dot(tInfo1.vAxisDir[2]);
	r = abs(d[2]);
	r0 = tInfo1.fAxisLen[2];
	r1 = tInfo2.fAxisLen[0] * absC[2][0] + tInfo2.fAxisLen[1] * absC[2][1] + tInfo2.fAxisLen[2] * absC[2][2];
	if (r > r0 + r1)
		return FALSE;

	r = abs(diff.Dot(tInfo2.vAxisDir[0]));
	r0 = tInfo1.fAxisLen[0] * absC[0][0] + tInfo1.fAxisLen[1] * absC[1][0] + tInfo1.fAxisLen[2] * absC[2][0];
	r1 = tInfo2.fAxisLen[0];
	if (r > r0 + r1)
		return FALSE;

	r = abs(diff.Dot(tInfo2.vAxisDir[1]));
	r0 = tInfo1.fAxisLen[0] * absC[0][1] + tInfo1.fAxisLen[1] * absC[1][1] + tInfo1.fAxisLen[2] * absC[2][1];
	r1 = tInfo2.fAxisLen[1];
	if (r > r0 + r1)
		return FALSE;

	r = abs(diff.Dot(tInfo2.vAxisDir[2]));
	r0 = tInfo1.fAxisLen[0] * absC[0][2] + tInfo1.fAxisLen[1] * absC[1][2] + tInfo1.fAxisLen[2] * absC[2][2];
	r1 = tInfo2.fAxisLen[2];
	if (r > r0 + r1)
		return FALSE;

	if (existsParallelPair == true)
		return TRUE;

	r = abs(d[2] * c[1][0] - d[1] * c[2][0]);
	r0 = tInfo1.fAxisLen[1] * absC[2][0] + tInfo1.fAxisLen[2] * absC[1][0];
	r1 = tInfo2.fAxisLen[1] * absC[0][2] + tInfo2.fAxisLen[2] * absC[0][1];
	if (r > r0 + r1)
		return FALSE;

	r = abs(d[2] * c[1][1] - d[1] * c[2][1]);
	r0 = tInfo1.fAxisLen[1] * absC[2][1] + tInfo1.fAxisLen[2] * absC[1][1];
	r1 = tInfo2.fAxisLen[0] * absC[0][2] + tInfo2.fAxisLen[2] * absC[0][0];
	if (r > r0 + r1)
		return FALSE;

	r = abs(d[2] * c[1][2] - d[1] * c[2][2]);
	r0 = tInfo1.fAxisLen[1] * absC[2][2] + tInfo1.fAxisLen[2] * absC[1][2];
	r1 = tInfo2.fAxisLen[0] * absC[0][1] + tInfo2.fAxisLen[1] * absC[0][0];
	if (r > r0 + r1)
		return FALSE;

	r = abs(d[0] * c[2][0] - d[2] * c[0][0]);
	r0 = tInfo1.fAxisLen[0] * absC[2][0] + tInfo1.fAxisLen[2] * absC[0][0];
	r1 = tInfo2.fAxisLen[1] * absC[1][2] + tInfo2.fAxisLen[2] * absC[1][1];
	if (r > r0 + r1)
		return FALSE;

	r = abs(d[0] * c[2][1] - d[2] * c[0][1]);
	r0 = tInfo1.fAxisLen[0] * absC[2][1] + tInfo1.fAxisLen[2] * absC[0][1];
	r1 = tInfo2.fAxisLen[0] * absC[1][2] + tInfo2.fAxisLen[2] * absC[1][0];
	if (r > r0 + r1)
		return FALSE;

	r = abs(d[0] * c[2][2] - d[2] * c[0][2]);
	r0 = tInfo1.fAxisLen[0] * absC[2][2] + tInfo1.fAxisLen[2] * absC[0][2];
	r1 = tInfo2.fAxisLen[0] * absC[1][1] + tInfo2.fAxisLen[1] * absC[1][0];
	if (r > r0 + r1)
		return FALSE;

	r = abs(d[1] * c[0][0] - d[0] * c[1][0]);
	r0 = tInfo1.fAxisLen[0] * absC[1][0] + tInfo1.fAxisLen[1] * absC[0][0];
	r1 = tInfo2.fAxisLen[1] * absC[2][2] + tInfo2.fAxisLen[2] * absC[2][1];
	if (r > r0 + r1)
		return FALSE;

	r = abs(d[1] * c[0][1] - d[0] * c[1][1]);
	r0 = tInfo1.fAxisLen[0] * absC[1][1] + tInfo1.fAxisLen[1] * absC[0][1];
	r1 = tInfo2.fAxisLen[0] * absC[2][2] + tInfo2.fAxisLen[2] * absC[2][0];
	if (r > r0 + r1)
		return FALSE;

	r = abs(d[1] * c[0][2] - d[0] * c[1][2]);
	r0 = tInfo1.fAxisLen[0] * absC[1][2] + tInfo1.fAxisLen[1] * absC[0][2];
	r1 = tInfo2.fAxisLen[0] * absC[2][1] + tInfo2.fAxisLen[1] * absC[2][0];
	if (r > r0 + r1)
		return FALSE;

	return TRUE;
}



bool CDx11Collider::CollisionRectToRect(const RECTINFO & tInfo1,
	const RECTINFO & tInfo2)
{
	if (tInfo1.l > tInfo2.r)
		return false;

	else if (tInfo1.r < tInfo2.l)
		return false;

	else if (tInfo1.t > tInfo2.b)
		return false;

	else if (tInfo1.b < tInfo2.t)
		return false;

	return true;
}

bool CDx11Collider::CollisionRectToPoint(const RECTINFO & tRC, const Vec2 & vPos)
{
	if (tRC.l > vPos.x)
		return false;

	else if (tRC.r < vPos.x)
		return false;

	else if (tRC.t > vPos.y)
		return false;

	else if (tRC.b < vPos.y)
		return false;

	return true;
}
