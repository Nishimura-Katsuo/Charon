/**
 * Borrowed a bit from d2bs. Thanks noah!
 */
#include "headers/diablo2/intercepts.h"

void __declspec(naked) _gameInput() {
	__asm {
		pushad
		mov ecx, ebx
		call gameInput
		cmp eax, 0
		popad
		je BlockIt
		call D2::InputCall_I
		ret

		BlockIt:
		xor eax, eax
		ret
	}
}

HMODULE __stdcall multi(LPSTR Class, LPSTR Window) {
    return 0;
}

void __declspec(naked) FTJReduce() {
	__asm
	{
		cmp esi, 4000;
		ret;
	}
}
