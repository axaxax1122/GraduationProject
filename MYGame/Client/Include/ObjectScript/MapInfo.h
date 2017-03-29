#pragma once
#include "Component\Dx11Script.h"

DX11_USING

class FBXINFO {
public:
	string MeshName, name;
	float PosX, PosY, PosZ;
	float RotX, RotY, RotZ;
	float ScaleX, ScaleY, ScaleZ;
};

class CMapInfo :
	public CDx11Script
{
public:
	CMapInfo();
	CMapInfo(const CMapInfo& map);
	~CMapInfo();

public:	
	FBXINFO			info;
	vector<FBXINFO>	m_vInfo;
	unordered_map<string, FBXINFO>		m_vInfoPos;

public:
	vector<FBXINFO> *GetInfo();
public:
	void Road();
	void Well();
	void Object();
	void One();
	void Two();
	void Three();
	void Four();

public:
	virtual bool Init();
	virtual void Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual void Render(float fTime);
	virtual CMapInfo* Clone();
	virtual void OnCollisionEnter(class CDx11Collider* pColl, float fTime);
	virtual void OnCollision(class CDx11Collider* pColl, float fTime);
	virtual void OnCollisionExit(class CDx11Collider* pColl, float fTime);

};

