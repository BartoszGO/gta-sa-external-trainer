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

HackClass::HackClass()
{
	processName = L"gta_sa.exe";
	procId = GetProcessId(processName);
	hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, procId);
	moduleBaseAddr = GetModuleBaseAddress(procId, processName);
	playerBasePtr = moduleBaseAddr + ofOffsets.playerOffset;
	currentCarPtr = moduleBaseAddr + ofOffsets.currentCarOffset;
	playerPosAddr = FindDynamicAddr(hProc, playerBasePtr, ofOffsets.positionOffsets);
	healthAddr = FindDynamicAddr(hProc, playerBasePtr, ofOffsets.healthOffsets);
	armorAddr = FindDynamicAddr(hProc, playerBasePtr, ofOffsets.armorOffsets);
	ReadProcessMemory(hProc, LPCVOID(playerBasePtr), &playerAddr, sizeof(playerAddr), 0);
}

HackClass::~HackClass()
{
	
}

void HackClass::SavePlayerPosition()
{

	uintptr_t currentVehiclePointer;
	uintptr_t currentCarPosAddr;

	currentVehiclePointer = CheckIfInVehicle();
	if (currentVehiclePointer)
	{
		playerSavedPos = GetEntityPosition(currentVehiclePointer);
	}
	else
	{
		ReadProcessMemory(hProc, (LPCVOID)playerPosAddr, &playerSavedPos, sizeof(playerSavedPos), 0);
	}
	swSwitches.posSaved = true;

}


void HackClass::LoadPlayerSavedPosition()
{

	int currentCarPointer;
	currentCarPointer = CheckIfInVehicle();
	uintptr_t currentCarPosAddr;
	if (swSwitches.posSaved)
	{
		if (currentCarPointer)
		{
			currentCarPosAddr = FindDynamicAddr(hProc, currentCarPtr, ofOffsets.positionOffsets);
			WriteProcessMemory(hProc, (LPVOID)currentCarPosAddr, &playerSavedPos, sizeof(playerSavedPos), 0);
		}
		else
		{
			WriteProcessMemory(hProc, (LPVOID)playerPosAddr, &playerSavedPos, sizeof(playerSavedPos), 0);
		}
	}
	swSwitches.loadPos = true;
}


void HackClass::LoadPlayerPosition(vec3 pos)
{

	int currentCarPointer;
	currentCarPointer = CheckIfInVehicle();
	uintptr_t currentCarPosAddr;
	if (currentCarPointer)
	{
		currentCarPosAddr = FindDynamicAddr(hProc, currentCarPtr, ofOffsets.positionOffsets);
		WriteProcessMemory(hProc, (LPVOID)currentCarPosAddr, &pos, sizeof(pos), 0);
	}
	else
	{
		WriteProcessMemory(hProc, (LPVOID)playerPosAddr, &pos, sizeof(pos), 0);
	}

	
}

void HackClass::MovePlayerPosition(vec3 movement)
{
	uintptr_t currentCar = CheckIfInVehicle();
	uintptr_t currentCarPosAddr;
	vec3 currentPosition;
	vec3 currentCarPos;

	if (currentCar)
	{
		currentCarPosAddr = FindDynamicAddr(hProc, currentCarPtr, ofOffsets.positionOffsets);
		ReadProcessMemory(hProc, (LPCVOID)currentCarPosAddr, &currentCarPos, sizeof(currentCarPos), 0);
		currentCarPos.x += movement.x;
		currentCarPos.y += movement.y;
		currentCarPos.z += movement.z;
		WriteProcessMemory(hProc, (LPVOID)currentCarPosAddr, &currentCarPos, sizeof(currentCarPos), 0);
		return;
	}
	ReadProcessMemory(hProc, (LPCVOID)playerPosAddr, &currentPosition, sizeof(currentPosition), 0);
	currentPosition.x += movement.x;
	currentPosition.y += movement.y;
	currentPosition.z += movement.z;
	WriteProcessMemory(hProc, (LPVOID)playerPosAddr, &currentPosition, sizeof(currentPosition), 0);
}




