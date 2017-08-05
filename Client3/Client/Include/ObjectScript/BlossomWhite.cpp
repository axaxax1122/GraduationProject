#include "BlossomWhite.h"
#include "GameObject/Dx11GameObject.h"
#include "Component/Dx11Transform.h"
#include "Player.h"
#include "Scene/Dx11Layer.h"
#include "Scene/Dx11Scene.h"
#include "Core/Dx11Device.h"
#include "Core/Dx11Input.h"
#include "Component/Dx11Mouse.h"
#include<iostream>	
using namespace std;

CBlossomWhite::CBlossomWhite()
{
}


CBlossomWhite::~CBlossomWhite()
{
	//�ܼ�
	//FreeConsole();
}

CBlossomWhite::CBlossomWhite(const CBlossomWhite & BW)
{
}


bool CBlossomWhite::Init()
{
	m_fSpeed = 3.f;
	m_fDist = 20.f;
	m_random_size = (rand() % 10 + 1);
	m_random_x = 0;
	m_random_y= 0;
	m_vSize = m_pTransform->GetWorldScale();
	m_vSizeTemp = m_pTransform->GetWorldScale();
	m_hWnd = DX11_GET_SINGLE(CDx11Input)->GetHWND();

	
	m_ptGap = Vec2(0.f, 0.f);

	GetCursorPos(&m_ptPos);
	ScreenToClient(m_hWnd, &m_ptPos);
	m_ptPrevPos = m_ptPos;

	//�ܼ�
	//AllocConsole();
	//freopen("CONOUT$", "wt", stdout);
	
	return true;
}

void CBlossomWhite::Input(float fTime)
{
}

int CBlossomWhite::Update(float fTime)
{	
	//------------------------ �⺻ ----------------------------------------------------------------
	//�̵�&ȸ��
	m_pTransform->MoveWorld(-15.f, fTime * 3, AXIS_X);
	m_pTransform->MoveWorld(8.f, fTime * 5, AXIS_Y);	

	//----------------------- ĳ���� �̵�&���콺�� ���� �߰�! ---------------------------------------------
	Mouse(fTime);
	ChraterMove(fTime);
	//----------------------- ����ó��--------------------------------------------------------------
	Exception();
	
	
	
	return 0;
}

int CBlossomWhite::LateUpdate(float fTime)
{
	return 0;
}

void CBlossomWhite::Render(float fTime)
{
}

CBlossomWhite * CBlossomWhite::Clone()
{
	return new CBlossomWhite(*this);
}


void CBlossomWhite::ChraterMove(float fTime)
{

	//Ű���� --------------------------------------
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

	//---------------------------------���콺 -------------------------
	
}

void CBlossomWhite::Exception()
{
	//----------------------- ����ó��1(x,y â) -----------------------------------------------------
	
	

	if (m_pTransform->GetWorldPos().y >= 740 || m_pTransform->GetWorldPos().y <= -20) {
		m_random_y = (rand() % 10 + 1);
		m_pTransform->SetWorldPos(m_pTransform->GetWorldPos().x, m_random_y, m_pTransform->GetWorldPos().z);
	}
	if (m_pTransform->GetWorldPos().x >= 1300|| m_pTransform->GetWorldPos().x <= -20) {
		m_random_x = (rand() % 1250 + 50);
		m_pTransform->SetWorldPos(m_random_x, m_pTransform->GetWorldPos().y, m_pTransform->GetWorldPos().z);
	}

	//----------------------- ����ó��2(move) -----------------------------------------------------
	//��
	if (m_vSizeTemp.x >(m_vSize.x + m_random_size) || m_vSizeTemp.y >(m_vSize.y + m_random_size)) {
		m_vSizeTemp.x = 0;
		m_vSizeTemp.y = 0;
	}	
	//��
	if (m_vSizeTemp.x < 0 || m_vSizeTemp.y < 0) {
		m_vSizeTemp.x = m_vSize.x;
		m_vSizeTemp.y = m_vSize.y;
	}
}


void CBlossomWhite::Mouse(float fTime)
{
	//1. �������� ���� ã�´�-------------------------------------------------

	// ���콺�� ��ġ�� �����Ѵ�.
	GetCursorPos(&m_ptPos);

	// ��ũ�� ��ǥ�� Ŭ���̾�Ʈ ��ǥ�� ��ȯ�Ѵ�.
	ScreenToClient(m_hWnd, &m_ptPos);

	//// ���콺 ���� �ɼ�	
	GetWindowRect(m_hWnd, &rect);
	//	ClipCursor(&rect);

	pt.x = (rect.right - rect.left) / 2;
	pt.y = (rect.bottom - rect.top) / 2;

	// ���� ���Ѵ�.
	/*m_ptGap.x = 648.f - m_ptPos.x;
	m_ptGap.y = 379.f - m_ptPos.y;*/
	m_ptGap.x = pt.x - m_ptPos.x;
	m_ptGap.y = pt.y - m_ptPos.y;
	//m_ptPrevPos = m_ptPos;

	//cout << "x :" << pt.x << ", y : " << pt.y <<" // �� x:"<< m_ptGap.x << "�� y:" << m_ptGap.y << endl;

	//2.���̸�ŭ �ӵ������ش�! -------------------------------------------------
	//��X �� +�϶�
	if (m_ptGap.x > 1.0f&&m_ptGap.x < 50.0f) {
		m_pTransform->MoveWorld(abs(m_ptGap.x)*75.f, fTime * 3, AXIS_X);
	}
	//��X �� -�϶�
	else if (m_ptGap.x < -1.0f&&m_ptGap.x > -50.0f) {
		m_pTransform->MoveWorld(-(abs(m_ptGap.x)*75.f), fTime * 3, AXIS_X);
	}
	//��Y �� +�϶�
	if (m_ptGap.y > 1.0f && m_ptGap.y < 50.0f) {
		m_pTransform->MoveWorld(abs(m_ptGap.y)*75.f, fTime * 3, AXIS_Y);
	}
	//��Y �� -��
	else if (m_ptGap.y < -1.0f && m_ptGap.y > -50.0f ) {
		m_pTransform->MoveWorld(-abs(m_ptGap.y)*50.f, fTime * 3, AXIS_Y);
	}

	return;
}