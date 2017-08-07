#define WIN32_LEAN_AND_MEAN		
#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <DbgHelp.h>

class CDump
{
public:
	static BOOL Begin(VOID);
	static BOOL End(VOID);
};

