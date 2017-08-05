#pragma once
#include "Dx11Component.h"

DX11_BEGIN

class DX11_DLL CDx11FSM :
	public CDx11Component
{
public:
	CDx11FSM();
	CDx11FSM(const CDx11FSM& fsm);
	~CDx11FSM();

private:
	vector<class CDx11State*>	m_vecStateList;
	class CDx11State*			m_pCurState;
	int							m_iStateFlag;
	float						m_fTime;

public:
	CDx11State*	GetCurrentState() const;
	CDx11State* FindStateFromList(const string& strClipName);
	void SetCurrentState(const string& strName);
	void AddState(const string& strName, float fStartTime, float fEndTime);

public:
	virtual bool Init();
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual void Render(float fTime);
	virtual CDx11FSM* Clone();


};

DX11_END

