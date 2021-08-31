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
	uintptr_t playerPtrAddr;
	uintptr_t currentCarPtrAddr;
	uintptr_t playerPtr;
	uintptr_t playerPosPtr;
	uintptr_t healthPtr;
	uintptr_t armorPtr;
	offsets ofOffsets;
	vec3 playerSavedPos;
	vec3 targetEntityPos;
	vec3 sniperBulletPos;
	switches swSwitches;
	Flags flags;
	

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
	void TeleportToSniperBulletLocation();
	uintptr_t CheckIfInVehicle();
	void FreezeHealth();
	void ChangeFloatValue(float value, uintptr_t addr);
	void FreezeAmmo();
	void UnfreezeAmmo();
	void KillWithSight();
	void SetFlagsForGodMode(uintptr_t entity, bool setFlag);
	void SetFlagsForAirbreak(uintptr_t entity,bool setFlag, bool isCar);
	void MakeCurrentVehicleGodMode(bool isEnabled);
	void FixCurrentVehicle();
	void VehicleSpeedBoost();
	void VehicleJump();
	void MainHackLoop();
	void MenuDisplay();
};