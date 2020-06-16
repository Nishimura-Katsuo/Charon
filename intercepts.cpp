/**
 * Borrowed a bit from d2bs. Thanks noah!
 */
#include "headers/common.h"
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

void oogDraw();
void gameDraw();

// Since we patch to override DrawSprites, we need to call it ourselves.
void _oogDraw() {
	// This is hard coded and the game doesn't maintain it when OOG, so we do it here.
	*D2::ScreenSizeX = D2::DEFAULT_SCREEN_WIDTH;
	*D2::ScreenSizeY = D2::DEFAULT_SCREEN_HEIGHT;
	DWORD old = D2::SetFont(DEFAULT_FONT);
	oogDraw();
	D2::SetFont(old);
	D2::DrawSprites();
}

void _gameDraw() {
	DWORD old = D2::SetFont(DEFAULT_FONT);
	gameDraw();
	D2::SetFont(old);
}

// Keeps the game at a steady framerate without using too much CPU.
// D2 doesn't do a great job at it by default, so we're helping out.
void _throttle() {
	using frameDuration = std::chrono::duration<int64_t, std::ratio<1, 25>>; // Limit the game to 25 fps always (matches OOG and single player)
	using std::chrono::system_clock;
	using std::this_thread::sleep_until;
	static system_clock::time_point nextFrame = system_clock::now(), now;

	now = system_clock::now();

	while (nextFrame < now) {
		nextFrame += frameDuration{ 1 };
	}

	sleep_until(nextFrame);
}

void gameAutomapPreDraw();
void gameAutomapPostDraw();

void _gameAutomapDraw() {
	gameAutomapPreDraw();
	D2::DrawAutomap();
	gameAutomapPostDraw();
}
