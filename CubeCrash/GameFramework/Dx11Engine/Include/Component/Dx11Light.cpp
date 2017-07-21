#include "Dx11Light.h"
#include "../Rendering/Dx11Shader.h"
#include "../Scene/Dx11Scene.h"
#include "../GameObject/Dx11GameObject.h"
#include "../Component/Dx11Transform.h"

DX11_USING

CDx11Light::CDx11Light()	:
	m_tInfo({}),
	m_tFogInfo({}),
	m_Obj(NULL),
	m_pPlayer(NULL)
{
	m_eComType = CT_LIGHT;
	SetTypeName<CDx11Light>();
}


CDx11Light::CDx11Light(const CDx11Light & light)	:
	CDx11Component(light)
{
	m_tInfo = light.m_tInfo;
	m_tFogInfo = light.m_tFogInfo;
}

CDx11Light::~CDx11Light()
{
	SAFE_RELEASE(m_pPlayer);
	SAFE_RELEASE(m_Obj);	
}

void CDx11Light::SetDiffuse(const Vec4 & vDif)
{
	m_tInfo.vDif = vDif;
}

void CDx11Light::SetAmbient(const Vec4 & vAmb)
{
	m_tInfo.vAmb = vAmb;
}

void CDx11Light::SetSpecular(const Vec4 & vSpc)
{
	m_tInfo.vSpc = vSpc;
}

void CDx11Light::SetFogPos(const Vec3 & vPos)
{
	//m_tFogInfo.vEyePos = vPos;
}

void CDx11Light::SetFogColor(const Vec4 & vColor)
{
	m_tFogInfo.vColor = vColor;
}

bool CDx11Light::Init()
{
	m_tInfo.vDif = White;
	m_tInfo.vAmb = White;
	m_tInfo.vSpc = White;

	m_tFogInfo.vEyePos = 0;
	m_tFogInfo.fStart = Vec3(0.f, 0.f, 0.f);
	m_tFogInfo.fRange = 300.f;
	m_tFogInfo.vColor = Vec4(0.5f, 0.5f, 0.5f, 1.f);

	return true;
}

void CDx11Light::Input(float fTime)
{
}

int CDx11Light::Update(float fTime)
{
	return 0;
}

int CDx11Light::LateUpdate(float fTime)
{
	return 0;
}

void CDx11Light::Render(float fTime)
{
}

void CDx11Light::SetLight(CDx11Shader * pShader)
{
	CDx11Transform*		pObjTr = m_Obj->GetTransform();
	if (m_pPlayer == nullptr)
		m_pPlayer = m_pScene->FindGameObjectFromTag("PlayerObject");
	CDx11Transform*		pPlaTr = m_pPlayer->GetTransform();

	m_tFogInfo.vEyePos = ((pObjTr->GetWorldPos()) - pPlaTr->GetWorldPos()).Length();

	SAFE_RELEASE(pPlaTr);
	SAFE_RELEASE(pObjTr);

	pShader->UpdateConstantBuffer("LightInfo", &m_tInfo, ST_PIXEL);
	pShader->UpdateConstantBuffer("FogInfo", &m_tFogInfo, ST_PIXEL);
}

void CDx11Light::SetObj(CDx11GameObject *Obj)
{
	m_Obj = Obj;
}

