#include <Windows.h>
#include <iostream>
#include "Hooks.h"
#include <math.h>
#include <vector>

DWORD xposAddy;

DWORD moduleBase;

DWORD inMatchModule;

DWORD screenWidthA;
DWORD screenHeightA;
int screenWidth;
int screenHeight;

struct enemyEntity
{
	DWORD baseAdress;

	DWORD xPosA;
	DWORD yPosA;

	float xPos;
	float yPos;

	DWORD teamA;
	int team;

	DWORD isLiveA;
	int isLive;

	DWORD isQuitA;
	int isQuit;

	DWORD inMatchA;
	float inMatch;
	float prevInMatch;
	int eqCount = 0;
};

struct player
{
	DWORD baseAdress;

	DWORD xPosA;
	DWORD yPosA;

	float xPos;
	float yPos;

	DWORD teamA;
	int team;

	DWORD isLiveA;
	int isLive;

	DWORD isQuitA;
	int isQuit;

} Player;

struct screen
{
	DWORD mouseX;
	DWORD mouseY;
	DWORD baseAdress;
} Screen;

typedef enemyEntity EnemyEntity;

bool isExist(std::vector<EnemyEntity> v, DWORD s);

std::vector<EnemyEntity> enemies;

bool aim = false;
void InitiateHooks()
{
	xposAddy = FindPattern("cs2d.exe", "\x9E\x0F\x97\xC0\x0F\xB6\xC0\x83\xF8\x00\x0F\x85\x00\x00\x00\x00\xD9\x05\x00\x00\x00\x00\xD9\x9F\x00\x00\x00\x00\x8B\x87\x00\x00\x00\x00", "xxxxxxxxxxxx????xx????xx????xx????");
	xposAddy += 0x1C; //5 bytes = jmp instruction + 4 bytes for a 4byte address
	deathJmpBack = xposAddy + 0x6; 
	PlaceJMP((BYTE*)xposAddy, (DWORD)getDeath, 6);

	moduleBase = getModuleBase("cs2d.exe");
}

