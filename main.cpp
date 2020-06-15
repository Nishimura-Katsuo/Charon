/**
 * Charon - For the finer life in Diablo 2.
 */

#include "headers/common.h"
#include "headers/diablo2/intercepts.h"

using std::wcout;
using std::cout;
using std::endl;

void drawStats(bool inGame) {
    WCHAR msgtext[] = L"Charon v0.1";
    DWORD width = 0, height = 0, fileno = 1;

    height = D2::GetTextSize(msgtext, &width, &fileno);
    D2::DrawGameText(msgtext, *D2::ScreenSizeX - width - 5, *D2::ScreenSizeY - 5, inGame ? 0 : 4, 0);
}

// We can use this to mark objects directly on the screen (could probably simulate item beams like diablo 3)
void WorldToScreen(long *x, long *y) {
    D2::MapToAbsScreen(x, y);
    *x -= D2::GetMouseXOffset();
    *y -= D2::GetMouseYOffset();
}

void ScreenToWorld(long* x, long* y) {
    D2::AbsScreenToMap(x, y);
    *x += D2::GetMouseXOffset();
    *y += D2::GetMouseYOffset();
}

// Handy for maphack stuff
void WorldToAutomap(long* x, long* y) {
    D2::MapToAbsScreen(x, y);
    *x = *x / *D2::Divisor - D2::Offset->x + 8;
    *y = *y / *D2::Divisor - D2::Offset->y - 8;
}

void DrawMinimapLine(long x1, long y1, long x2, long y2, DWORD dwColor = 0x62, DWORD dwOpacity = 0xFF) {
    WorldToAutomap(&x1, &y1);
    WorldToAutomap(&x2, &y2);
    D2::DrawLine(x1, y1, x2, y2, dwColor, dwOpacity);
}

void DrawMinimapDot(long x1, long y1, DWORD dwColor = 0x62, DWORD dwOpacity = 0xFF) {
    WorldToAutomap(&x1, &y1);
    D2::DrawLine(x1 - 1, y1, x1 + 1, y1, dwColor, dwOpacity);
    D2::DrawLine(x1, y1 - 1, x1, y1 + 1, dwColor, dwOpacity);
}

void DrawMinimapX(long x, long y, DWORD dwColor = 0x62, DWORD dwOpacity = 0xFF, int size = 5) {
    DrawMinimapLine(x, y - size, x, y + size, dwColor, dwOpacity);
    DrawMinimapLine(x - size, y, x + size, y, dwColor, dwOpacity);
}

bool isEnemy(D2::Types::UnitAny *unit) {
    DWORD hp = D2::GetUnitStat(unit, 6, 0) >> 8, alignment = D2::GetUnitStat(unit, 172, 0);
    return hp > 0 && alignment != 2;
}

void gameDraw() {
    drawStats(true);

    if (*D2::AutomapOn) {
        // Client side tracks missiles
        // Needs filtering, and maybe coloring 
        BYTE d = 3;
        for (int c = 0; c < 128; c++) {
            for (D2::Types::UnitAny* unit = D2::ClientSideUnitHashTables[d].table[c]; unit != NULL; unit = unit->pListNext) {
                DrawMinimapDot(unit->pPath->xPos, unit->pPath->yPos, 0x99, 0xFF);
            }
        }

        // Server side tracks enemies
        // Needs filtering
        d = 1;
        for (int c = 0; c < 128; c++) {
            for (D2::Types::UnitAny* unit = D2::ServerSideUnitHashTables[d].table[c]; unit != NULL; unit = unit->pListNext) {
                if (isEnemy(unit)) {
                    DWORD color = 10;

                    if (unit->pMonsterData->fBoss) {
                        color = 12;
                    }
                    else if (unit->pMonsterData->fChamp) {
                        color = 12;
                    }
                    else if (unit->pMonsterData->fMinion) {
                        color = 12;
                    }
                    else if (unit->pMonsterData->fUnk) {
                        color = 13;
                    }
                    else {
                        color = 10;
                    }

                    DrawMinimapX(unit->pPath->xPos, unit->pPath->yPos, color);

                    // Turn this into a stat overlay with flags, etc for debugging
                    long sX = unit->pPath->xPos, sY = unit->pPath->yPos;
                    WorldToScreen(&sX, &sY);
                    D2::DrawRectangle(sX - 10, sY - 10, sX + 10, sY + 10, color, 1);
                }
            }
        }
    }
}

void gameLoop() {
    //cout << "In-game!" << endl;
}

void oogDraw() {
    drawStats(false);
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
    PatchCall<5>(Offset::Base + 0x51C2A + 5, _throttle);
    SetBytes<22>(Offset::Base + 0x51C2A + 10, 0x90);

    // override the entire sleepy section - 23 bytes long
    PatchCall<5>(Offset::Base + 0xFA663, oogLoop);
    PatchCall<5>(Offset::Base + 0xFA663 + 5, _throttle);
    SetBytes<13>(Offset::Base + 0xFA663 + 10, 0x90);

    PatchCall<5>(Offset::Base + 0x4CB14, _gameDraw); // Hook the game draw
    PatchCall<5>(Offset::Base + 0xF9A0D, _oogDraw); // Hook the oog draw

    PatchCall<6>(Offset::Base + 0xF5623, multi); // Allow multiple windows open
    PatchCall<5>(Offset::Base + 0x7C89D, _gameInput); // Intercept game input
    PatchCall<6>(Offset::Base + 0x4EF28, FTJReduce); // Reduce Failed To Join (QoL fix)
    SetBytes<1>(Offset::Base + 0x3BF60, 0xC3); // Prevent battle.net connections

    SetBytes<1>(Offset::Base + 0x4846DC, 1); // enables debug printouts
    
    *D2::NoPickUp = true;

    cout << "Charon loaded." << endl;
}
