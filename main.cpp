/**
 * Charon - For the finer life in Diablo 2.
 */

#include "headers/common.h"

using std::wcout;
using std::cout;
using std::endl;

void drawBranding(bool inGame) {
    WCHAR msgtext[] = L"Charon v0.1";
    DWORD width = 0, height = 0, fileno = 1;

    height = D2::GetTextSize(msgtext, &width, &fileno);
    D2::DrawGameText(msgtext, D2::ScreenWidth - width - 5, D2::ScreenHeight - 5, inGame ? 0 : 4, 0);
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

void gameAutomapPreDraw() {

}

void gameAutomapPostDraw() {
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

void gameDraw() {
    drawBranding(true);
}

void gameLoop() {
    //cout << "In-game!" << endl;
}

void oogDraw() {
    drawBranding(false);
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
    MemoryPatch(D2::GameLoopPatch)
        << CALL(gameLoop)
        << CALL(_throttle)
        << BYTES(ASM::NOP, 22);

    // override the entire sleepy section - 23 bytes long
    MemoryPatch(D2::oogLoopPatch)
        << CALL(oogLoop)
        << CALL(_throttle)
        << BYTES(ASM::NOP, 13);

    MemoryPatch(D2::GameAutomapDrawPatch) << CALL(_gameAutomapDraw); // Hook the automap draw
    MemoryPatch(D2::GameDrawPatch) << CALL(_gameDraw); // Hook the game draw
    MemoryPatch(D2::oogDrawPatch) << CALL(_oogDraw); // Hook the oog draw
    MemoryPatch(D2::MultiPatch) << CALL(multi) << ASM::NOP; // Allow multiple windows open
    MemoryPatch(D2::GameInputPatch) << CALL(_gameInput); // Intercept game input
    MemoryPatch(D2::FTJReducePatch) << CALL(FTJReduce) << ASM::NOP; // Reduce Failed To Join (QoL fix)
    MemoryPatch(D2::DisableBattleNetPatch) << BYTE(0xC3); // Prevent battle.net connections
    MemoryPatch(D2::EnableDebugPrint) << true; // Enable in-game debug prints
    MemoryPatch(D2::NullDebugPrintf) << JUMP(printf_newline); // Enable even more console debug prints

    *D2::NoPickUp = true;

    cout << "Charon loaded." << endl;
}
