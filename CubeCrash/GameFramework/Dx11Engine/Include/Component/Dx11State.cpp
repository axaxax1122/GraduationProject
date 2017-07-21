#include "Dx11State.h"

DX11_USING

CDx11State::CDx11State()
{
}


CDx11State::~CDx11State()
{
}

string CDx11State::GetClipName() const
{
	return m_strClipName;
}

float CDx11State::GetStartTime() const
{
	return m_fStartTime;
}

float CDx11State::GetEndTime() const
{
	return m_fEndTime;
}

void CDx11State::SetStateInfo(const string & strName, float fStartTime, float fEndTime)
{
	m_strClipName = strName;
	m_fStartTime = fStartTime;
	m_fEndTime = fEndTime;
}
