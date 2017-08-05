#pragma once
#include "../Dx11Object.h"

DX11_BEGIN

class DX11_DLL Dx11SoundManager :
	public CDx11Object
{
	DX11_DECLARE_SINGLE(Dx11SoundManager)

public :
	enum SoundNumber {
		BGM1,
		BGM2,
		WALK1,
		JUMP,
		WALL,
		FIRE,
		DAMAGE,
		Result,
		Loding,
		Dumpling,

	};


public:
	virtual bool Init();
	virtual void Input(float fTime);
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual void Render(float fTime);
	virtual Dx11SoundManager* Clone();
	template <typename T>
	void SetTypeName()
	{
		m_strTypeName = typeid(T).name() + 12;
	}

public :
	void InitSound();
	void ReleaseSound();
public:
	void PlayBGM1();
	void PlayBGM2();
	void PlayWALK1();
	void PlayJUMP();
	void PlayWALL();
	void PlayFIRE();
	void PlayDAMAGE();
	void PlayResult();
	void PlayLoding();
	void PlayDumpling();
};

DX11_END
