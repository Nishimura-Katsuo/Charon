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

void drawStats() {
    D2::DrawText(L"Charon v0.1", 700, 595, 4, 0);
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

void gameDraw() {
    drawStats();
}

void gameLoop() {
    //cout << "In-game!" << endl;
}

void oogDraw() {
    drawStats();
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

    PatchJump<5>(Offset::Base + 0x53B30, gameDraw); // Hook the game draw
    PatchCall<5>(Offset::Base + 0xF9A0D, oogDraw); // Hook the oog draw

    PatchCall<6>(Offset::Base + 0xF5623, multi); // Allow multiple windows open
    PatchCall<5>(Offset::Base + 0x7C89D, _gameInput); // Intercept game input
    PatchCall<6>(Offset::Base + 0x4EF28, FTJReduce); // Reduce Failed To Join (QoL fix)
    SetBytes<1>(Offset::Base + 0x3BF60, 0xC3); // Prevent battle.net connections

    cout << "Charon loaded." << endl;
}
