#pragma once
#include<Windows.h>
#include"structs.h"


class hackClass
{
private:
	const wchar_t* processName;
	DWORD procId;
	HANDLE hProc;
	uintptr_t moduleBaseAddr;
	uintptr_t dynamicPtrPlayerBaseAddr;
	uintptr_t dynamicPtrCurrentCarBaseAddr;
	uintptr_t playerPosAddr;
	uintptr_t weaponStructAddr;
	uintptr_t healthAddr;
	uintptr_t armorAddr;
	offsets ofOffsets;
	vec3 playerPos;
	vec3 currentCarPost;
	vec3 targetEntityPos;
	switches swSwitches;
public:
	hackClass();
	~hackClass();
	void SavePlayerPosition();
	void LoadPlayerPosition();
	void TeleportToTargetEntity();
	void ChangeFloatValue(float value);
	void FreezeAmmo();
	void UnfreezeAmmo();
	void KillWithSigth();
	void MainHackLoop();
	void MenuDisplay();
};