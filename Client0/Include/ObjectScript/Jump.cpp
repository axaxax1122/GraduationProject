#include "Jump.h"
#include "Component/Dx11Transform.h"
#include "GameObject/Dx11GameObject.h"
#include "Scene/Dx11Layer.h"
#include "Core/Dx11Input.h"
#include "Scene/Dx11Scene.h"
#include "PlayerCollision.h"
#include "../Network/NetworkManager.h"

CJump::CJump() :
	_jumpPower(0),
	_gravity(0),
	_isJump(false)
{
}
CJump::~CJump()
{
}
CJump::CJump(const CJump & jump) :
	CDx11Script(jump)
{
}
bool CJump::Init()
{
	DX11_GET_SINGLE(CDx11Input)->CreateKey("Jump", 'G');

	return true;
}

void CJump::Input(float fTime)
{
	if (DX11_GET_SINGLE(CDx11Input)->KeyDown("Jump"))
	{
		if (!_isJump) {
			_jumpPower = 0.4;
			_gravity = 0.04f;
			_isJump = true;
		}
	}
}

int CJump::Update(float fTime)
{
	CDx11GameObject *pPlayer = m_pLayer->FindGameObjectFromTag("PlayerObject");
	CDx11Transform *pPlayerTr = pPlayer->GetTransform();
	CPlayerCollision *pPlayerColl = (CPlayerCollision*)pPlayer->FindComponentFromTag("PlayerCollisionGo");

	//점프일 경우
	if (_isJump) {
		m_pTransform->MoveWorld(_jumpPower, 1, AXIS_Y);
		GET_SINGLE(CNetworkManager)->JumpPlayer(m_pTransform->GetWorldPos());
		_jumpPower -= _gravity;

		//탈출 조건
		if ((pPlayerColl->GetPS()) == DownColl) {
			_jumpPower = _gravity = 0;
			_isJump = false;
		}

	}

	//버그로 바닥으로 떨어지는경우=>최수의 수단
	if (pPlayerTr->GetWorldPos().y < -3.0f) {
		_jumpPower = _gravity = 0;
		_isJump = false;
		pPlayerTr->SetWorldPos(pPlayerTr->GetWorldPos().x, 0.5, pPlayerTr->GetWorldPos().z);
	}

	SAFE_RELEASE(pPlayerColl);
	SAFE_RELEASE(pPlayerTr);
	SAFE_RELEASE(pPlayer);
	return 0;
}

int CJump::LateUpdate(float fTime)
{


	return 0;
}

void CJump::Render(float fTime)
{
}

CJump * CJump::Clone()
{
	return nullptr;
}

void CJump::OnCollisionEnter(CDx11Collider * pColl, float fTime)
{
}

void CJump::OnCollision(CDx11Collider * pColl, float fTime)
{
}

void CJump::OnCollisionExit(CDx11Collider * pColl, float fTime)
{
}
