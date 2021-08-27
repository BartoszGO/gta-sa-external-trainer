#pragma once
#include<Windows.h>
#include"structs.h"
//#include"entityclass.h"


class HackClass
{
private:
	const wchar_t* processName;
	DWORD procId;
	HANDLE hProc;
	uintptr_t moduleBaseAddr;
	uintptr_t playerBasePtr;
	uintptr_t currentCarPtr;
	uintptr_t playerAddr;
	uintptr_t playerPosAddr;
	uintptr_t healthAddr;
	uintptr_t armorAddr;
	offsets ofOffsets;
	vec3 playerSavedPos;
	vec3 targetEntityPos;
	vec3 sniperBulletPos;
	switches swSwitches;
	

public:
	HackClass();
	~HackClass();
	void SavePlayerPosition();
	void LoadPlayerSavedPosition();
	void LoadPlayerPosition(vec3 pos);
	void MovePlayerPosition(vec3 movement);
	void SetPlayerPosition(vec3 pos);
	vec3 GetEntityPosition(uintptr_t entity);
	vec3 RotatePoint(float cx, float cy, float angle, vec3 p);
	void AirBreak();
	void AirBreakOff();
	void TeleportToTargetEntity();
	void TeleportToBulletLocation();
	uintptr_t CheckIfInVehicle();
	void ChangeFloatValue(float value, uintptr_t addr);
	void FreezeAmmo();
	void UnfreezeAmmo();
	void KillWithSigth();
	void SetFlags(uintptr_t entity,bool setFlag, bool isCar);
	void MakeCurrentVehicleGodMode(bool isEnabled);
	void FixCurrentVehicle();
	void VehicleSpeedBoost();
	void VehicleJump();
	void MainHackLoop();
	void MenuDisplay();
};