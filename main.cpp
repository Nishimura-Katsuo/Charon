/**
 * Charon - For the finer life in Diablo 2.
 */

#include "headers/common.h"

using std::wcout;
using std::cout;
using std::endl;

bool drawDebug = true;

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

POINT WorldToScreen(POINT point) {
    WorldToScreen(&point.x, &point.y);
    return point;
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

POINT WorldToAutomap(POINT point) {
    WorldToAutomap(&point.x, &point.y);
    return point;
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

void DrawAutomapLine(POINT p1, POINT p2, DWORD dwColor, DWORD dwOpacity) {
    p1 = WorldToAutomap(p1);
    p2 = WorldToAutomap(p2);
    D2::DrawLine(p1.x, p1.y, p2.x, p2.y, dwColor, dwOpacity);
}

void DrawWorldLine(POINT p1, POINT p2, DWORD dwColor, DWORD dwOpacity) {
    p1 = WorldToScreen(p1);
    p2 = WorldToScreen(p2);
    D2::DrawLine(p1.x, p1.y, p2.x, p2.y, dwColor, dwOpacity);
}

void DrawScreenLine(POINT p1, POINT p2, DWORD dwColor, DWORD dwOpacity) {
    D2::DrawLine(p1.x, p1.y, p2.x, p2.y, dwColor, dwOpacity);
}

DWORD unitHP(D2::Types::UnitAny* unit) {
    return D2::GetUnitStat(unit, 6, 0) >> 8;
}

bool isFriendly(D2::Types::UnitAny* unit) {
    return D2::GetUnitStat(unit, 172, 0) == 2;
}

bool isEnemy(D2::Types::UnitAny *unit) {
    return unitHP(unit) > 0 && !isFriendly(unit);
}

POINT unitScreenPos(D2::Types::UnitAny* unit) {
    // Turn this into a stat overlay with flags, etc for debugging
    POINT pos{ unit->pPath->xPos, unit->pPath->yPos }, offset{ unit->pPath->xOffset, unit->pPath->yOffset };

    WorldToScreen(&pos.x, &pos.y);
    WorldToScreen(&offset.x, &offset.y);

    pos.x += offset.x >> 16;
    pos.y += (offset.y >> 16) + 6;

    return pos;
}


void gameUnitPreDraw() {
    // Could use this to draw tile markers or something.
    // Server side tracks enemies
    // Needs filtering

    if (drawDebug) {
        wchar_t msg[255];
        POINT pos{ D2::PlayerUnit[0]->pPath->xPos, D2::PlayerUnit[0]->pPath->yPos };
        DWORD color = 28;
        swprintf_s(msg, L"Using color: %d", color);
        D2::SetFont(0);
        D2::DrawGameText(msg, 0, 16, 0, 0);

        for (int x = -16; x <= 16; x++) {
            for (int y = -16; y <= 16; y++) {
                if (x < 16) {
                    DrawWorldLine({ pos.x + x, pos.y + y }, { pos.x + x + 1, pos.y + y }, color, 1);
                }
                if (y < 16) {
                    DrawWorldLine({ pos.x + x, pos.y + y }, { pos.x + x, pos.y + y + 1 }, color, 1);
                }
            }
        }

        BYTE d = 1;
        for (int c = 0; c < 128; c++) {
            for (D2::Types::UnitAny* unit = D2::ServerSideUnitHashTables[d].table[c]; unit != NULL; unit = unit->pListNext) {
                if (unit->pPath) {
                    // Turn this into a stat overlay with flags, etc for debugging
                    POINT pos = unitScreenPos(unit);

                    if (pos.x >= 0 && pos.y >= 0 && pos.x < D2::ScreenWidth && pos.y < D2::ScreenHeight) {
                        if (unitHP(unit) > 0) {
                            POINT target = WorldToScreen({ unit->pPath->xTarget, unit->pPath->yTarget });
                            target.y += 12;
                            DrawScreenLine(pos, target, 0x99, 1);
                        }
                    }
                }
            }
        }
    }
}

void gameUnitPostDraw() {
    // Server side tracks enemies
    // Needs filtering
    if (drawDebug) {
        BYTE d = 1;
        wchar_t msg[255];
        for (int c = 0; c < 128; c++) {
            for (D2::Types::UnitAny* unit = D2::ServerSideUnitHashTables[d].table[c]; unit != NULL; unit = unit->pListNext) {
                if (unit->pPath) {
                    // Turn this into a stat overlay with flags, etc for debugging
                    POINT pos = unitScreenPos(unit);

                    if (pos.x >= 0 && pos.y >= 0 && pos.x < D2::ScreenWidth && pos.y < D2::ScreenHeight) {

                        swprintf_s(msg, L"%s", D2::GetUnitName(unit));
                        DWORD fontNum = 12, width = 0, height = 0;
                        D2::SetFont(fontNum);
                        height = D2::GetTextSize(msg, &width, &fontNum);
                        D2::DrawGameText(msg, pos.x - (width >> 1), pos.y + height, 0, 0);
                    }
                }
            }
        }
    }
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
            }
        }
    }
}

void gamePostDraw() {
    drawBranding(true);
}

void gameLoop() {
    //cout << "In-game!" << endl;
}

void oogPostDraw() {
    drawBranding(false);
}

void oogLoop() {
    //cout << "Out-of-game!" << endl;
}

BOOL __fastcall chatInput(wchar_t* wMsg) {
    if (wMsg[0] == L'~') {
        drawDebug = !drawDebug;
        return FALSE;
    }

    return TRUE;
}

void __fastcall myDebugPrint(DWORD unk, char* szMsg, DWORD color) {
    cout << (LPVOID)unk << " " << szMsg;
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

    MemoryPatch(D2::PreDrawUnitsPatch) << CALL(_preDrawUnitsPatch); // Hook the unit draw
    MemoryPatch(D2::DrawWorldEndPatch) << JUMP(gameUnitPostDraw);
    MemoryPatch(D2::GameAutomapDrawPatch) << CALL(_gameAutomapDraw); // Hook the automap draw
    MemoryPatch(D2::GameDrawPatch) << CALL(_gameDraw); // Hook the game draw
    MemoryPatch(D2::oogDrawPatch) << CALL(_oogDraw); // Hook the oog draw
    MemoryPatch(D2::MultiPatch) << CALL(multi) << ASM::NOP; // Allow multiple windows open
    MemoryPatch(D2::ChatInputPatch) << CALL(_chatInput); // Intercept game input
    MemoryPatch(D2::FTJReducePatch) << BYTESEQ{ 0x81, 0xFE, 0xA0, 0x0F, 0x00, 0x00 }; // FTJ Patch - cmp esi, 4000
    //MemoryPatch(D2::DisableBattleNetPatch) << BYTE(0xC3); // Prevent battle.net connections
    MemoryPatch(D2::EnableDebugPrint) << true; // Enable in-game debug prints
    MemoryPatch(D2::NullDebugPrintf) << JUMP(printf_newline); // Enable even more console debug prints

    MemoryPatch(D2::CustomDebugPrintPatch) << CALL(myDebugPrint);

    *D2::NoPickUp = true;

    cout << "Charon loaded." << endl;
}