//This thread contains our hack loop and toggles the hacks based on key input
DWORD WINAPI OverwriteValues()
{
	for (;; Sleep(30))
	{
		if (GetAsyncKeyState(VK_HOME))
		{
			system("cls");
			if (!aim)
				std::cout << "Active!" << std::endl;
			else
				std::cout << "Deactive!" << std::endl;
			aim = !aim;
			Sleep(100);
		}



		inMatchModule = (moduleBase + 0x3BD2A4);

		if (*(int *)inMatchModule == 0) 
		{
			if (enemies.size() > 0)
				enemies.clear();
			continue;
		}

		if (enemyBase == 0)
		{
			continue;
		}

		Player.baseAdress = *(DWORD *)(moduleBase + 0x3BD648);
		Screen.baseAdress = *(DWORD *)(moduleBase + 0x41AC60);

		screenWidthA = (moduleBase + 0x3BDA94);
		screenHeightA = (moduleBase + 0x3BDA98);
		screenWidth = *(int *)screenWidthA;
		screenHeight = *(int *)screenHeightA;
		//std::cout << "width: " << screenWidth << " --- height: " << screenHeight << std::endl;


		Player.teamA = Player.baseAdress + 0x1CC;
		Player.team = *(int*)Player.teamA;
		DWORD enemyTeamA = enemyBase + 0x1CC;
		int enemyTeam = *(int*)enemyTeamA;
		//std::cout << "Enemy team no: " << enemyTeam << "    My Team No: " << Player.team << std::endl;

		if (!isExist(enemies, enemyBase))
		{
			EnemyEntity e;
			e.baseAdress = enemyBase;
			e.teamA = enemyTeamA;
			e.team = *(int*)e.teamA;
			if (enemyTeam != Player.team)
			{
				e.prevInMatch = 1.0f;
				enemies.push_back(e);
				//std::cout << "dusman eklendi!  --  " << std::hex << enemyBase << std::endl;
			}
		}

		if (enemies.size() > 0)
		{
			// update enemy vars
			for (std::vector<int>::size_type i = 0; i != enemies.size(); i++)
			{

				enemies[i].inMatchA = enemies[i].baseAdress + 0x78; //float
				enemies[i].inMatch = *(float*)enemies[i].inMatchA;//float
				//std::cout << "inMatch: " << enemies[i].inMatch << " --- prevMatch " << enemies[i].prevInMatch << std::endl;

				if (enemies[i].eqCount >= 5)
				{
					//std::cout << i << " enemy deleted: not in the game" << std::endl;
					enemies.erase(enemies.begin() + i);
					if (i != enemies.size() - 1)
						i = i - 1;
					continue;
				}

				if (enemies[i].inMatch == enemies[i].prevInMatch)
				{
					enemies[i].eqCount++; 
				}

				if (enemies[i].team == Player.team)
				{
					//std::cout << i << " enemy deleted: change team" << std::endl;
					enemies.erase(enemies.begin() + i);
					if (i != enemies.size() - 1)
						i = i - 1;
					continue;
				}

				enemies[i].prevInMatch = enemies[i].inMatch;

				Player.xPosA = Player.baseAdress + 0x1DC; //float
				Player.yPosA = Player.baseAdress + 0x1E0; //float
				Player.isLiveA = Player.baseAdress + 0x228; //int
				Player.isLive = *(int *)Player.isLiveA; //int

				enemies[i].xPosA = enemies[i].baseAdress + 0x1DC; //float
				enemies[i].yPosA = enemies[i].baseAdress + 0x1E0; //float
				enemies[i].isLiveA = enemies[i].baseAdress + 0x228; //int
				enemies[i].isLive = *(int *)enemies[i].isLiveA; //int
			}
		}

		if (aim && enemies.size() > 0)
		{
			float maxLen = sqrt((screenWidth / 2) ^ 2 + (screenHeight / 2) ^ 2); //max distance 
			int targetIndex = 0;
			float xDistance;
			float yDistance;
			for (std::vector<int>::size_type i = 0; i != enemies.size(); i++)
			{
				if (enemies[i].isLive != 0)
				{
					continue;
				}

				//update mouse coords
				Screen.mouseX = Screen.baseAdress + 0x18; //int
				Screen.mouseY = Screen.baseAdress + 0x1C; //int

				//find enemy and my pos, then calculate distance
				Player.xPos = *(float *)Player.xPosA;
				Player.yPos = *(float *)Player.yPosA;
				enemies[i].xPos = *(float *)enemies[i].xPosA;
				enemies[i].yPos = *(float *)enemies[i].yPosA;

				xDistance = abs(enemies[i].xPos - Player.xPos);
				yDistance = abs(enemies[i].yPos - Player.yPos);
				float len = sqrtf(sqrtf(xDistance) + sqrtf(yDistance));
				if (i == 0)
				{
					maxLen = len;
				}
				else if (len < maxLen)
				{
					maxLen = len;
					targetIndex = i;
				}

				//std::cout << "enemy[" << i << "]: isLive: " << enemies[i].isLive << std::endl;
				//std::cout << "enemy[" << i << "]: " << "x: " << enemies[i].xPos << " --- y: " << enemies[i].yPos << std::endl;
				//std::cout << "player[" << i << "]: " << "x: " << Player.xPos << " --- y: " << Player.yPos << std::endl;
				//std::cout << "distance [" << i << "]: " << "x: " << xDistance << " --- y: " << yDistance << std::endl;
			}

			//std::cout << enemies.size() << std::endl;
			if (abs(enemies[targetIndex].xPos - Player.xPos) <= screenWidth / 2 && abs(enemies[targetIndex].yPos - Player.yPos) <= screenHeight / 2 && Player.isLive == 0 && enemies[targetIndex].isLive == 0)
			{
				*(int *)Screen.mouseX = (screenWidth / 2) + (enemies[targetIndex].xPos - Player.xPos);
				*(int *)Screen.mouseY = (screenHeight / 2) + (enemies[targetIndex].yPos - Player.yPos);
				//std::cout << "width: " << screenWidth / 2 << " --- height: " << screenHeight / 2 << std::endl;
				//std::cout << "distance [" << targetIndex << "]: " << "x: " << (enemies[targetIndex].xPos - Player.xPos) << " --- y: " << (enemies[targetIndex].yPos - Player.yPos) << std::endl;
				//std::cout << "mouseX: " << *(int *)Screen.mouseX << " --- mouseY: " << *(int *)Screen.mouseY << std::endl;
			}
		}
	}
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
	// Perform actions based on the reason for calling.
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		AllocConsole();
		freopen("CONOUT$", "w", stdout);

		//Should do InitiateHooks in the thread not in PROCESS_ATTACH...
		InitiateHooks();
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)OverwriteValues, NULL, NULL, NULL);
		break;
	}
	return TRUE;
}

bool isExist(std::vector<EnemyEntity> v, DWORD value)
{
	for (auto s : v)
	{
		if (s.baseAdress == value)
			return true;
	}
	return false;
}
