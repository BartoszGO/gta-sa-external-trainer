#pragma once
#include<Windows.h>
#include"structs.h"


class HackClass
{
private:
	const wchar_t* processName;
	DWORD procId;
	HANDLE hProc;
	uintptr_t moduleBaseAddr;
	uintptr_t dynamicPtrPlayerBaseAddr;
	uintptr_t dynamicPtrCurrentCarBaseAddr;
	uintptr_t playerPosAddr;
	uintptr_t savedRcCarAddr;
	uintptr_t weaponStructAddr;
	uintptr_t healthAddr;
	uintptr_t armorAddr;
	offsets ofOffsets;
	vec3 playerPos;
	vec3 currentCarPost;
	vec3 targetEntityPos;
	vec3 sniperBulletLocation;
	switches swSwitches;
public:
	HackClass();
	~HackClass();
	void SavePlayerPosition();
	void LoadPlayerSavedPosition();
	void LoadPlayerPosition(vec3 pos);
	void TeleportToTargetEntity();
	void TeleportToBulletLocation();
	uintptr_t CheckIfInVehicle();
	void ChangeFloatValue(float value, uintptr_t addr);
	void FreezeAmmo();
	void UnfreezeAmmo();
	void KillWithSigth();
	void MakeCurrentVehicleGodMode(bool isEnabled);
	void FixCurrentVehicle();
	void VehicleSpeedBoost();
	void VehicleJump();
	void MainHackLoop();
	void MenuDisplay();
};