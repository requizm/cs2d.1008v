#include "Functions.h"

//Store where we jump back to after our hook
DWORD AmmoJmpBack = 0;

//These are our functions which we direct code execution to:

//increments ammo instead of decrementing
__declspec(naked) void InfiniteAmmo()
{
	__asm
	{
		INC[ESI];				//increment the variable at the address pointed to by ESI
		push edi;				//push edi on the stack, so you don't lose it's value in the next instruction
		mov edi, [esp + 0x14];	//this moves the local stack variable at 0x14 into edi
								//you must use 0x to make it hex
		jmp[AmmoJmpBack];		//Jump back to the games original code
	}
}

DWORD FlyingJmpBack = 0;
//stores the register's address
DWORD YaxisRegister = 0x0;
DWORD YAxisPtr;

//Grab the address of our X coordinate out of the register
__declspec(naked) void GetPlayerYaxis()
{
	//ESI = address of player object, 0x3C = offset to Y coordinate
	__asm
	{
		MOV ECX, [ESI + 0x3C];
		MOV YaxisRegister, ESI; //get the
		MOV[ESI + 0x08], EDX;
		jmp[FlyingJmpBack];		//Jump back to the original code
	}
}

DWORD deathJmpBack = 0;
DWORD enemyBase = 0x0;

__declspec(naked) void getDeath()
{
	__asm
	{	
		MOV EAX, [EDI + 0x1EC];
		MOV enemyBase, EDI;
								//you must use 0x to make it hex
		jmp[deathJmpBack];		//Jump back to the games original code
	}
}