vec3 HackClass::RotatePoint(float cx, float cy, float angle, vec3 p)
{
	float s = sin(angle);
	float c = cos(angle);

	// translate point back to origin:
	p.x -= cx;
	p.y -= cy;

	// rotate point
	float xnew = p.x * c - p.y * s;
	float ynew = p.x * s + p.y * c;

	// translate point back:
	p.x = xnew + cx;
	p.y = ynew + cy;
	return p;
}

void HackClass::SetFlags(uintptr_t entity, bool setFlag, bool isCar)
{
	BYTE noGravityFlag = 16;
	uint8_t noSpeedFlag = 32;
	uint8_t freezeFlag = 2;
	uint8_t gravityFlag = 18;
	uint8_t speedFlag = 2;
	uint8_t noFreezeFlag = 0;

	
	if (setFlag)
	{
		if (!isCar)WriteProcessMemory(hProc, (LPVOID)(entity + 0x40), &noGravityFlag, sizeof(noGravityFlag), 0);
		if (!isCar)WriteProcessMemory(hProc, (LPVOID)(entity + 0x41), &noSpeedFlag, sizeof(noSpeedFlag), 0);
		if(!isCar) WriteProcessMemory(hProc, (LPVOID)(entity + 0x42), &freezeFlag, sizeof(freezeFlag), 0);

	}
	else
	{
		if (!isCar)WriteProcessMemory(hProc, (LPVOID)(entity + 0x40), &gravityFlag, sizeof(gravityFlag), 0);
		if(!isCar)WriteProcessMemory(hProc, (LPVOID)(entity + 0x41), &speedFlag, sizeof(speedFlag), 0);
		if (!isCar)WriteProcessMemory(hProc, (LPVOID)(entity + 0x42), &noFreezeFlag, sizeof(noFreezeFlag), 0);
	}

}


vec3 HackClass::GetEntityPosition(uintptr_t entity)
{
	uintptr_t positionAddr;
	uintptr_t offset;
	vec3 position = { 0.0f,0.0f,0.0f };
	positionAddr = FindDynamicAddr2(hProc, entity, ofOffsets.positionOffsets);
	ReadProcessMemory(hProc, (LPCVOID)positionAddr, &position, sizeof(position), 0);
	return position;

}

void HackClass::SetPlayerPosition(vec3 pos)
{
	WriteProcessMemory(hProc, LPVOID(playerPosAddr), &pos, sizeof(pos), 0);
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

	BYTE noGravityFlag = 16;
	uint8_t noSpeedFlag = 32;
	uint8_t freezeFlag = 2;
	uintptr_t currentCar;


	currentCar = CheckIfInVehicle();
	ReadProcessMemory(hProc, (LPCVOID)(playerAddr + 0x40), &flags, sizeof(flags), 0);
	
	if (flags != 16)
	{
		SetFlags(playerAddr, 1,0);
	}

	if (currentCar)
	{
		playerPosition = GetEntityPosition(currentCar);
		ReadProcessMemory(hProc, (LPCVOID)(currentCar + 0x40), &carFlags, sizeof(carFlags), 0);
		if (carFlags != 16)
		{
			SetFlags(currentCar, 1, 1);
		}
	}

	ReadProcessMemory(hProc, (LPCVOID)(ofOffsets.cameraAim), &cameraAim, sizeof(cameraAim), 0);
	playerPosition = GetEntityPosition(playerAddr);

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

	MovePlayerPosition(movement);

}


void HackClass::AirBreakOff()
{

	uint8_t carFlags;
	uint8_t flags;
	uintptr_t currentCar;

	ReadProcessMemory(hProc, (LPCVOID)(currentCarPtr), &currentCar, sizeof(currentCar), 0);
	ReadProcessMemory(hProc, (LPCVOID)(playerAddr + 0x40), &flags, sizeof(flags), 0);
	ReadProcessMemory(hProc, (LPCVOID)(currentCar + 0x40), &carFlags, sizeof(carFlags), 0);

	if (flags == 16)
	{
		SetFlags(playerAddr, 0,0);
	}

	if (currentCar && carFlags == 16)
	{
		SetFlags(currentCar, 0,1);
	}

}


