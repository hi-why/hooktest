#include "stdafx.h"
#include "stdio.h"
#include "string"

DWORD OBJADDR;
DWORD OLDFUNCADDR;
DWORD DLLADDR;
DWORD JMPBACKADDR;

_declspec(naked) void AsmChangeWxVersion() {
	__asm {		
		MOV	[EAX + 2], 0x31
		MOV	[EAX + 4], 0x32
		MOV	[EAX + 8], 0x31
		CALL	OLDFUNCADDR
		JMP	JMPBACKADDR
	}
}

void ModifyAddress(DWORD hook_addr, char *origin, int sz) {
	DWORD old_protext = 0;
	char oldstrp[20];
	SIZE_T redSize = 0;

	if (!VirtualProtect((LPVOID)hook_addr, sz, PAGE_READWRITE, &old_protext)) {
		MessageBoxA(NULL, "VirtualProtect error", "提示", MB_OK);
		return;
	}
	if (!ReadProcessMemory(GetCurrentProcess(), (LPVOID)hook_addr, oldstrp, sz, &redSize)) {
		MessageBoxA(NULL, "ReadProcessMemory error", "提示", MB_OK);
		return;
	}
	//memcpy((void *)hook_addr, origin, len);
	if (!WriteProcessMemory(GetCurrentProcess(), (LPVOID)hook_addr, origin, sz, &redSize)) {
		MessageBoxA(NULL, "WriteProcessMemory error", "提示", MB_OK);
		return;
	}
	if (!VirtualProtect((LPVOID)hook_addr, sz, old_protext, &old_protext)) {
		MessageBoxA(NULL, "VirtualProtect2 error", "提示", MB_OK);
	}
}

void ChangeWxVersion() {
	HMODULE exeBase = GetModuleHandleA(NULL);
	OBJADDR = (DWORD)exeBase + 17898584;
	OLDFUNCADDR = (DWORD)exeBase + 13379184 - 12386304;

	char newOpCode[5] = { 0 };
	newOpCode[0] = 0xE9;
	*(long *)&newOpCode[1] = ((long)AsmChangeWxVersion - OBJADDR - 5);

	ModifyAddress((DWORD)OBJADDR, newOpCode, 5);
	JMPBACKADDR = OBJADDR + 5;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
	DLLADDR = (DWORD)hModule;
    switch (ul_reason_for_call)
    {
		case DLL_PROCESS_ATTACH:
			ChangeWxVersion();
			//MessageBox(NULL, L"DLL_PROCESS_ATTACH 成功了", L"提示", MB_OK);
			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}
