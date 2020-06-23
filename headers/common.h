/**
 * Just some common stuff.
 */
#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <vector>
#include <iostream>
#include <string>
#include <unordered_map>
#include <functional>
#include <sstream>
#include <iomanip>

const std::wstring version = L"Charon v0.97";

class GameOutputBuffer : public std::wstringbuf
{
public:
    DWORD color = 0;
    virtual int sync();
};

class GameOutput : public std::wostream {
    GameOutputBuffer buf;
public:
    GameOutput();
    GameOutput& operator()(DWORD color);
};

extern GameOutput gamelog;

const DWORD DEFAULT_FONT = 1;

namespace D2 {
    extern int ScreenWidth, ScreenHeight;
}
