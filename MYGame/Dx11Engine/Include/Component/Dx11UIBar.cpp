#include "Dx11UIBar.h"
#include "Dx11Transform.h"

DX11_USING

CDx11UIBar::CDx11UIBar()
{
	SetTypeName<CDx11UIBar>();
	m_fMin = 0.f;
	m_fMax = 1.f;
	m_fValue = 0.f;
	m_eBarDir = UBD_LEFT;
}

CDx11UIBar::CDx11UIBar(const CDx11UIBar & ui) :
	CDx11UI(ui)
{
	m_vBarSize = ui.m_vBarSize;
	m_vBarPos = ui.m_vBarPos;
	m_fMin = ui.m_fMin;
	m_fMax = ui.m_fMax;
	m_fValue = ui.m_fValue;
	m_eBarDir = ui.m_eBarDir;
}

CDx11UIBar::~CDx11UIBar()
{
}

void CDx11UIBar::SetBarDir(UIBAR_DIR eDir)
{
	m_eBarDir = eDir;
}

void CDx11UIBar::SetBarSize(const Vec2 & vSize)
{
	m_vBarSize = vSize;

	m_pTransform->SetWorldScale(vSize.x, vSize.y, 1.f);
}

void CDx11UIBar::SetBarSize(float x, float y)
{
	m_vBarSize.x = x;
	m_vBarSize.y = y;

	m_pTransform->SetWorldScale(x, y, 1.f);
}

void CDx11UIBar::SetBarPos(const Vec2 & vPos)
{
	m_vBarPos = vPos;
	m_pTransform->SetWorldPos(vPos.x, vPos.y, 0.f);
}

void CDx11UIBar::SetBarPos(float x, float y)
{
	m_vBarPos.x = x;
	m_vBarPos.y = y;
	m_pTransform->SetWorldPos(x, y, 0.f);
}

void CDx11UIBar::SetValueMinMax(float fMin, float fMax)
{
	m_fMin = fMin;
	m_fMax = fMax;
}

void CDx11UIBar::SetValue(float fValue)
{
	m_fValue = fValue;

	if (m_fValue < m_fMin)
		m_fValue = m_fMin;

	else if (m_fValue > m_fMax)
		m_fValue = m_fMax;
}

void CDx11UIBar::AddValue(float fValue)
{
	m_fValue += fValue;

	if (m_fValue < m_fMin)
		m_fValue = m_fMin;

	else if (m_fValue > m_fMax)
		m_fValue = m_fMax;
}

bool CDx11UIBar::Init()
{
	return true;
}

void CDx11UIBar::Input(float fTime)
{
}

int CDx11UIBar::Update(float fTime)
{
	return 0;
}

int CDx11UIBar::LateUpdate(float fTime)
{
	float	fValue = m_fValue - m_fMin;//현재 hp
	float	fLength = m_fMax - m_fMin;//전체

	fValue /= fLength;//현재 hp /전체 = 비율ㅇㅇ

	switch (m_eBarDir)
	{
	case UBD_LEFT:
		m_pTransform->SetWorldScale(m_vBarSize.x * fValue, m_vBarSize.y, 1.f);
		break;
	case UBD_RIGHT:
		m_pTransform->SetWorldPos(m_vBarPos.x + m_vBarSize.x * (1.f - fValue), m_vBarPos.y,
			0.f);
		m_pTransform->SetWorldScale(m_vBarSize.x * fValue, m_vBarSize.y, 1.f);
		break;
	case UBD_DOWN:
		m_pTransform->SetWorldPos(m_vBarPos.x, m_vBarPos.y + m_vBarSize.y * (1.f - fValue),
			0.f);
		m_pTransform->SetWorldScale(m_vBarSize.x, m_vBarSize.y * fValue, 1.f);
		break;
	case UBD_UP:
		m_pTransform->SetWorldScale(m_vBarSize.x, m_vBarSize.y * fValue, 1.f);
		break;
	}

	return 0;
}

void CDx11UIBar::Render(float fTime)
{
}

CDx11UIBar * CDx11UIBar::Clone()
{
	return new CDx11UIBar(*this);
}
