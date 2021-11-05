#define _USE_MATH_DEFINES
#include<Windows.h>
#include<iostream>
#include "memproc.h"
#include "structs.h"
#include "hacks.h"
#include <string>
#include<time.h>
#include<math.h>
#include<cmath>
//#include<entityclass.h>
#include"displayutilities.h"
#include"pointutilities.h"

HackClass::HackClass()
{
	processName = L"gta_sa.exe";
	Initialize();
}

HackClass::~HackClass()
{
	
}

void HackClass::Initialize()
{
	float temp;
	procId = GetProcessId(processName);
	while (!procId)
	{
		cls(false);
		std::cout << "Can't find GTA San Andreas process, please open gta_sa.exe..." << std::endl;
		std::cout << "-----------------------------------------------" << std::endl;
		Sleep(5000);
		procId = GetProcessId(processName);
		
	}

	cls(true);

	
	hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, procId);
	while (!hProc)
	{
		cls(false);
		std::cout << "Waiting for the process to open..." << std::endl;
		std::cout << "-----------------------------------------------" << std::endl;
		Sleep(20);
		hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, procId);
	}

	moduleBaseAddr = GetModuleBaseAddress(procId, processName);
	playerPtrAddr = moduleBaseAddr + ofOffsets.playerOffset;
	
	ReadProcessMemory(hProc, LPCVOID(playerPtrAddr), &playerPtr, sizeof(playerPtr), 0);
	
	while (!playerPtr)
	{
		
		cls(false);
		std::cout << playerPtr << std::endl;
		std::cout << "Waiting for the game to load..." << std::endl;
		std::cout << "-----------------------------------------------" << std::endl;
		ReadProcessMemory(hProc, LPCVOID(playerPtrAddr), &playerPtr, sizeof(playerPtr), 0);
		Sleep(20);
	}
	
	cls(true);

	currentCarPtrAddr = moduleBaseAddr + ofOffsets.currentCarOffset;
	playerPosPtr = FindDynamicAddr(hProc, playerPtrAddr, ofOffsets.positionOffsets);
	healthPtr = FindDynamicAddr(hProc, playerPtrAddr, ofOffsets.healthOffsets);
	armorPtr = FindDynamicAddr(hProc, playerPtrAddr, ofOffsets.armorOffsets);
	
	
	
}

void HackClass::SavePlayerPosition()
{

	uintptr_t currentVehiclePtr;
	uintptr_t currentCarPosPtr;

	currentVehiclePtr = CheckIfInVehicle();
	if (currentVehiclePtr)
	{
		playerSavedPos = GetEntityPosition(currentVehiclePtr);
	}
	else
	{
		ReadProcessMemory(hProc, (LPCVOID)playerPosPtr, &playerSavedPos, sizeof(playerSavedPos), 0);
	}
	swSwitches.posSaved = true;

}


void HackClass::LoadPlayerSavedPosition()
{

	uintptr_t currentCarPointer;
	uintptr_t currentCarPosPtr;
	currentCarPointer = CheckIfInVehicle();
	
	if (swSwitches.posSaved)
	{
		if (currentCarPointer)
		{
			currentCarPosPtr = FindDynamicAddr2(hProc, currentCarPointer, ofOffsets.positionOffsets);
			WriteProcessMemory(hProc, (LPVOID)currentCarPosPtr, &playerSavedPos, sizeof(playerSavedPos), 0);
		}
		else
		{
			WriteProcessMemory(hProc, (LPVOID)playerPosPtr, &playerSavedPos, sizeof(playerSavedPos), 0);
		}
	}
	swSwitches.loadPos = true;
}


void HackClass::LoadPlayerPosition(vec3 pos)
{

	uintptr_t currentCarPointer;
	currentCarPointer = CheckIfInVehicle();
	uintptr_t currentCarPosAddr;
	if (currentCarPointer)
	{
		currentCarPosAddr = FindDynamicAddr(hProc, currentCarPtrAddr, ofOffsets.positionOffsets);
		WriteProcessMemory(hProc, (LPVOID)currentCarPosAddr, &pos, sizeof(pos), 0);
	}
	else
	{
		WriteProcessMemory(hProc, (LPVOID)playerPosPtr, &pos, sizeof(pos), 0);
	}

	
}