void HackClass::TeleportToTargetEntity()
{
	uintptr_t targetEntityAddr = FindDynamicAddr(hProc, playerBasePtr, ofOffsets.targetEntityOffsets);
	int targetEntity;
	ReadProcessMemory(hProc, (LPCVOID)targetEntityAddr, &targetEntity, sizeof(targetEntity), 0);
	uintptr_t targetEntityPositionAddr;
	vec3 targetEntityPos;

	if (targetEntity)
	{
		targetEntityPositionAddr = FindDynamicAddr(hProc, targetEntityAddr, ofOffsets.positionOffsets);
		ReadProcessMemory(hProc, (LPCVOID)targetEntityPositionAddr, &targetEntityPos, sizeof(targetEntityPos), 0);
		targetEntityPos.x += 0.5;
		WriteProcessMemory(hProc, (LPVOID)playerPosAddr, &targetEntityPos, sizeof(targetEntityPos), 0);
		swSwitches.teleportToTargetEntity = true;

	}
}

void HackClass::TeleportToBulletLocation()
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
			ReadProcessMemory(hProc, (LPCVOID)playerPosAddr, &currentPos, sizeof(currentPos), 0);

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


void HackClass::KillWithSigth()
{
	uintptr_t targetEntityAddr = FindDynamicAddr(hProc, playerBasePtr, ofOffsets.targetEntityOffsets);
	uintptr_t targetEntityHealthAddr;
	int entityHealth = 0;

	if (targetEntityAddr)
	{
		targetEntityHealthAddr = FindDynamicAddr(hProc, targetEntityAddr, ofOffsets.healthOffsets);
		WriteProcessMemory(hProc, (LPVOID)targetEntityHealthAddr, &entityHealth, sizeof(entityHealth), 0);
	}

}


void HackClass::MakeCurrentVehicleGodMode(bool isEnabled) 
{
	unsigned char carFlags;
	uintptr_t currentVehiclePointer;
	uintptr_t currentCarFlagsAddr;

	currentVehiclePointer = CheckIfInVehicle();
	if (currentVehiclePointer)
	{
		currentCarFlagsAddr = currentVehiclePointer + 0x42;
		isEnabled ? carFlags = 188 : carFlags = 0;
		WriteProcessMemory(hProc, (LPVOID)currentCarFlagsAddr, &carFlags, sizeof(carFlags), 0);
	}
}

void HackClass::FixCurrentVehicle()
{
	float newVehicleHealth = 1000;
	int carTiresOkValue = 0;
	unsigned short bikeTiresOkValue = 0;
	uintptr_t currentVehiclePointer;
	uintptr_t currentVehicleHealthAddr;
	uintptr_t currentCarTiresAddr;
	uintptr_t currentBikeTiresAddr;
	unsigned char bikeTireStatus;
	
	currentVehiclePointer = CheckIfInVehicle();
	if (currentVehiclePointer)
	{
		currentVehicleHealthAddr = currentVehiclePointer + ofOffsets.carHealthOffset;
		currentBikeTiresAddr = currentVehiclePointer + ofOffsets.bikeFrontTireOffset;
		ReadProcessMemory(hProc, (LPCVOID)currentBikeTiresAddr, &bikeTireStatus, sizeof(bikeTireStatus), 0);
		if (bikeTireStatus >= 0 && bikeTireStatus <= 3)
		{
			
			WriteProcessMemory(hProc, (LPVOID)currentBikeTiresAddr, &bikeTiresOkValue, sizeof(bikeTiresOkValue), 0);
		}
		else
		{
			currentCarTiresAddr = currentVehiclePointer + ofOffsets.carLeftFrontTireOffset;
			WriteProcessMemory(hProc, (LPVOID)currentCarTiresAddr, &carTiresOkValue, sizeof(carTiresOkValue), 0);
		}
		
		WriteProcessMemory(hProc, (LPVOID)currentVehicleHealthAddr, &newVehicleHealth, sizeof(newVehicleHealth), 0);
		

	}
}


