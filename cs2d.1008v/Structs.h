#pragma once
#include "stdafx.h"

struct enemyEntity
{
	DWORD baseAdress;

	DWORD xPosA;
	DWORD yPosA;

	float xPos = 0.0f;
	float yPos = 0.0f;

	DWORD teamA;
	int team = 0;

	DWORD isLiveA;
	int isLive = 1;

	DWORD isQuitA;
	int isQuit = 0;

	DWORD inMatchA;
	float inMatch = 0.0f;
	float prevInMatch = 0.0f;
	int eqCount = 0;
};

struct player
{
	DWORD baseAdress;

	DWORD xPosA;
	DWORD yPosA;

	float xPos = 0.0f;
	float yPos = 0.0f;

	DWORD teamA;
	int team = 0;

	DWORD isLiveA;
	int isLive = 1;

	DWORD isQuitA;
	int isQuit = 0;

} Player;

struct screen
{
	DWORD mouseX;
	DWORD mouseY;
	DWORD baseAdress;
} Screen;