void HackClass::MovePlayerPosition(vec3 movement)
{
	uintptr_t currentCarPtr = CheckIfInVehicle();
	uintptr_t currentCarPosPtr;
	vec3 currentPosition;
	vec3 currentCarPos;

	if (currentCarPtr)
	{
		currentCarPosPtr = FindDynamicAddr(hProc, currentCarPtrAddr, ofOffsets.positionOffsets);
		ReadProcessMemory(hProc, (LPCVOID)currentCarPosPtr, &currentCarPos, sizeof(currentCarPos), 0);
		currentCarPos.x += movement.x;
		currentCarPos.y += movement.y;
		currentCarPos.z += movement.z;
		WriteProcessMemory(hProc, (LPVOID)currentCarPosPtr, &currentCarPos, sizeof(currentCarPos), 0);
		return;
	}
	ReadProcessMemory(hProc, (LPCVOID)playerPosPtr, &currentPosition, sizeof(currentPosition), 0);
	currentPosition.x += movement.x;
	currentPosition.y += movement.y;
	currentPosition.z += movement.z;
	WriteProcessMemory(hProc, (LPVOID)playerPosPtr, &currentPosition, sizeof(currentPosition), 0);
}







void HackClass::SetFlagsForGodMode(uintptr_t entity, bool setFlag)
{
	uint8_t tmpFlag;
	if (setFlag)
	{
		ReadProcessMemory(hProc, (LPCVOID)(entity + 0x42), &tmpFlag, sizeof(tmpFlag), 0);
		tmpFlag += flags.godModeFlag;
		WriteProcessMemory(hProc, (LPVOID)(entity + 0x42), &tmpFlag, sizeof(tmpFlag), 0);
		
		ReadProcessMemory(hProc, (LPVOID)(entity + 0x470), &tmpFlag, sizeof(tmpFlag), 0);
		tmpFlag += flags.headImmunity;
		WriteProcessMemory(hProc, (LPVOID)(entity + 0x470), &tmpFlag, sizeof(tmpFlag), 0);
		swSwitches.godModeFlagsSet = true;

	}
	else
	{
		ReadProcessMemory(hProc, (LPCVOID)(entity + 0x42), &tmpFlag, sizeof(tmpFlag), 0);
		tmpFlag -= flags.godModeFlag;
		WriteProcessMemory(hProc, (LPVOID)(entity + 0x42), &tmpFlag, sizeof(tmpFlag), 0);

		ReadProcessMemory(hProc, (LPVOID)(entity + 0x470), &tmpFlag, sizeof(tmpFlag), 0);
		tmpFlag -= flags.headImmunity;
		WriteProcessMemory(hProc, (LPVOID)(entity + 0x470), &tmpFlag, sizeof(tmpFlag), 0);
		swSwitches.godModeFlagsSet = true;
		swSwitches.godModeFlagsSet = false;
	}

}

