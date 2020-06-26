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

ASMPTR keyPress = 0x46A847;
ASMPTR keyPress_II = 0x46A854;
ASMPTR keyPress_III = 0x46A93B;

BOOL __fastcall keyPressEvent(WPARAM wparam, LPARAM lparam) {

    BOOL chatBox = D2::GetUiFlag(0x05);
    BOOL escMenu = D2::GetUiFlag(0x09);

    //gamelog << COLOR(4) << "chatbox: " << chatBox << "\t" << "escMenu: " << escMenu << "\t" << wparam <<"\t" << lparam << std::endl;

    if (State["inGame"] && !chatBox && !escMenu) {

        char keycode = static_cast<char>(wparam);

        HotkeyMapIterator it = HotkeyCallbacks.find(keycode);
        if (it != HotkeyCallbacks.end()) {
            HotkeyCallback cb = it->second;
            if (cb(lparam)) {
                return true;
            }
        }
    }

    return false;
}

void __declspec(naked) _keyPressIntercept() {
    __asm {

        mov ecx, [edi + 0x08] //(wparam)
        mov edx, [edi + 0x0c] //(lparam)

        call keyPressEvent
        cmp eax, 0

        // need a separated label for this, cant make that big of a jump in jne
        jne block  // jump not equal

        // not blocked
        jmp[keyPress_II]

        block:
        jmp[keyPress_III] // block key for d2
    }
}

REMOTEFUNC(void, SoundChaosDebug, (), 0x4BABC0);

// Replaces the 'soundchaosdebug' command
BOOL __fastcall ChatCommandProcessor(char* msg) {
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
        // 28 - 15 = 13
        MemoryPatch(0x47c53d) << ASM::MOV_ECX_EDI << CALL(ChatCommandProcessor) << ASM::TEST_EAX << JUMP_ZERO((LPVOID)0x47ca4f) << BYTES(ASM::NOP, 13);

        // Override the d2 internal function of pressing a key
        MemoryPatch(keyPress) << JUMP(_keyPressIntercept) << ASM::NOP;

        // Since we patched this out, we should probably re-implement it
        ChatInputCallbacks[L"soundchaosdebug"] = [&](std::wstring cmd, InputStream wchat) -> BOOL {
            SoundChaosDebug();
            return FALSE;
        };
    }
} feature;
