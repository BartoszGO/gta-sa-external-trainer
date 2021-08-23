#include<Windows.h>
#include<iostream>
#include "memproc.h"
#include "structs.h"
#include "hacks.h"
#include <string>
#include<time.h>

HackClass::HackClass()
{
	processName = L"gta_sa.exe";
	procId = GetProcessId(processName);
	hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, procId);
	moduleBaseAddr = GetModuleBaseAddress(procId, processName);
	dynamicPtrPlayerBaseAddr = moduleBaseAddr + ofOffsets.playerOffset;
	dynamicPtrCurrentCarBaseAddr = moduleBaseAddr + ofOffsets.currentCarOffset;
	playerPosAddr = FindDynamicAddr(hProc, dynamicPtrPlayerBaseAddr, ofOffsets.positionOffsets);
	weaponStructAddr = FindDynamicAddr(hProc, dynamicPtrPlayerBaseAddr, ofOffsets.weaponStructOffsets);
	healthAddr = FindDynamicAddr(hProc, dynamicPtrPlayerBaseAddr, ofOffsets.healthOffsets);
	armorAddr = FindDynamicAddr(hProc, dynamicPtrPlayerBaseAddr, ofOffsets.armorOffsets);
}

HackClass::~HackClass()
{
	
}

void HackClass::SavePlayerPosition()
{

	int currentVehiclePointer;
	uintptr_t currentCarPosAddr;

	currentVehiclePointer = CheckIfInVehicle();
	if (currentVehiclePointer)
	{
		currentCarPosAddr = FindDynamicAddr(hProc, dynamicPtrCurrentCarBaseAddr, ofOffsets.positionOffsets);
		ReadProcessMemory(hProc, (LPCVOID)currentCarPosAddr, &playerPos, sizeof(playerPos), 0);
	}
	else
	{
		ReadProcessMemory(hProc, (LPCVOID)playerPosAddr, &playerPos, sizeof(playerPos), 0);
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
			currentCarPosAddr = FindDynamicAddr(hProc, dynamicPtrCurrentCarBaseAddr, ofOffsets.positionOffsets);
			WriteProcessMemory(hProc, (LPVOID)currentCarPosAddr, &playerPos, sizeof(playerPos), 0);
		}
		else
		{
			WriteProcessMemory(hProc, (LPVOID)playerPosAddr, &playerPos, sizeof(playerPos), 0);
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
		currentCarPosAddr = FindDynamicAddr(hProc, dynamicPtrCurrentCarBaseAddr, ofOffsets.positionOffsets);
		WriteProcessMemory(hProc, (LPVOID)currentCarPosAddr, &pos, sizeof(pos), 0);
	}
	else
	{
		WriteProcessMemory(hProc, (LPVOID)playerPosAddr, &pos, sizeof(pos), 0);
	}

	
}


void HackClass::TeleportToTargetEntity()
{
	uintptr_t targetEntityAddr = FindDynamicAddr(hProc, dynamicPtrPlayerBaseAddr, ofOffsets.targetEntityOffsets);
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
		swSwitches.TeleportToTargetEntity = true;

	}
}

void HackClass::TeleportToBulletLocation()
{
	ReadProcessMemory(hProc, (LPCVOID)ofOffsets.sniperBulletLocationOffset, &sniperBulletLocation, sizeof(sniperBulletLocation), 0);
	if (sniperBulletLocation.x)
	{
		sniperBulletLocation.z += 2;
		LoadPlayerPosition(sniperBulletLocation);
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
	uintptr_t targetEntityAddr = FindDynamicAddr(hProc, dynamicPtrPlayerBaseAddr, ofOffsets.targetEntityOffsets);
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
	system("CLS");
	std::string activated = "[X]";
	std::string dezactivated = "[ ]";

	std::cout << "GTA SA External trainer made by Chemik" << std::endl;
	std::cout << "-----------------------------------------------" << std::endl;
	if (hProc)
	{
		
		std::cout << "[NUMPAD_1] Freeze Health               : ";
		if (swSwitches.freezeHealth) std::cout << activated << std::endl;
		else std::cout << dezactivated << std::endl;

		std::cout << "[NUMPAD_2] Freeze Armor                : ";
		if (swSwitches.freezeArmor) std::cout << activated << std::endl;
		else std::cout << dezactivated << std::endl;

		std::cout << "[NUMPAD_3] Freeze Ammo                 : ";
		if (swSwitches.freezeAmmo) std::cout << activated << std::endl;
		else std::cout << dezactivated << std::endl;

		std::cout << "[NUMPAD_4] Save Player Position        : ";
		if (swSwitches.posSaved) std::cout << activated << std::endl;
		else std::cout << dezactivated << std::endl;

		std::cout << "[NUMPAD_5] Load Player Position        : ";
		if (swSwitches.loadPos) std::cout << "[Position Loaded!]" << std::endl;
		else std::cout << dezactivated << std::endl;

		std::cout << "[NUMPAD_6] Teleport To Targeted Entity : ";
		if (swSwitches.TeleportToTargetEntity) std::cout << "[Teleported Sucessfully!]" << std::endl;
		else std::cout << dezactivated << std::endl;
		
		std::cout << "[NUMPAD_7] Kill With Sight             : ";
		if (swSwitches.killWithSight) std::cout << activated << std::endl;
		else std::cout << dezactivated << std::endl;
		
		std::cout << "[NUMPAD_8] Vehicle Godmode             : ";
		if (swSwitches.vehicleGodMode) std::cout << activated << std::endl;
		else std::cout << dezactivated << std::endl;

		std::cout << "[NUMPAD_9] Vehicle Repair              : ";
		if (swSwitches.vehicleFix) std::cout << activated << std::endl;
		else std::cout << dezactivated << std::endl;

	}
	else
	{
		std::cout << "Can't find GTA San Andreas process" << std::endl;
	}
	
	std::cout << "-----------------------------------------------" << std::endl;
	std::cout << "[NUMPAD_0] Exit Hack" << std::endl;
	
	swSwitches.loadPos = false;
	swSwitches.TeleportToTargetEntity = false;
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

		if (GetAsyncKeyState(VK_NUMPAD8))
		{
			swSwitches.vehicleGodMode = !swSwitches.vehicleGodMode;
			MakeCurrentVehicleGodMode(swSwitches.vehicleGodMode);
		}

		if (GetAsyncKeyState(VK_NUMPAD9))
		{
			swSwitches.vehicleFix = !swSwitches.vehicleFix;
			FixCurrentVehicle();
		}

		if (GetAsyncKeyState('X'))
		{
			VehicleSpeedBoost();
		}

		if (GetAsyncKeyState('B'))
		{
			VehicleJump();
		}

		if (GetAsyncKeyState('O'))
		{
			TeleportToBulletLocation();
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

		


		//MenuDisplay();
		Sleep(10);

	}
}