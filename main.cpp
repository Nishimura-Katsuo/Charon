/**
 * Charon - For the finer life in Diablo 2.
 */

#include "headers/common.h"

using std::wcout;
using std::cout;
using std::endl;

void gameDraw() {
    D2::DrawText(L"Charon v0.1", 700, 595, 4, 0);
}

void OOGDraw() {
    D2::DrawText(L"Charon v0.1", 700, 595, 4, 0);
}

void gameLoop() {
}

void init(std::vector<LPWSTR> argv, DllMainArgs dllargs) {
    PatchCall<7>(Offset::Base + 0x51C31, gameLoop);
    PatchJump<5>(Offset::Base + 0x53B30, gameDraw);
    PatchCall<5>(Offset::Base + 0xF9A0D, OOGDraw);
    cout << "Charon loaded." << endl;
}
