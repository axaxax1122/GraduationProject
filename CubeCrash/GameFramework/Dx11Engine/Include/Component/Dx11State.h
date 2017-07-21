#pragma once
#include "../Dx11Object.h"

DX11_BEGIN

class DX11_DLL CDx11State :
	public CDx11Object
{
private:
	friend class CDx11FSM;

private:
	CDx11State();
	~CDx11State();

private:
	string	m_strClipName;
	float	m_fStartTime;
	float	m_fEndTime;

public:
	string GetClipName() const;
	float GetStartTime() const;
	float GetEndTime() const;

public:
	void SetStateInfo(const string& strName, float fStartTime, float fEndTime);

};

DX11_END