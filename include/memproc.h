#pragma once
#include<Windows.h>
#include<TlHelp32.h>
#include<vector>


DWORD GetProcessId(const wchar_t* procName);
uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName);
uintptr_t FindDynamicAddr(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int>offsets);
uintptr_t FindDynamicAddr2(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int>offsets);
void PatchEx(HANDLE hProc, LPVOID Addr, LPCVOID buffer, SIZE_T size);
void NopEx(HANDLE hProc, LPVOID Addr, SIZE_T size);
uintptr_t GetAdressOfElementInMemory(int start, int step, int index);