void HackClass::SetFlagsForAirbreak(uintptr_t entity, bool setFlag, bool isCar)
{
	
	uint8_t tmpFlag;
	if (setFlag)
	{
		if (isCar)
		{
			ReadProcessMemory(hProc, (LPCVOID)(entity + 0x40), &tmpFlag, sizeof(tmpFlag), 0);
			tmpFlag += flags.noGravityFlagCar;
			WriteProcessMemory(hProc, (LPVOID)(entity + 0x40), &tmpFlag, sizeof(tmpFlag), 0);
		}
		else
		{
			ReadProcessMemory(hProc, (LPCVOID)(entity + 0x40), &tmpFlag, sizeof(tmpFlag), 0);
			tmpFlag -= flags.noGravityFlag;
			WriteProcessMemory(hProc, (LPVOID)(entity + 0x40), &tmpFlag, sizeof(tmpFlag), 0);

			WriteProcessMemory(hProc, (LPVOID)(entity + 0x46c), &flags.playerFlag, sizeof(flags.playerFlag), 0);

			
		}

		ReadProcessMemory(hProc, (LPCVOID)(entity + 0x41), &tmpFlag, sizeof(tmpFlag), 0);
		tmpFlag += flags.noSpeedFlag;
		WriteProcessMemory(hProc, (LPVOID)(entity + 0x41), &tmpFlag, sizeof(tmpFlag), 0);

		ReadProcessMemory(hProc, (LPCVOID)(entity + 0x42), &tmpFlag, sizeof(tmpFlag), 0);
		tmpFlag += flags.freezeFlag;
		WriteProcessMemory(hProc, (LPVOID)(entity + 0x42), &tmpFlag, sizeof(tmpFlag), 0);


		
	}
	else
	{
		if (isCar)
		{
			ReadProcessMemory(hProc, (LPCVOID)(entity + 0x40), &tmpFlag, sizeof(tmpFlag), 0);
			tmpFlag -= flags.noGravityFlagCar;
			WriteProcessMemory(hProc, (LPVOID)(entity + 0x40), &tmpFlag, sizeof(tmpFlag), 0);
		}
		else
		{
			ReadProcessMemory(hProc, (LPCVOID)(entity + 0x40), &tmpFlag, sizeof(tmpFlag), 0);
			tmpFlag += flags.noGravityFlag;
			WriteProcessMemory(hProc, (LPVOID)(entity + 0x40), &tmpFlag, sizeof(tmpFlag), 0);

			WriteProcessMemory(hProc, (LPVOID)(entity + 0x46c), &flags.noPlayerFlag, sizeof(flags.noPlayerFlag), 0);


		}

		ReadProcessMemory(hProc, (LPCVOID)(entity + 0x41), &tmpFlag, sizeof(tmpFlag), 0);
		tmpFlag -= flags.noSpeedFlag;
		WriteProcessMemory(hProc, (LPVOID)(entity + 0x41), &tmpFlag, sizeof(tmpFlag), 0);

		ReadProcessMemory(hProc, (LPCVOID)(entity + 0x42), &tmpFlag, sizeof(tmpFlag), 0);
		tmpFlag -= flags.freezeFlag;
		WriteProcessMemory(hProc, (LPVOID)(entity + 0x42), &tmpFlag, sizeof(tmpFlag), 0);
	
	}

}


vec3 HackClass::GetEntityPosition(uintptr_t entity)
{
	uintptr_t positionPtr;
	uintptr_t offset;
	vec3 position = { 0.0f,0.0f,0.0f };
	positionPtr = FindDynamicAddr2(hProc, entity, ofOffsets.positionOffsets);
	ReadProcessMemory(hProc, (LPCVOID)positionPtr, &position, sizeof(position), 0);
	return position;

}

