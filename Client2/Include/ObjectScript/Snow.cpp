#include "Snow.h"
#include "GameObject/Dx11GameObject.h"
#include "Component/Dx11Transform.h"
#include "Core/Dx11Device.h"
#include "Core/Dx11Input.h"
#include<iostream>
using namespace std;

CSnow::CSnow()
{
}
CSnow::~CSnow()
{
}
CSnow::CSnow(const CSnow & snow)
{
}

bool CSnow::Init()
{
	m_fSpeed = 3.f;
	m_fDist = 20.f;
	m_random_size = (rand() % 15 + 1);
	m_random_x = 0;
	m_random_y = 0;
	m_ptGap = Vec2(0.f, 0.f);
	m_ptPos.x = 0;
	m_ptPos.y = 0;
	GetCursorPos(&m_ptPos);
	ScreenToClient(m_hWnd, &m_ptPos);
	m_ptPrevPos = m_ptPos;
	m_vSize = m_pTransform->GetWorldScale();
	m_vSizeTemp = m_pTransform->GetWorldScale();
	pt.x = 0;
	pt.y = 0;
	return true;
}

void CSnow::Input(float fTime)
{
}

int CSnow::Update(float fTime)
{

	//------------------------ 기본 ----------------------------------------------------------------
	//이동&회전
	m_pTransform->MoveWorld(10.f, fTime * 3, AXIS_X);
	m_pTransform->MoveWorld(10.f, fTime * 5, AXIS_Y);

	//----------------------- 캐릭터 이동에 따라 추가! ---------------------------------------------
	//Mouse(fTime);
	ChraterMove(fTime);
	//----------------------- 예외처리--------------------------------------------------------------
	Exception();

	return 0;
}

int CSnow::LateUpdate(float fTime)
{
	
	return 0;
}

void CSnow::Render(float fTime)
{
}

CSnow * CSnow::Clone()
{
	return new CSnow(*this);
}

void CSnow::OnCollisionEnter(CDx11Collider * pColl, float fTime)
{

}

void CSnow::OnCollision(CDx11Collider * pColl, float fTime)
{

}

