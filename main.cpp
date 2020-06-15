/**
 * Charon - For the finer life in Diablo 2.
 */

#include "headers/common.h"
#include "headers/diablo2/intercepts.h"
#include <chrono>
#include <thread>

using std::wcout;
using std::cout;
using std::endl;

void drawStats(bool inGame) {
    WCHAR msgtext[] = L"Charon v0.1";
    DWORD width = 0, fileno = 1, screenmode = D2::GetScreenSize(), ScreenWidth = 0, ScreenHeight = 0;

    switch (screenmode) {
    case 0:
        ScreenWidth = 640;
        ScreenHeight = 480;
        break;
    case 1:
    case 2:
        ScreenWidth = 800;
        ScreenHeight = 600;
        break;
    }

    DWORD old = D2::SetFontFileNumber(1);
    D2::GetTextSize(msgtext, &width, &fileno);
    D2::DrawText(msgtext, ScreenWidth - width - 5, ScreenHeight - 5, inGame ? 0 : 4, 0);
    D2::SetFontFileNumber(old);
}

// Keeps the game at a steady framerate without using too much CPU.
// D2 doesn't do a great job at it by default, so we're helping out.
void throttle() {
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

// Since we patch to override this function, we need to call it ourselves.
void gameDraw() {
    drawStats(true);
    //D2::unknown_53B30();
}

void gameLoop() {
    //cout << "In-game!" << endl;
}

// Since we patch to override DrawSprites, we need to call it ourselves.
void oogDraw() {
    drawStats(false);
    D2::DrawSprites();
}

void oogLoop() {
    //cout << "Out-of-game!" << endl;
}

bool __fastcall gameInput(wchar_t* wMsg) {
    if (wMsg[0] == L'/') {
        return FALSE;
    }

    return TRUE;
}

void init(std::vector<LPWSTR> argv, DllMainArgs dllargs) {
    // override the entire sleepy section - 32 bytes long
    PatchCall<5>(Offset::Base + 0x51C2A, gameLoop);
    PatchCall<5>(Offset::Base + 0x51C2A + 5, throttle);
    SetBytes<22>(Offset::Base + 0x51C2A + 10, 0x90);

    // override the entire sleepy section - 23 bytes long
    PatchCall<5>(Offset::Base + 0xFA663, oogLoop);
    PatchCall<5>(Offset::Base + 0xFA663 + 5, throttle);
    SetBytes<13>(Offset::Base + 0xFA663 + 10, 0x90);

    PatchCall<5>(Offset::Base + 0x4CB14, gameDraw); // Hook the game draw
    PatchCall<5>(Offset::Base + 0xF9A0D, oogDraw); // Hook the oog draw

    PatchCall<6>(Offset::Base + 0xF5623, multi); // Allow multiple windows open
    PatchCall<5>(Offset::Base + 0x7C89D, _gameInput); // Intercept game input
    PatchCall<6>(Offset::Base + 0x4EF28, FTJReduce); // Reduce Failed To Join (QoL fix)
    SetBytes<1>(Offset::Base + 0x3BF60, 0xC3); // Prevent battle.net connections

    SetBytes<1>(Offset::Base + 0x4846DC, 1); // enables debug printouts
    
    *D2::NoPickUp = true;

    cout << "Charon loaded." << endl;
}
