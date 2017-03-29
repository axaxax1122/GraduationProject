#include "Dx11Snow.h"
#include "GameObject/Dx11GameObject.h"
#include "Component/Dx11Transform.h"


CDx11Snow::CDx11Snow()
{
}


CDx11Snow::~CDx11Snow()
{
}

CDx11Snow::CDx11Snow(const CDx11Snow & snow):
	CDx11Script(snow)
{
}

bool CDx11Snow::Init()
{
	return true;
}

void CDx11Snow::Input(float fTime)
{
}

int CDx11Snow::Update(float fTime)
{
	return 0;
}

int CDx11Snow::LateUpdate(float fTime)
{
	return 0;
}

void CDx11Snow::Render(float fTime)
{
}

CDx11Snow * CDx11Snow::Clone()
{
	return new CDx11Snow(*this);
}

void CDx11Snow::OnCollisionEnter(CDx11Collider * pColl, float fTime)
{
}

void CDx11Snow::OnCollision(CDx11Collider * pColl, float fTime)
{
}

void CDx11Snow::OnCollisionExit(CDx11Collider * pColl, float fTime)
{
}
