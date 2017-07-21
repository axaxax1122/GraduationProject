#pragma once

#include "../Dx11Object.h"

DX11_BEGIN

class DX11_DLL CDx11Window :
	public CDx11Object
{
private:
	friend class CDx11Core;

private:
	CDx11Window();
	~CDx11Window();

private:
	HWND		m_hWnd;
	HINSTANCE	m_hInst;
	RESOLUTION_TYPE	m_eRSType;

public:
	RESOLUTION_TYPE GetResolutionType();
	HWND GetWindowHandle();

public:
	bool Init(wchar_t* pTitle, wchar_t* pClass, HINSTANCE hInst,
		int iIconID, int iSmallIconID, RESOLUTION_TYPE eRT,
		WNDPROC pProc);

private:
	ATOM MyRegisterClass(HINSTANCE hInst, wchar_t* pClass,
		int iIconID, int iSmallIconID, WNDPROC pProc);
	BOOL InitInstance(wchar_t* pTitle, wchar_t* pClass, RESOLUTION_TYPE eRT);
};

DX11_END