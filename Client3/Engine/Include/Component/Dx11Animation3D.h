#pragma once
#include "Dx11Component.h"

DX11_BEGIN

class DX11_DLL CDx11Animation3D :
	public CDx11Component
{
private:
	friend class CDx11GameObject;
	friend class CDx11Mesh;

public:
	CDx11Animation3D();
	CDx11Animation3D(const CDx11Animation3D& anim);
	~CDx11Animation3D();

private:
	vector<class CDx11AnimationClip*>	m_vecAnimationClip;
	float			m_fGlobalTime;
	UINT			m_iCurrentClip;
	vector<BONE>	m_vecBoneHierarchy;
	BONECBUFFER		m_tBoneCBuffer;
	class CDx11Texture*	m_pBoneTexture;
	string				m_strName;
	ANIMATION_STATE		m_eState;
	float				m_fStartTime;
	float				m_fEndTime;

public:
	virtual bool Init();
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual void Render(float fTime);
	virtual CDx11Animation3D* Clone();

public:
	void SetMeshName(const string& strName);
	void CreateBones(vector<BONE>& vecBone);
	void AddAnimationClip(const ANIMATIONCLIP& tClip);
	void UpdateConstantBuffer(class CDx11Shader* pShader);
	void UpdateAnimationState(float fTime);
	
};

DX11_END