void HackClass::AirBreak()
{
	uint8_t carFlags;
	uint8_t flags;
	vec3 cameraAim;
	vec3 cameraPos;
	vec3 newPos;
	vec3 playerPosition;

	vec3 movement = { 0.0f,0.0f,0.0f };
	static float boost = 1;

	BYTE noGravityFlag = 16;
	uint8_t noSpeedFlag = 32;
	uint8_t freezeFlag = 2;
	uintptr_t currentCarPtr;


	currentCarPtr = CheckIfInVehicle();
	ReadProcessMemory(hProc, (LPCVOID)(playerPtr + 0x40), &flags, sizeof(flags), 0);
	
	if (flags != 16)
	{
		SetFlagsForAirbreak(playerPtr, 1,0);
	}

	if (currentCarPtr)
	{
		playerPosition = GetEntityPosition(currentCarPtr);
		ReadProcessMemory(hProc, (LPCVOID)(currentCarPtr + 0x40), &carFlags, sizeof(carFlags), 0);
		if (carFlags != 16)
		{
			SetFlagsForAirbreak(currentCarPtr, 1, 1);
		}
	}

	ReadProcessMemory(hProc, (LPCVOID)(ofOffsets.cameraAim), &cameraAim, sizeof(cameraAim), 0);
	playerPosition = GetEntityPosition(playerPtr);

	cameraPos.x = playerPosition.x - cameraAim.x;
	cameraPos.y = playerPosition.y - cameraAim.y;
	cameraPos.z = playerPosition.x - cameraAim.z;

	if (GetAsyncKeyState('W'))
	{

		movement.x += cameraAim.x;
		movement.y += cameraAim.y;
		movement.z += cameraAim.z;
		
	}

	if (GetAsyncKeyState('S'))
	{
		movement.x -= cameraAim.x;
		movement.y -= cameraAim.y;
		movement.z -= cameraAim.z;

	}

	if (GetAsyncKeyState('A'))
	{
		newPos = RotatePoint(playerPosition.x, playerPosition.y, M_PI*3/2 , cameraPos);
		newPos.z = playerPosition.z;


		movement.x += newPos.x - playerPosition.x;
		movement.y += newPos.y - playerPosition.y;
		movement.z += newPos.z - playerPosition.z;
		
	}

	if (GetAsyncKeyState('D'))
	{

		newPos = RotatePoint(playerPosition.x, playerPosition.y, M_PI / 2, cameraPos);
		newPos.z = playerPosition.z;


		movement.x += newPos.x - playerPosition.x;
		movement.y += newPos.y - playerPosition.y;
		movement.z += newPos.z - playerPosition.z;

	}


	if (GetAsyncKeyState(VK_SPACE))
	{
	
		movement.z += 0.5f;
	}


	if (GetAsyncKeyState(VK_LCONTROL))
	{
		movement.z -= 0.5f;
	}


	if (GetAsyncKeyState(VK_ADD) & 0x01)
	{
		if (boost < 0.5f )
		{
			boost += 0.05f;
		}
		else
		{
			boost += 0.5f;
		}
		

	}

	if (GetAsyncKeyState(VK_SUBTRACT) & 0x01)
	{
		
		if(boost>0.5f) boost -= 0.5f;

		if (boost <=  0.5f && boost >= 0.05f)
		{
			boost -= 0.05f;
		}


	}
	
	movement.x *= boost;
	movement.y *= boost;
	movement.z *= boost;
	
	MovePlayerPosition(movement);

}


void HackClass::AirBreakOff()
{

	uint8_t carFlags;
	uint8_t flags;
	uintptr_t currentCarPtr;

	ReadProcessMemory(hProc, (LPCVOID)(currentCarPtrAddr), &currentCarPtr, sizeof(currentCarPtr), 0);
	ReadProcessMemory(hProc, (LPCVOID)(playerPtr + 0x40), &flags, sizeof(flags), 0);
	ReadProcessMemory(hProc, (LPCVOID)(currentCarPtr + 0x40), &carFlags, sizeof(carFlags), 0);

	
	if (currentCarPtr)
	{
		SetFlagsForAirbreak(currentCarPtr, 0,1);
	}
	
	if(flags == 16)
	{
		SetFlagsForAirbreak(playerPtr, 0, 0);
	}

}


void HackClass::TeleportToTargetEntity()
{
	uintptr_t targetEntityPtr = FindDynamicAddr(hProc, playerPtrAddr, ofOffsets.targetEntityOffsets);
	uintptr_t targetEntity;
	ReadProcessMemory(hProc, (LPCVOID)targetEntityPtr, &targetEntity, sizeof(targetEntity), 0);
	uintptr_t targetEntityPositionPtr;
	vec3 targetEntityPos;

	if (targetEntity)
	{
		targetEntityPositionPtr = FindDynamicAddr(hProc, targetEntityPtr, ofOffsets.positionOffsets);
		ReadProcessMemory(hProc, (LPCVOID)targetEntityPositionPtr, &targetEntityPos, sizeof(targetEntityPos), 0);
		targetEntityPos.x += 0.5;
		WriteProcessMemory(hProc, (LPVOID)playerPosPtr, &targetEntityPos, sizeof(targetEntityPos), 0);
		swSwitches.teleportToTargetEntity = true;

	}
}

