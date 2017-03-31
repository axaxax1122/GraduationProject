#include "Dx11SnowUI.h"
#include "Dx11Transform.h"

DX11_USING

CDx11SnowUI::CDx11SnowUI()
{
	
}
CDx11SnowUI::~CDx11SnowUI()
{
}

CDx11SnowUI::CDx11SnowUI(const CDx11SnowUI & ui) :
	CDx11UI(ui)
{
	
}

void CDx11SnowUI::SetBarPos(float x, float y,float z)
{	
	m_vBarPos.x = x;
	m_vBarPos.y = y;
	m_pTransform->SetWorldPos(x, y, z);	
}

void CDx11SnowUI::SetBarSize(float x, float y)
{
	m_vBarSize.x = x;
	m_vBarSize.y = y;

	m_pTransform->SetWorldScale(x, y, 1.f);
}

bool CDx11SnowUI::Init()
{
	return true;
}

void CDx11SnowUI::Input(float fTime)
{
}

int CDx11SnowUI::Update(float fTime)
{
	return 0;
}

int CDx11SnowUI::LateUpdate(float fTime)
{
	return 0;
}

void CDx11SnowUI::Render(float fTime)
{
}

CDx11SnowUI * CDx11SnowUI::Clone()
{
	return new CDx11SnowUI(*this);
}
