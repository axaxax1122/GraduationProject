#include "Dx11FSM.h"
#include "Dx11State.h"
#include "../GameObject/Dx11GameObject.h"

DX11_USING

CDx11FSM::CDx11FSM()	:
	m_pCurState(NULL)
{
	m_eComType = CT_FSM;
}

CDx11FSM::CDx11FSM(const CDx11FSM & fsm)
{
	m_iStateFlag = fsm.m_iStateFlag;
	m_fTime = fsm.m_fTime;

	for (size_t i = 0; i < fsm.m_vecStateList.size(); ++i)
	{
		m_vecStateList.push_back(fsm.m_vecStateList[i]);
	}
	m_pCurState = fsm.m_pCurState;
}

CDx11FSM::~CDx11FSM()
{
	Safe_Release_VecList(m_vecStateList);
}

CDx11State * CDx11FSM::GetCurrentState() const
{
	if (!m_pCurState)
		return NULL;

	m_pCurState->AddRef();
	return m_pCurState;
}

CDx11State* CDx11FSM::FindStateFromList(const string & strClipName)
{
	vector<CDx11State*>::iterator	iter;
	vector<CDx11State*>::iterator	iterEnd = m_vecStateList.end();

	for (iter = m_vecStateList.begin(); iter != iterEnd; ++iter)
	{
		if ((*iter)->GetClipName() == strClipName)
		{
			return *iter;
		}
	}
	return NULL;
}


void CDx11FSM::SetCurrentState(const string & strName)
{
	CDx11State* pState = FindStateFromList(strName);
	if (pState)
		m_pCurState = pState;
}

void CDx11FSM::AddState(const string & strName, float fStartTime, float fEndTime)
{
	CDx11State* pState = new CDx11State;
	pState->SetStateInfo(strName, fStartTime, fEndTime);

	m_vecStateList.push_back(pState);
}



bool CDx11FSM::Init()
{
	m_iStateFlag = STATE_IDLE;
	m_fTime = 0.f;
	return true;
}

int CDx11FSM::Update(float fTime)
{
	if (m_iStateFlag == m_pGameObject->GetStateFlag())
		m_fTime += fTime;
	else
		m_fTime = 0.f;

	m_iStateFlag = m_pGameObject->GetStateFlag();

	if ((m_iStateFlag & STATE_JUMP) && (m_iStateFlag & STATE_SHOOT))
	{
		m_pCurState = FindStateFromList("JUMP_SHOOT");
	}
	else if ((m_iStateFlag & STATE_WALK) && (m_iStateFlag & STATE_SHOOT))
	{
		m_pCurState = FindStateFromList("WALK_SHOOT");
	}
	else if ((m_iStateFlag & STATE_IDLE) && (m_iStateFlag & STATE_SHOOT))
	{
		m_pCurState = FindStateFromList("SHOOT");
	}
	else if ((m_iStateFlag & STATE_WALK) && (m_iStateFlag & STATE_LYING))
	{
		m_pCurState = FindStateFromList("CRAWLING");
	}
	else if (m_iStateFlag & STATE_LYING)
	{
		m_pCurState = FindStateFromList("LYING");
	}
	else if (m_iStateFlag & STATE_ROLL)
	{
		m_pCurState = FindStateFromList("ROLL_FRONT");
	}
	else if (m_iStateFlag & STATE_JUMP)
	{
		m_pCurState = FindStateFromList("JUMP");
	}
	else if (m_iStateFlag & STATE_WALK)
	{
		m_pCurState = FindStateFromList("RUN");
	}
	else if (m_iStateFlag & STATE_IDLE)
	{
		if (m_fTime < 3.f)
			m_pCurState = FindStateFromList("IDLE1");
		else
		{
			m_pCurState = FindStateFromList("IDLE2");
			if (m_fTime > 4.967f)
				m_fTime = 0.f;
		}
	}





	return 0;
}

int CDx11FSM::LateUpdate(float fTime)
{
	return 0;
}

void CDx11FSM::Render(float fTime)
{
}

CDx11FSM * CDx11FSM::Clone()
{
	return new CDx11FSM(*this);
}