void HackClass::TeleportToSniperBulletLocation()
{

	vec3 currentPos;
	
	//variables for fixing
	bool isBullet;
	vec3 vec;
	float playerBulletDistance;
	float fixedDistance = 10.5;

	ReadProcessMemory(hProc, (LPCVOID)ofOffsets.sniperBulletExistsOffset, &isBullet, sizeof(isBullet), 0);
	if (isBullet)
	{
		while (isBullet)
		{
			ReadProcessMemory(hProc, (LPCVOID)ofOffsets.sniperBulletExistsOffset, &isBullet, sizeof(isBullet), 0);
			Sleep(40);
		}

		ReadProcessMemory(hProc, (LPCVOID)ofOffsets.sniperBulletLocationOffset, &sniperBulletPos, sizeof(sniperBulletPos), 0);
		if (sniperBulletPos.x)
		{
			ReadProcessMemory(hProc, (LPCVOID)playerPosPtr, &currentPos, sizeof(currentPos), 0);

			//fix sniper bullet location with trigonometry
			vec.x = sniperBulletPos.x - currentPos.x;
			vec.y = sniperBulletPos.y - currentPos.y;
			vec.z = sniperBulletPos.z - currentPos.z;

			playerBulletDistance = std::sqrt(std::pow((vec.x), 2) + std::pow(vec.y, 2));
			vec.x = (vec.x * (playerBulletDistance - fixedDistance)) / playerBulletDistance;
			vec.y = (vec.y * (playerBulletDistance - fixedDistance)) / playerBulletDistance;
			vec.z = (vec.z * (playerBulletDistance - fixedDistance)) / playerBulletDistance;

			vec.x += currentPos.x;
			vec.y += currentPos.y;
			vec.z += currentPos.z;

			sniperBulletPos.x = vec.x;
			sniperBulletPos.y = vec.y;
			sniperBulletPos.z = vec.z + 0.5;

			LoadPlayerPosition(sniperBulletPos);
		}
	}
}





uintptr_t HackClass::CheckIfInVehicle()
{
	uintptr_t currentVehiclePointer;
	ReadProcessMemory(hProc, (LPCVOID)0xBA18FC, &currentVehiclePointer, sizeof(currentVehiclePointer), 0);
	return currentVehiclePointer;
}


void HackClass::ChangeFloatValue(float value, uintptr_t addr)
{

	WriteProcessMemory(hProc, (LPVOID)addr, &value, sizeof(value), 0);
}

void HackClass::FreezeHealth()
{
	if(!swSwitches.godModeFlagsSet)
	{
		SetFlagsForGodMode(playerPtr, 1);
	}
	
	ChangeFloatValue(131.0f, healthPtr);
}


void HackClass::FreezeAmmo()
{

	NopEx(hProc, (LPVOID)(moduleBaseAddr + 0x3428AF), 1);
	NopEx(hProc, (LPVOID)(moduleBaseAddr + 0x3428E6), 3);
}


void HackClass::UnfreezeAmmo()
{
	PatchEx(hProc, (LPVOID)(moduleBaseAddr + 0x3428AF), (LPVOID)("\x48"), 1);
	PatchEx(hProc, (LPVOID)(moduleBaseAddr + 0x3428E6), (LPVOID)("\xFF\x4E\x0C"), 3);
}


void HackClass::KillWithSight()
{
	uintptr_t targetEntityPtr = FindDynamicAddr(hProc, playerPtrAddr, ofOffsets.targetEntityOffsets);
	uintptr_t targetEntityHealthPtr;
	int entityHealth = 0;

	if (targetEntityPtr)
	{
		targetEntityHealthPtr = FindDynamicAddr(hProc, targetEntityPtr, ofOffsets.healthOffsets);
		WriteProcessMemory(hProc, (LPVOID)targetEntityHealthPtr, &entityHealth, sizeof(entityHealth), 0);
	}

}


void HackClass::MakeCurrentVehicleGodMode(bool isEnabled) 
{
	unsigned char carFlags;
	uintptr_t currentVehiclePtr;
	uintptr_t currentVehicleFlagsPtr;

	currentVehiclePtr = CheckIfInVehicle();
	if (currentVehiclePtr)
	{
		currentVehicleFlagsPtr = currentVehiclePtr + 0x42;
		isEnabled ? carFlags = 188 : carFlags = 0;
		WriteProcessMemory(hProc, (LPVOID)currentVehicleFlagsPtr, &carFlags, sizeof(carFlags), 0);
	}
}

