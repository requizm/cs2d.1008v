#include "stdafx.h"
#pragma comment(lib,"OpenGL32.lib")
#include <Windows.h>
#include <iostream>
#include "Hooks.h"
#include <math.h>
#include <vector>
#include "Menu.h"
#include "Structs.h"

#define MENU_FONT_HEIGHT 15

DWORD xposAddy;
DWORD moduleBase;
DWORD inMatchModule;
DWORD screenWidthA;
DWORD screenHeightA;
int screenWidth;
int screenHeight;

bool aim = false;
bool esp = false;
bool menuSt = true;


typedef enemyEntity EnemyEntity;
std::vector<EnemyEntity> enemies;

bool isExist(std::vector<EnemyEntity> v, DWORD s);
void Render();

twglSwapBuffers owglSwapBuffers;

void DisableFlashEffect()
{
	DWORD OldProtection = NULL;
	VirtualProtect(reinterpret_cast<void*>(0x005ACA9D), 1, PAGE_EXECUTE_READWRITE, &OldProtection);
	*reinterpret_cast<PBYTE>(0x005ACA9D) = 0xE9;
	VirtualProtect(reinterpret_cast<void*>(0x005ACA9D), 1, OldProtection, &OldProtection);

	OldProtection = NULL;
	VirtualProtect(reinterpret_cast<void*>(0x005ACA9E), 1, PAGE_EXECUTE_READWRITE, &OldProtection);
	*reinterpret_cast<PBYTE>(0x005ACA9E) = 0x6E;
	VirtualProtect(reinterpret_cast<void*>(0x005ACA9E), 1, OldProtection, &OldProtection);

	OldProtection = NULL;
	VirtualProtect(reinterpret_cast<void*>(0x005ACA9F), 1, PAGE_EXECUTE_READWRITE, &OldProtection);
	*reinterpret_cast<PBYTE>(0x005ACA9F) = 0x02;
	VirtualProtect(reinterpret_cast<void*>(0x005ACA9F), 1, OldProtection, &OldProtection);

	OldProtection = NULL;
	VirtualProtect(reinterpret_cast<void*>(0x005ACAA0), 1, PAGE_EXECUTE_READWRITE, &OldProtection);
	*reinterpret_cast<PBYTE>(0x005ACAA0) = 0x00;
	VirtualProtect(reinterpret_cast<void*>(0x005ACAA0), 1, OldProtection, &OldProtection);

	OldProtection = NULL;
	VirtualProtect(reinterpret_cast<void*>(0x005ACAA1), 1, PAGE_EXECUTE_READWRITE, &OldProtection);
	*reinterpret_cast<PBYTE>(0x005ACAA1) = 0x00;
	VirtualProtect(reinterpret_cast<void*>(0x005ACAA1), 1, OldProtection, &OldProtection);

	OldProtection = NULL;
	VirtualProtect(reinterpret_cast<void*>(0x005ACAA2), 1, PAGE_EXECUTE_READWRITE, &OldProtection);
	*reinterpret_cast<PBYTE>(0x005ACAA2) = 0x90;
	VirtualProtect(reinterpret_cast<void*>(0x005ACAA2), 1, OldProtection, &OldProtection);
}

BOOL _stdcall SwapBuffersHook(HDC hDC)
{
	Render();
	return owglSwapBuffers(hDC);
}


void InitiateHooks()
{
	xposAddy = FindPattern("cs2d.exe", "\x9E\x0F\x97\xC0\x0F\xB6\xC0\x83\xF8\x00\x0F\x85\x00\x00\x00\x00\xD9\x05\x00\x00\x00\x00\xD9\x9F\x00\x00\x00\x00\x8B\x87\x00\x00\x00\x00", "xxxxxxxxxxxx????xx????xx????xx????");
	xposAddy += 0x1C; //5 bytes = jmp instruction + 4 bytes for a 4byte address
	xJmpBack = xposAddy + 0x6;
	PlaceJMP((BYTE*)xposAddy, (DWORD)getEnemyBase, 6);
	moduleBase = getModuleBase("cs2d.exe");
	DisableFlashEffect();
}

