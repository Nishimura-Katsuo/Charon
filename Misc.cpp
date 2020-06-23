/**
 * Small patches and features that don't belong elsewhere.
 */

#include "headers/feature.h"
#include "headers/common.h"
#include "headers/pointers.h"
#include "headers/hook.h"
#include <iostream>

void _drawAutoMapInfo(DWORD size) {
    DWORD width = 0, height = 0, fileno = 1;
    height = D2::GetTextSize(L"test", &width, &fileno);
    DWORD bottom = *D2::DrawAutoMapStatsOffsetY - height;
    for (AutomapInfoCallback func : AutomapInfoHooks) {
        std::wstring msg = func();
        bottom += D2::GetTextSize(msg.c_str(), &width, &fileno);
        D2::DrawGameText(msg.c_str(), D2::ScreenWidth - 16 - width, bottom, 4, 0);
    }
}

HMODULE __stdcall multi(LPSTR Class, LPSTR Window) {
    return 0;
}

// This feature class registers itself.
class : public Feature {
public:
    void init() {
        std::cout << "Installing misc patches and features..." << std::endl;
		MemoryPatch(0x4F5623) << CALL(multi) << ASM::NOP; // Allow multiple windows open
        MemoryPatch(0x476D40) << ASM::RET; // Ignore shaking requests
        MemoryPatch(0x43BF60) << ASM::RET; // Prevent battle.net connections
        MemoryPatch(0x515FB1) << BYTE(0x01); // Delay of 1 on cleaning up sounds after quiting game
        MemoryPatch(0x4781AC) << BYTESEQ{ 0x6A, 0x05, 0x90, 0x90, 0x90 }; // Hyperjoin for TCP/IP games

        MemoryPatch(0x45ADE8) << CALL(_drawAutoMapInfo);

        AutomapInfoHooks.push_back([]() -> std::wstring {
            return version;
            });

        AutomapInfoHooks.push_back([]() -> std::wstring {
            DWORD elapsed = GetTickCount() - 0, seconds = (elapsed / 1000) % 60, minutes = (elapsed / 60000) % 60;
            wchar_t msg[16];
            swprintf_s(msg, L"%d:%02d", minutes, seconds);
            return msg;
        });

        *D2::NoPickUp = true;
    }

    void oogPostDraw() {
        DWORD width = 0, height = 0, fileno = 1;
        height = D2::GetTextSize(version.c_str(), &width, &fileno);
        D2::DrawGameText(version.c_str(), D2::ScreenWidth - width - 5, D2::ScreenHeight - 5, 4, 0);
    }
} feature;
