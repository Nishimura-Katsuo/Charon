/**
 * Enables chat command and hotkey hooks.
 *
 * Thanks to Jaenster for the hotkey stuff!
 */
#include "headers/feature.h"
#include "headers/common.h"
#include "headers/hook.h"
#include "headers/remote.h"
#include <iostream>

const ASMPTR SoundChaosCheckStart = 0x47c53d;
const ASMPTR SoundChaosCheckEnd = 0x47c559;
const DWORD SuccessfulCommandAddress = 0x47ca4f;
WNDPROC OldWndProc;

BOOL keyPressEvent(WPARAM wparam, LPARAM lparam) {

    BOOL chatBox = D2::GetUiFlag(0x05);
    BOOL escMenu = D2::GetUiFlag(0x09);

    //gamelog << COLOR(4) << "chatbox: " << chatBox << "\t" << "escMenu: " << escMenu << "\t" << wparam <<"\t" << lparam << std::endl;

    if (!State["inGame"] || !chatBox && !escMenu) {

        char keycode = static_cast<char>(wparam);

        HotkeyMapIterator it = HotkeyCallbacks.find(keycode);
        if (it != HotkeyCallbacks.end()) {
            HotkeyCallback cb = it->second;
            return cb(lparam);
        }
    }

    return true;
}

LRESULT CALLBACK WindowProc(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam) {
    HDC hdc;
    PAINTSTRUCT ps;

    switch (uMsg) {
    case WM_KEYDOWN:
        if (!(lParam & 0x40000000) && !keyPressEvent(wParam, lParam)) {
            return 0;
        }
        break;
    case WM_SYSKEYDOWN:
        if (!(lParam & 0x40000000) && !keyPressEvent(wParam, lParam)) {
            return 0;
        }
        break;
    }
    return OldWndProc(hwnd, uMsg, wParam, lParam);
}
ATOM __stdcall RegisterClassAHook(WNDCLASSA* lpWndClass) {
    OldWndProc = lpWndClass->lpfnWndProc;
    lpWndClass->lpfnWndProc = WindowProc;
    return RegisterClassA(lpWndClass);
}

REMOTEFUNC(void, SoundChaosDebug, (), 0x4BABC0);

// Replaces the 'soundchaosdebug' command
bool __fastcall ChatCommandProcessor(char* msg) {
    std::string tmp(msg);
    std::wstring wMsg(tmp.begin(), tmp.end());
    try {
        std::wstringstream msg(wMsg);
        std::wstring cmd;
        msg >> cmd;
        return ChatInputCallbacks.at(cmd)(cmd, msg); // Find the callback, and then call it.
    }
    catch (...) {
        return TRUE; // Ignore the exception. Command not found.
    }
}

// This feature class registers itself.
class : public Feature {
public:
    void init() {
        MemoryPatch(SoundChaosCheckStart)
            << ASM::MOV_ECX_EDI
            << CALL(ChatCommandProcessor)
            << ASM::TEST_AL
            << JUMP_ZERO(SuccessfulCommandAddress)
            << NOP_TO(SoundChaosCheckEnd);

        MemoryPatch(0x4f5379) << CALL(RegisterClassAHook) << ASM::NOP;

        // Since we patched this out, we should probably re-implement it
        ChatInputCallbacks[L"soundchaosdebug"] = [&](std::wstring cmd, InputStream wchat) -> BOOL {
            SoundChaosDebug();
            return FALSE;
        };
    }
} feature;
