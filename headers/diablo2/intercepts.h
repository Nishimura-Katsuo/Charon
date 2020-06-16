/**
 * Declare stuff for the fun stuff.
 */
#pragma once

#include "D2Structs.h"
#include "pointers.h"
#include <chrono>
#include <thread>

const DWORD DEFAULT_FONT = 1;

bool __fastcall gameInput(wchar_t* wMsg);
void _gameInput();
HMODULE __stdcall multi(LPSTR Class, LPSTR Window);
void FTJReduce();
void _oogDraw();
void _gameDraw();
void _throttle();
void _gameAutomapDraw();
int __stdcall printf_newline(const char* format, ...);
namespace D2 {
    extern int ScreenWidth, ScreenHeight;
}