DWORD WINAPI OverwriteValues()
{
	Hook("wglSwapBuffers", (uintptr_t &)owglSwapBuffers, &SwapBuffersHook);

	for (;; Sleep(50))
	{
		if (GetAsyncKeyState(VK_HOME))
		{
			if (!menuSt)
			{
				//std::cout << "Menu Active!" << std::endl;
			}

			else
			{
				//std::cout << "Menu Deactive!" << std::endl;
			}
			menuSt = !menuSt;
			Sleep(100);
		}

		if (GetAsyncKeyState(VK_F5))
		{
			if (!aim)
			{
				menu.sAimbotStatus = "ON";
				//std::cout << "Aim Active!" << std::endl;
			}

			else
			{
				menu.sAimbotStatus = "OFF";
				//std::cout << "Aim Deactive!" << std::endl;
			}
			aim = !aim;
			Sleep(100);
		}

		if (GetAsyncKeyState(VK_F6))
		{
			if (!esp)
			{
				menu.sESP = "ON";
				//std::cout << "Esp Active!" << std::endl;
			}

			else
			{
				menu.sESP = "OFF";
				//std::cout << "Esp Deactive!" << std::endl;
			}
			esp = !esp;
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


		Player.isLiveA = Player.baseAdress + 0x228; //int
		Player.isLive = *(int *)Player.isLiveA; //int
		DWORD enemyTeamA = enemyBase + 0x1CC;
		int enemyTeam = *(int*)enemyTeamA;
		//std::cout << "Enemy team no: " << enemyTeam << "    My Team No: " << Player.team << std::endl;

		if (!isExist(enemies, enemyBase))
		{
			EnemyEntity e;
			e.baseAdress = enemyBase;
			e.teamA = enemyTeamA;
			e.team = *(int*)e.teamA;
			e.prevInMatch = 1.0f;
			enemies.push_back(e);
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


				enemies[i].prevInMatch = enemies[i].inMatch;

				enemies[i].isLiveA = enemies[i].baseAdress + 0x228; //int
				enemies[i].isLive = *(int *)enemies[i].isLiveA; //int

				Player.xPosA = Player.baseAdress + 0x1DC; //float
				Player.yPosA = Player.baseAdress + 0x1E0; //float

				enemies[i].xPosA = enemies[i].baseAdress + 0x1DC; //float
				enemies[i].yPosA = enemies[i].baseAdress + 0x1E0; //float

				Player.teamA = Player.baseAdress + 0x1CC;
				Player.team = *(int*)Player.teamA;

				//std::cout << "PLAYER TEAM: " << Player.team << std::endl;
				enemies[i].teamA = enemies[i].baseAdress + 0x1CC; //int
				enemies[i].team = *(int *)enemies[i].teamA; //int
			}
		}

		if (enemies.size() > 0)
		{
			float maxLen = 340282346638528859811704183484516925440.0000000000000000f; //max distance 
			int targetIndex = 0;
			for (std::vector<int>::size_type i = 0; i != enemies.size(); i++)
			{
				if (enemies[i].team == Player.team)
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

				float xDistance = abs(enemies[i].xPos - Player.xPos);
				float yDistance = abs(enemies[i].yPos - Player.yPos);
				float len = sqrtf(pow(xDistance, 2) + pow(yDistance, 2));
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

			if (enemies[targetIndex].eqCount <= 5 && aim && Player.isLive == 0 && enemies[targetIndex].isLive == 0 && enemies[targetIndex].team != Player.team)
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
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		//AllocConsole();
		//freopen("CONOUT$", "w", stdout);

		InitiateHooks();
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)OverwriteValues, NULL, NULL, NULL);
		break;
	}
	return TRUE;
}


void Render()
{
	HDC currentHDC = wglGetCurrentDC();

	if (!font.bBuilt || currentHDC != font.hdc)
	{
		font.Build(MENU_FONT_HEIGHT);
	}

	SetupOrtho();

	if (menuSt)
		menu.DrawMenu();

	if (esp && enemies.size() > 0 && Player.isLive == 0 && *(int *)inMatchModule != 0)
	{
		for (std::vector<int>::size_type i = 0; i != enemies.size(); i++)
		{
			if (enemies[i].isLive != 0 || enemies[i].team == Player.team || enemies[i].eqCount >= 5)
				continue;
			float xDistance = enemies[i].xPos - Player.xPos;
			float yDistance = enemies[i].yPos - Player.yPos;

			float enemyX = (screenWidth / 2) + ((screenWidth / 2) * (xDistance)) / 423;
			float enemyY = (screenHeight / 2) + ((screenHeight / 2) * (yDistance)) / 270;

			DrawLine(screenWidth / 2, screenHeight / 2, enemyX, enemyY, rgb::green);
		}
	}
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