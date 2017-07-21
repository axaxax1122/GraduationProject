
#include "ClientCore.h"
#include "Dump.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	CDump::Begin();
	if (!GET_SINGLE(CClientCore)->Init(L"Cube Crash", L"Cube Crash",
		hInstance, IDI_ICON1, IDI_ICON1, RT_HD, true))
	{
		DESTROY_SINGLE(CClientCore);
		return 0;
	}

	int	iRet = GET_SINGLE(CClientCore)->Run();

	DESTROY_SINGLE(CClientCore);

	CDump::End();

	return iRet;
}