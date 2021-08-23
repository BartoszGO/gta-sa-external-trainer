#pragma once
#include<vector>

struct offsets
{
	unsigned int playerOffset = 0x76F5F0;
	unsigned int currentCarOffset = 0x7A18FC;
	unsigned int carHealthOffset =  0x4C0;
	unsigned int carLeftFrontTireOffset = 0x5A5;
	unsigned int carLeftRearTireOffset = 0x5A6;
	unsigned int carRightFrontTireOffset = 0x5A7;
	unsigned int carRightRearTireOffset = 0x5A8;
	unsigned int bikeFrontTireOffset = 0x65C;
	unsigned int bikeRearTireOffset = 0x65D;
	unsigned int vehicleSpeedVectorOffset = 0x44;
	unsigned int sniperBulletLocationOffset = 0xC88750;
	std::vector<unsigned int> healthOffsets = { 0x540 };
	std::vector<unsigned int> weaponStructOffsets = { 0x5A0 };
	std::vector<unsigned int> targetEntityOffsets = { 0x79C };
	std::vector<unsigned int> armorOffsets = { 0x548 };
	std::vector<unsigned int> carColor1Offsets = { 0x434 };
	std::vector<unsigned int> carColor2Offsets = { 0x436 };
	std::vector<unsigned int> positionOffsets = { 0x14, 0x30 };
};


struct vec3
{
	float x, y, z;
};


struct switches
{
	bool freezeHealth = false;
	bool freezeAmmo = false;
	bool freezeArmor = false;
	bool killWithSight = false;
	bool savePos = false;
	bool loadPos = false;
	bool posSaved = false;
	bool inCar = false;
	bool TeleportToTargetEntity = false;
	bool vehicleGodMode = false;
	bool vehicleFix = false;
};


