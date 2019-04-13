#include "stdafx.h"
#include "Functions.h"

DWORD xJmpBack = 0;
DWORD enemyBase = 0x0;

__declspec(naked) void getEnemyBase()
{
	__asm
	{
		MOV EAX, [EDI + 0x1EC];
		MOV enemyBase, EDI;
		jmp[xJmpBack];
	}
}