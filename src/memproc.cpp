#include "memproc.h"



DWORD GetProcessId(const wchar_t* procName)
{
	DWORD procId = NULL;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	PROCESSENTRY32 procEntry;
	procEntry.dwSize = sizeof(PROCESSENTRY32);
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		if (Process32First(hSnap, &procEntry))
		{
			do
			{
				if (!_wcsicmp(procName, procEntry.szExeFile))
				{
					procId = procEntry.th32ProcessID;
					break;
				}

			} while (Process32Next(hSnap, &procEntry));
		}
	}
	CloseHandle(hSnap);
	return procId;
}



uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName)
{
	uintptr_t modBaseAddr = NULL;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, procId);
	MODULEENTRY32 modEntry;
	modEntry.dwSize = sizeof(MODULEENTRY32);
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		if (Module32First(hSnap, &modEntry))
		{
			do
			{
				if (!_wcsicmp(modName, modEntry.szModule))
				{
					modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
					break;
				}

			} while (Module32Next(hSnap, &modEntry));
		}
	}
	CloseHandle(hSnap);
	return modBaseAddr;
}

uintptr_t FindDynamicAddr(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int>offsets)
{
	uintptr_t dynamicAddr = ptr;
	for (unsigned int i = 0; i < offsets.size(); i++)
	{
		ReadProcessMemory(hProc, (LPVOID)dynamicAddr, &dynamicAddr, sizeof(dynamicAddr), 0);
		dynamicAddr += offsets[i];
	}
	return dynamicAddr;

}


void PatchEx(HANDLE hProc, LPVOID Addr, LPCVOID buffer, SIZE_T size)
{
	DWORD dwOldProtect;
	VirtualProtectEx(hProc, Addr, size, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	WriteProcessMemory(hProc, Addr, buffer, size, 0);
	VirtualProtectEx(hProc, Addr, size, dwOldProtect, &dwOldProtect);
}


void NopEx(HANDLE hProc, LPVOID Addr, SIZE_T size)
{
	BYTE* nopArray = new BYTE[size];
	memset(nopArray, 0x90, size);
	PatchEx(hProc, Addr, nopArray, size);
	delete[] nopArray;
}

uintptr_t GetAdressOfElementInMemory(int start, int step, int index)
{
	uintptr_t slotAddress;
	slotAddress = start + index * (step * 4);
	return slotAddress;
}