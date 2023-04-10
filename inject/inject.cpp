#include "pch.h"
#include <iostream>
#include <string>
using namespace std;

BOOL InjectDll(DWORD pid, char* dllPath) {
	printf("InjectDll %d  %s \n", pid, dllPath);
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (hProcess == NULL) {
		MessageBox(NULL, L"无法打开进程", L"错误", MB_OK);
		return FALSE;
	}

	LPVOID lpBaseAddress = VirtualAllocEx(hProcess, NULL, strlen(dllPath) + 1, MEM_COMMIT, PAGE_READWRITE);
	if (lpBaseAddress == NULL) {
		MessageBox(NULL, L"无法在目标进程中分配内存", L"错误", MB_OK);
		CloseHandle(hProcess);
		return FALSE;
	}

	SIZE_T dwWriteSize = 0;
	if (!WriteProcessMemory(hProcess, lpBaseAddress, dllPath, strlen(dllPath) + 1, &dwWriteSize)) {
		MessageBox(NULL, L"无法在目标进程中写入数据", L"错误", MB_OK);
		VirtualFreeEx(hProcess, lpBaseAddress, 0, MEM_RELEASE);
		CloseHandle(hProcess);
		return FALSE;
	}
	
	FARPROC pLoadLibraryAddress = GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");

	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pLoadLibraryAddress, lpBaseAddress, 0, NULL);
	if (hThread == NULL) {
		MessageBox(NULL, L"无法创建远程线程", L"错误", MB_OK);
		VirtualFreeEx(hProcess, lpBaseAddress, 0, MEM_RELEASE);
		CloseHandle(hProcess);
		return FALSE;
	}

	WaitForSingleObject(hThread, INFINITE);

	DWORD exitCode;
	GetExitCodeThread(hThread, &exitCode);
	if (exitCode == NULL) {
		MessageBox(NULL, L"注入失败", L"错误", MB_OK);
		CloseHandle(hThread);
		VirtualFreeEx(hProcess, lpBaseAddress, 0, MEM_RELEASE);
		CloseHandle(hProcess);
		return FALSE;
	}
	MessageBox(NULL, L"DLL注入成功", L"成功", MB_OK);
	CloseHandle(hThread);
	VirtualFreeEx(hProcess, lpBaseAddress, 0, MEM_RELEASE);
	CloseHandle(hProcess);
	return TRUE;
}

int main(int argc, char* argv[])
{
	if (argc < 2) {
		cout << "缺少参数" << endl;
		return 0;
	}

	DWORD pid = stoi(argv[1]);
	char dllPath[] = "D:\\hooktest\\dll.dll";
	InjectDll(pid, dllPath);
	return 0;
}
