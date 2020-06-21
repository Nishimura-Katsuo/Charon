/**
 * Declare stuff for the fun stuff.
 */
#pragma once

#include "D2Structs.h"
#include "pointers.h"
#include <chrono>
#include <thread>

const DWORD DEFAULT_FONT = 1;

BOOL __fastcall chatInput(wchar_t* wMsg);
void _chatInput();
HMODULE __stdcall multi(LPSTR Class, LPSTR Window);
void _oogDraw();
void _gameDraw();
void _throttle();
void _gameAutomapDraw();
void _preDrawUnitsPatch();
void _gameLoop();
void _oogLoop();
void _drawFloor();
int __stdcall printf_newline(const char* format, ...);
namespace D2 {
    extern int ScreenWidth, ScreenHeight;
}
