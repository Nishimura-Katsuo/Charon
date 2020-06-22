/**
 * Borrowed a bit from d2bs. Thanks noah!
 */
#include "../headers/common.h"

bool inGame = false;

void __declspec(naked) _chatInput() {
	__asm {
		pushad
		mov ecx, ebx
		call chatInput
		cmp eax, 0
		popad
		je BlockIt
		call D2::ChatInput
		ret

		BlockIt :
		xor eax, eax
			ret
	}
}
void __fastcall keyPressEvent(WPARAM wparam, LPARAM lparam);
void __declspec(naked) _keyPressIntercept() {
	__asm {
		
		mov ecx, [edi + 0x08] //(wparam)
		mov edx, [edi + 0x0c] //(lparam)

		call keyPressEvent
		//ToDo; implement blocking a key
	
		jmp [D2::keyPress_II]
	}
}

HMODULE __stdcall multi(LPSTR Class, LPSTR Window) {
    return 0;
}

void oogPostDraw();
void gamePostDraw();

namespace D2 {
	int ScreenWidth = 0, ScreenHeight = 0;
}

// Since we patch to override DrawSprites, we need to call it ourselves.
void _oogDraw() {
	inGame = false;
	D2::GetScreenModeSize(D2::GetScreenMode(), &D2::ScreenWidth, &D2::ScreenHeight);
	DWORD old = D2::SetFont(DEFAULT_FONT);
	oogPostDraw();
	D2::SetFont(old);
	D2::DrawSprites();
}

void _gameDraw() {
	DWORD old = D2::SetFont(DEFAULT_FONT);
	gamePostDraw();
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
	if (now < nextFrame) {
		sleep_until(nextFrame);
		nextFrame += frameDuration{ 1 };
	}
	else {
		nextFrame = system_clock::now() + frameDuration{ 1 };
	}
}

void gameAutomapPreDraw();
void gameAutomapPostDraw();
void gameUnitPreDraw();

void _gameAutomapDraw() {
	gameAutomapPreDraw();
	D2::DrawAutomap();
	gameAutomapPostDraw();
}

void _preDrawUnitsPatch() {
	inGame = true;
	gameUnitPreDraw();
	__asm {
		call D2::SomethingBeforeDrawUnits
	}
}

void gameLoop();
void oogLoop();

void _gameLoop() {
	gameLoop();
}

void _oogLoop() {
	oogLoop();
}

void gameDrawAutoMapInfo();

void _drawAutoMapInfo(DWORD size) {
	gameDrawAutoMapInfo();
}

// This is based on the actual source for printf... uses varargs.
int __stdcall printf_newline(const char* format, ...) {
	va_list arg;
	int done;

	va_start(arg, format);
	done = vfprintf(stdout, format, arg);
	va_end(arg);

	// We want to force a newline after debug prints :)
	puts("");

	return done;
}

void __fastcall _drawFloor(void *unknown) {
	if (!debugMode) {
		D2::DrawFloor(unknown);
	}
}