void HackClass::VehicleSpeedBoost()
{
	uintptr_t currentVehicleAddr = CheckIfInVehicle();
	vec3 speedVector;
	if (currentVehicleAddr)
	{
		ReadProcessMemory(hProc, (LPCVOID)(currentVehicleAddr + ofOffsets.vehicleSpeedVectorOffset), &speedVector, sizeof(speedVector), 0);
		speedVector.x = speedVector.x + (speedVector.x * 0.012);
		speedVector.y = speedVector.y + (speedVector.y * 0.012);
		speedVector.z = speedVector.z + (speedVector.z * 0.012);
		WriteProcessMemory(hProc, (LPVOID)(currentVehicleAddr + ofOffsets.vehicleSpeedVectorOffset), &speedVector, sizeof(speedVector), 0);
	}
	
}


void HackClass::VehicleJump()
{
	uintptr_t currentVehicleAddr = CheckIfInVehicle();
	vec3 speedVector;
	if (currentVehicleAddr)
	{
		ReadProcessMemory(hProc, (LPCVOID)(currentVehicleAddr + ofOffsets.vehicleSpeedVectorOffset), &speedVector, sizeof(speedVector), 0);
		speedVector.z = speedVector.z + 0.023;
		WriteProcessMemory(hProc, (LPVOID)(currentVehicleAddr + ofOffsets.vehicleSpeedVectorOffset), &speedVector, sizeof(speedVector), 0);
	}

}




void HackClass::MenuDisplay()
{
	cls();
	std::string buffer;
	buffer.reserve(1000);
	std::cout << "GTA SA External trainer made by Chemik" << std::endl;
	std::cout << "-----------------------------------------------" << std::endl;
	if (hProc)
	{
		
		addToBuffer(buffer, "[NUMPAD_1] Freeze Health               : ", swSwitches.freezeHealth,0);
		addToBuffer(buffer, "[NUMPAD_2] Freeze Armor                : ", swSwitches.freezeArmor,0);
		addToBuffer(buffer, "[NUMPAD_3] Freeze Ammo                 : ", swSwitches.freezeAmmo,0);
		addToBuffer(buffer, "[NUMPAD_4] Save Player Position        : ", swSwitches.posSaved,0);
		addToBuffer(buffer, "[NUMPAD_5] Load Player Position        : ", swSwitches.loadPos,0);
		addToBuffer(buffer, "[NUMPAD_6] Teleport To Targeted Entity : ", swSwitches.teleportToTargetEntity,0);
		addToBuffer(buffer, "[NUMPAD_7] Kill With Sight             : ", swSwitches.killWithSight,0);
		addToBuffer(buffer, "[NUMPAD_8] Vehicle Godmode             : ", swSwitches.vehicleGodMode,0);
		addToBuffer(buffer, "[NUMPAD_9] Vehicle Repair              : ", swSwitches.vehicleFix,0);
		addToBuffer(buffer, "[  'O'   ] Teleport To Sniper Bullet   : ", swSwitches.teleportToBullet,0);
		addToBuffer(buffer, "[  'X'   ] Vehicle Boost               : ", (bool)GetAsyncKeyState('X'),0);
		addToBuffer(buffer, "[  'B'   ] Vehicle Jump                : ", (bool)GetAsyncKeyState('B'),0);
		addToBuffer(buffer, "[  'L'   ] Air Break                   : ", swSwitches.airBreak, 0);
		addToBuffer(buffer, "-----------------------------------------------\n", 0, 1);
		addToBuffer(buffer, "[NUMPAD_0] Exit Trainer                  ", 0, 1);

		
		


	}
	else
	{
		std::cout << "Can't find GTA San Andreas process" << std::endl;
	}
	
	std::cout << "-----------------------------------------------" << std::endl;
	std::cout << "[NUMPAD_0] Exit Hack" << std::endl;
	


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
			swSwitches.teleportToBullet = !swSwitches.teleportToBullet;
		}

		if (swSwitches.freezeHealth)
		{
			ChangeFloatValue(131.0f,healthAddr);
		}

		if (swSwitches.freezeArmor)
		{
			ChangeFloatValue(131.0f,armorAddr);
		}


		if (swSwitches.killWithSight)
		{
			KillWithSigth();
		}

		if (swSwitches.teleportToBullet)
		{
			TeleportToBulletLocation();
		}

		if (swSwitches.airBreak)
		{
			AirBreak();
		}
		
		MenuDisplay();
		Sleep(20);

	}
}