void HackClass::FixCurrentVehicle()
{
	float newVehicleHealth = 1000;
	int carTiresOkValue = 0;
	unsigned short bikeTiresOkValue = 0;
	uintptr_t currentVehiclePtr;
	uintptr_t currentVehicleHealthPtr;
	uintptr_t currentCarTiresPtr;
	uintptr_t currentBikeTiresPtr;
	unsigned char bikeTireStatus;
	
	currentVehiclePtr = CheckIfInVehicle();
	if (currentVehiclePtr)
	{
		currentVehicleHealthPtr = currentVehiclePtr + ofOffsets.carHealthOffset;
		currentBikeTiresPtr = currentVehiclePtr + ofOffsets.bikeFrontTireOffset;
		ReadProcessMemory(hProc, (LPCVOID)currentBikeTiresPtr, &bikeTireStatus, sizeof(bikeTireStatus), 0);
		if (bikeTireStatus >= 0 && bikeTireStatus <= 3)
		{
			
			WriteProcessMemory(hProc, (LPVOID)currentBikeTiresPtr, &bikeTiresOkValue, sizeof(bikeTiresOkValue), 0);
		}
		else
		{
			currentCarTiresPtr = currentVehiclePtr + ofOffsets.carLeftFrontTireOffset;
			WriteProcessMemory(hProc, (LPVOID)currentCarTiresPtr, &carTiresOkValue, sizeof(carTiresOkValue), 0);
		}
		
		WriteProcessMemory(hProc, (LPVOID)currentVehicleHealthPtr, &newVehicleHealth, sizeof(newVehicleHealth), 0);
		

	}
}


void HackClass::VehicleSpeedBoost()
{
	uintptr_t currentVehiclePtr = CheckIfInVehicle();
	vec3 speedVector;
	if (currentVehiclePtr)
	{
		ReadProcessMemory(hProc, (LPCVOID)(currentVehiclePtr + ofOffsets.vehicleSpeedVectorOffset), &speedVector, sizeof(speedVector), 0);
		speedVector.x = speedVector.x + (speedVector.x * 0.012);
		speedVector.y = speedVector.y + (speedVector.y * 0.012);
		speedVector.z = speedVector.z + (speedVector.z * 0.012);
		WriteProcessMemory(hProc, (LPVOID)(currentVehiclePtr + ofOffsets.vehicleSpeedVectorOffset), &speedVector, sizeof(speedVector), 0);
	}
	
}


void HackClass::VehicleJump()
{
	uintptr_t currentVehiclePtr = CheckIfInVehicle();
	vec3 speedVector;
	if (currentVehiclePtr)
	{
		ReadProcessMemory(hProc, (LPCVOID)(currentVehiclePtr + ofOffsets.vehicleSpeedVectorOffset), &speedVector, sizeof(speedVector), 0);
		speedVector.z = speedVector.z + 0.023;
		WriteProcessMemory(hProc, (LPVOID)(currentVehiclePtr + ofOffsets.vehicleSpeedVectorOffset), &speedVector, sizeof(speedVector), 0);
	}

}




void HackClass::MenuDisplay()
{
	cls(false);
	std::string buffer;
	buffer.reserve(1000);
	std::cout << "GTA SA External trainer made by Chemik" << std::endl;
	std::cout << "-----------------------------------------------" << std::endl;
		
		addToBuffer(buffer, "[NUMPAD_1] Freeze Health               : ", swSwitches.freezeHealth,0);
		addToBuffer(buffer, "[NUMPAD_2] Freeze Armor                : ", swSwitches.freezeArmor,0);
		addToBuffer(buffer, "[NUMPAD_3] Freeze Ammo                 : ", swSwitches.freezeAmmo,0);
		addToBuffer(buffer, "[NUMPAD_4] Save Player Position        : ", swSwitches.posSaved,0);
		addToBuffer(buffer, "[NUMPAD_5] Load Player Position        : ", swSwitches.loadPos,0);
		addToBuffer(buffer, "[NUMPAD_6] Teleport To Targeted Entity : ", swSwitches.teleportToTargetEntity,0);
		addToBuffer(buffer, "[NUMPAD_7] Kill With Sight             : ", swSwitches.killWithSight,0);
		addToBuffer(buffer, "[NUMPAD_8] Vehicle Godmode             : ", swSwitches.vehicleGodMode,0);
		addToBuffer(buffer, "[NUMPAD_9] Vehicle Repair              : ", swSwitches.vehicleFix,0);
		addToBuffer(buffer, "[  'O'   ] Teleport To Sniper Bullet   : ", swSwitches.teleportToSniperBullet,0);
		addToBuffer(buffer, "[  'X'   ] Vehicle Boost               : ", (bool)GetAsyncKeyState('X'),0);
		addToBuffer(buffer, "[  'B'   ] Vehicle Jump                : ", (bool)GetAsyncKeyState('B'),0);
		addToBuffer(buffer, "[  'L'   ] Air Break                   : ", swSwitches.airBreak, 0);
		addToBuffer(buffer, "-----------------------------------------------\n", 0, 1);
		addToBuffer(buffer, "[NUMPAD_0] Exit Trainer                  ", 0, 1);

		
	
	std::cout << buffer << std::endl;
	swSwitches.loadPos = false;
	swSwitches.teleportToTargetEntity = false;
	swSwitches.vehicleFix = false;
}


