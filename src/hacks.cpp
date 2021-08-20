#include<Windows.h>
#include<iostream>
#include "memproc.h"
#include "structs.h"
#include "hacks.h"
#include <string>
#include<time.h>

hackClass::hackClass()
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

hackClass::~hackClass()
{

}

void hackClass::SavePlayerPosition()
{

	int currentCarPointer;
	uintptr_t currentCarPosAddr;

	ReadProcessMemory(hProc, (LPCVOID)0xBA18FC, &currentCarPointer, sizeof(currentCarPointer), 0);
	if (currentCarPointer)
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

void hackClass::LoadPlayerPosition()
{

	int currentCarPointer;
	ReadProcessMemory(hProc, (LPCVOID)0xBA18FC, &currentCarPointer, sizeof(currentCarPointer), 0);
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



void hackClass::TeleportToTargetEntity()
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


void hackClass::ChangeFloatValue(float value)
{

	WriteProcessMemory(hProc, (LPVOID)healthAddr, &value, sizeof(value), 0);
}



void hackClass::FreezeAmmo()
{
	NopEx(hProc, (LPVOID)(moduleBaseAddr + 0x3428AF), 1);
	NopEx(hProc, (LPVOID)(moduleBaseAddr + 0x3428E6), 3);
}


void hackClass::UnfreezeAmmo()
{
	PatchEx(hProc, (LPVOID)(moduleBaseAddr + 0x3428AF), (LPVOID)("\x48"), 1);
	PatchEx(hProc, (LPVOID)(moduleBaseAddr + 0x3428E6), (LPVOID)("\xFF\x4E\x0C"), 3);
}


void hackClass::KillWithSigth()
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



void hackClass::MenuDisplay()
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
	}
	else
	{
		std::cout << "Can't find GTA San Andreas process" << std::endl;
	}
	
	std::cout << "-----------------------------------------------" << std::endl;
	std::cout << "[NUMPAD_0] Exit Hack" << std::endl;
	
	swSwitches.loadPos = false;
	swSwitches.TeleportToTargetEntity = false;
}


void hackClass::MainHackLoop() {
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
		}

		if (GetAsyncKeyState(VK_NUMPAD4))
		{

			SavePlayerPosition();
		}

		if (GetAsyncKeyState(VK_NUMPAD5))
		{

			LoadPlayerPosition();
		}

		if (GetAsyncKeyState(VK_NUMPAD7))
		{
			swSwitches.killWithSight = !swSwitches.killWithSight;
		}


		if (GetAsyncKeyState(VK_NUMPAD6))
		{
			TeleportToTargetEntity();
		}


		if (swSwitches.freezeHealth)
		{
			ChangeFloatValue(131.0f);
		}

		if (swSwitches.freezeArmor)
		{
			ChangeFloatValue(131.0f);
		}

		if (swSwitches.freezeAmmo)
		{
			FreezeAmmo();
		}
		else
		{
			UnfreezeAmmo();
		}

		if (swSwitches.killWithSight)
		{
			KillWithSigth();
		}


		MenuDisplay();
		Sleep(100);

	}
}