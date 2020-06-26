#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include <sstream>
#include "utilities.h"

typedef std::wstringstream& InputStream;
typedef std::function<BOOL(std::wstring, InputStream)> InputCallback;
typedef std::unordered_map<std::wstring, InputCallback> InputCallbackMap;
typedef InputCallbackMap::iterator InputMapIterator;
typedef std::pair<std::wstring, InputCallback> InputCallbackPair;

// A hotkey is like a command
typedef std::function<BOOL(LPARAM lParam)> HotkeyCallback;
typedef std::unordered_map<int, HotkeyCallback> HotkeyCallbackMap;
typedef HotkeyCallbackMap::iterator HotkeyMapIterator;

typedef std::function<std::wstring()> AutomapInfoCallback;
typedef std::vector<AutomapInfoCallback> AutomapInfoCallbackList;

class Feature {
public:
	Feature* next;
	Feature();
	~Feature();
	virtual void init();
	virtual void deinit();
	virtual void gameLoop();
	virtual void oogLoop();
	virtual void gameUnitPreDraw();
	virtual void gameUnitPostDraw();
	virtual void gameAutomapPreDraw();
	virtual void gameAutomapPostDraw();
	virtual void gamePostDraw();
	virtual void oogPostDraw();
};

extern Feature* Features;

typedef std::unordered_map<std::string, DWORD> StateMap;

extern HotkeyCallbackMap HotkeyCallbacks;
extern AutomapInfoCallbackList AutomapInfoHooks;
extern InputCallbackMap ChatInputCallbacks;
extern StateMap State;