void CSnow::OnCollisionExit(CDx11Collider * pColl, float fTime)
{

}
void CSnow::ChraterMove(float fTime)
{

	//키보드 --------------------------------------
	if (DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveFront") &&
		!DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveLeft") &&
		!DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveRight") &&
		!DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveBack"))
	{
		//cout << "DIR_FRONT" << endl;
		m_vSizeTemp.x += 0.1;
		m_vSizeTemp.y += 0.1;
		m_pTransform->SetWorldScale(m_vSizeTemp);

	}
	if (DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveLeft") &&
		DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveFront") &&
		!DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveRight"))
	{
		//cout << "DIR_FRONT_LEFT" << endl;
		m_vSizeTemp.x += 0.1;
		m_vSizeTemp.y += 0.1;
		m_pTransform->SetWorldScale(m_vSizeTemp);
		m_pTransform->MoveWorld(50.f, fTime * 3, AXIS_X);
	}
	if (DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveRight") &&
		DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveFront") &&
		!DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveLeft"))
	{
		//cout<<"DIR_FRONT_RIGHT" << endl;
		m_vSizeTemp.x += 0.1;
		m_vSizeTemp.y += 0.1;
		m_pTransform->SetWorldScale(m_vSizeTemp);
		m_pTransform->MoveWorld(-50.f, fTime * 3, AXIS_X);
	}
	if (DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveBack") &&
		!DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveLeft") &&
		!DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveRight") &&
		!DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveFront"))
	{
		//cout << "DIR_BACK" << endl;
		m_vSizeTemp.x -= 0.1;
		m_vSizeTemp.y -= 0.1;
		m_pTransform->SetWorldScale(m_vSizeTemp);
	}
	if (DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveLeft") &&
		DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveBack") &&
		!DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveRight"))
	{
		//cout << "DIR_BACK_LEFT" << endl;
		m_vSizeTemp.x -= 0.1;
		m_vSizeTemp.y -= 0.1;
		m_pTransform->SetWorldScale(m_vSizeTemp);
		m_pTransform->MoveWorld(50.f, fTime * 3, AXIS_X);
	}
	if (DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveRight") &&
		DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveBack") &&
		!DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveLeft"))
	{
		//cout << "DIR_BACK_RIGHT" << endl;
		m_vSizeTemp.x -= 0.1;
		m_vSizeTemp.y -= 0.1;
		m_pTransform->SetWorldScale(m_vSizeTemp);
		m_pTransform->MoveWorld(-50.f, fTime * 3, AXIS_X);
	}
	if (DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveLeft") &&
		!DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveRight") &&
		!DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveFront") &&
		!DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveBack"))
	{
		//cout << "DIR_LEFT" << endl;
		m_pTransform->MoveWorld(50.f, fTime * 3, AXIS_X);
	}
	if (DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveRight") &&
		!DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveLeft") &&
		!DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveFront") &&
		!DX11_GET_SINGLE(CDx11Input)->KeyPush("MoveBack"))
	{
		//cout << "DIR_RIGHT" << endl;
		m_pTransform->MoveWorld(-50.f, fTime * 3, AXIS_X);
	}

	//---------------------------------마우스 -------------------------

}

void CSnow::Exception()
{
	//----------------------- 예외처리1(x,y 창) -----------------------------------------------------
	if (m_pTransform->GetWorldPos().y >= 820 || m_pTransform->GetWorldPos().y <= -20) {
		m_random_y = (rand() % 200 + 1);
		m_pTransform->SetWorldPos(m_pTransform->GetWorldPos().x, m_random_y, m_pTransform->GetWorldPos().z);
	}
	if (m_pTransform->GetWorldPos().x >= 1320 || m_pTransform->GetWorldPos().x <= -20) {
		m_random_x = (rand() % 1250 + 50);
		m_pTransform->SetWorldPos(m_random_x, m_pTransform->GetWorldPos().y, m_pTransform->GetWorldPos().z);
	}

	//----------------------- 예외처리2(move) -----------------------------------------------------
	//앞
	if (m_vSizeTemp.x >(m_vSize.x + m_random_size) || m_vSizeTemp.y >(m_vSize.y + m_random_size)) {
		m_vSizeTemp.x = 0;
		m_vSizeTemp.y = 0;
	}
	//뒤
	if (m_vSizeTemp.x < 0 || m_vSizeTemp.y < 0) {
		m_vSizeTemp.x = m_vSize.x;
		m_vSizeTemp.y = m_vSize.y;
	}
}


void CSnow::Mouse(float fTime)
{
	//1. 기존값과 갭을 찾는다-------------------------------------------------

	// 마우스의 위치를 갱신한다.
	GetCursorPos(&m_ptPos);

	// 스크린 좌표를 클라이언트 좌표로 변환한다.
	ScreenToClient(m_hWnd, &m_ptPos);

	//// 마우스 관련 옵션	
	GetWindowRect(m_hWnd, &rect);
	//	ClipCursor(&rect);

	pt.x = (rect.right - rect.left) / 2;
	pt.y = (rect.bottom - rect.top) / 2;

	// 갭을 구한다.
	/*m_ptGap.x = 648.f - m_ptPos.x;
	m_ptGap.y = 379.f - m_ptPos.y;*/
	m_ptGap.x = pt.x - m_ptPos.x;
	m_ptGap.y = pt.y - m_ptPos.y;
	//m_ptPrevPos = m_ptPos;

	//cout << "x :" << pt.x << ", y : " << pt.y <<" // 갭 x:"<< m_ptGap.x << "갭 y:" << m_ptGap.y << endl;

	//2.차이만큼 속도감을준다! -------------------------------------------------
	
	//갭X 가 +일때
	//if (m_ptGap.x > 1.0f) {
	//	m_pTransform->MoveWorld(abs(m_ptGap.x)*75.f, fTime * 3, AXIS_X);
	//}
	////갭X 가 -일때
	//else if (m_ptGap.x < -1.0f) {
	//	m_pTransform->MoveWorld(-(abs(m_ptGap.x)*75.f), fTime * 3, AXIS_X);
	//}
	////갭Y 가 +일때
	//if (m_ptGap.y > 1.0f) {
	//	m_pTransform->MoveWorld(abs(m_ptGap.y)*75.f, fTime * 3, AXIS_Y);
	//}
	////갭Y 가 -일때
	//else if (m_ptGap.y < -1.0f) {
	//	m_pTransform->MoveWorld(-abs(m_ptGap.y)*50.f, fTime * 3, AXIS_Y);
	//}
	

	return;
}