void HackClass::MainHackLoop() {
	
	bool running = true;
	while (running)
	{
		
		if (GetAsyncKeyState(VK_NUMPAD0))
		{
			running = false;
		}

		if (GetAsyncKeyState(VK_NUMPAD1) & 0x01)
		{
			swSwitches.freezeHealth = !swSwitches.freezeHealth;
			if (!swSwitches.freezeHealth)
			{
				SetFlagsForGodMode(playerPtr, 0);
			}
		}


		if (GetAsyncKeyState(VK_NUMPAD2) & 0x01)
		{
			swSwitches.freezeArmor = !swSwitches.freezeArmor;
		}


		if (GetAsyncKeyState(VK_NUMPAD3) & 0x01)
		{
			swSwitches.freezeAmmo = !swSwitches.freezeAmmo;
			swSwitches.freezeAmmo ? FreezeAmmo() : UnfreezeAmmo();
		}

		if (GetAsyncKeyState(VK_NUMPAD4))
		{

			SavePlayerPosition();
		}

		if (GetAsyncKeyState(VK_NUMPAD5))
		{

			LoadPlayerSavedPosition();
		}

		if (GetAsyncKeyState(VK_NUMPAD6))
		{
			TeleportToTargetEntity();
		}

		if (GetAsyncKeyState(VK_NUMPAD7))
		{
			swSwitches.killWithSight = !swSwitches.killWithSight;
		}

		if (GetAsyncKeyState(VK_NUMPAD8) & 0x01)
		{
			if (CheckIfInVehicle())
			{
				swSwitches.vehicleGodMode = !swSwitches.vehicleGodMode;
				MakeCurrentVehicleGodMode(swSwitches.vehicleGodMode);
			}
		}

		if (GetAsyncKeyState(VK_NUMPAD9))
		{
			if (CheckIfInVehicle())
			{
				swSwitches.vehicleFix = !swSwitches.vehicleFix;
				FixCurrentVehicle();
			}
		}

		if (GetAsyncKeyState('X'))
		{
			VehicleSpeedBoost();
		}

		if (GetAsyncKeyState('B'))
		{
			VehicleJump();
		}

		if (GetAsyncKeyState('L') & 0x01)
		{
			swSwitches.airBreak = !swSwitches.airBreak;
			if (!swSwitches.airBreak)
			{
				AirBreakOff();
			}
		}

		
		if (GetAsyncKeyState('O') & 0x01)
		{
			swSwitches.teleportToSniperBullet = !swSwitches.teleportToSniperBullet;
		}




		if (swSwitches.freezeHealth)
		{
			FreezeHealth();
		}

		if (swSwitches.freezeArmor)
		{
			ChangeFloatValue(131.0f,armorPtr);
		}


		if (swSwitches.killWithSight)
		{
			KillWithSight();
		}

		if (swSwitches.teleportToSniperBullet)
		{
			TeleportToSniperBulletLocation();
		}


	
		if (swSwitches.airBreak)
		{
			AirBreak();
		}
		
		MenuDisplay();
		Sleep(2);

	}
}