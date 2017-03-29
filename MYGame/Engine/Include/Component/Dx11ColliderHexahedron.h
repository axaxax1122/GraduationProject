#pragma once
#include "Dx11Collider.h"

DX11_BEGIN

class DX11_DLL Dx11ColliderHexahedron :
	public CDx11Collider
{
public:
	Dx11ColliderHexahedron();
	~Dx11ColliderHexahedron();

	Dx11ColliderHexahedron(const Dx11ColliderHexahedron& collider);
	

private:
	ST_OBB ObbInfo;

public:
	void SetInfo(float fLenth);
	void SetInfo(const Vec3& vCenter, float fLenth);
	void SetInfo(const Vec3 & vCenter, Vec3 & fRot, Vec3 & fScale);
	void MoveWorld(const Vec3 & vMove);

public:
	virtual bool Init();
	virtual void Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual void Render(float fTime);
	virtual Dx11ColliderHexahedron* Clone();

public:
	virtual bool Collision(CDx11Collider* pColl);

};


DX11_END
