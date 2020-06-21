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

#include "../headers/diablo2/pointers.h"
#include "../headers/hook.h"
#include "../headers/diablo2/intercepts.h"
#include "../headers/diablo2/utilities.h"

struct DllMainArgs {
    HMODULE hModule;
    DWORD  ul_reason_for_call;
    LPVOID lpReserved;
};

// Apparently OOG is hard coded to 800x600? Sounds dumb.
namespace D2 {
    const DWORD DEFAULT_SCREEN_WIDTH = 800, DEFAULT_SCREEN_HEIGHT = 600;
}

typedef std::wstringstream& InputStream;
typedef std::function<BOOL(std::wstring, InputStream)> InputCallback;
typedef std::unordered_map<std::wstring, InputCallback> InputCallbackMap;
typedef InputCallbackMap::iterator InputMapIterator;
typedef std::pair<std::wstring, InputCallback> InputCallbackPair;

extern bool inGame;