/**
 * Charon - For the finer life in Diablo 2.
 */

#include "headers/common.h"

using std::wcout;
using std::cout;
using std::endl;

bool drawDebug = true, drawSwatch = false;
wchar_t wHex[] = L"0123456789ABCDEF";
const wchar_t* align[] = { L"Hostile", L"Neutral", L"Friendly" };
InputCallbackMap ChatInputCallbacks;

void drawBranding(bool inGame) {
    WCHAR msgtext[] = L"Charon v0.1";
    DWORD width = 0, height = 0, fileno = 1;

    height = D2::GetTextSize(msgtext, &width, &fileno);
    D2::DrawGameText(msgtext, D2::ScreenWidth - width - 5, D2::ScreenHeight - 5, inGame ? 0 : 4, 0);
}

void gameUnitPreDraw() {
    // Could use this to draw tile markers or something.
    // Server side tracks enemies
    // Needs filtering

    if (drawDebug) {
        POINT pos{ D2::PlayerUnit[0]->pPath->xPos, D2::PlayerUnit[0]->pPath->yPos };
        int gridsize = 30, color = 193;

        for (int x = -gridsize; x <= gridsize; x++) {
            for (int y = -gridsize; y <= gridsize; y++) {
                DrawWorldLine({ pos.x + x, pos.y + y }, { pos.x + x + 1, pos.y + y }, color);
                DrawWorldLine({ pos.x + x, pos.y + y }, { pos.x + x, pos.y + y + 1 }, color);
            }
        }

        BYTE d = 1;
        for (int c = 0; c < 128; c++) {
            for (D2::Types::UnitAny* unit = D2::ServerSideUnitHashTables[d].table[c]; unit != NULL; unit = unit->pListNext) {
                if (unit->pPath && unitHP(unit) > 0) {
                    // Turn this into a stat overlay with flags, etc for debugging
                    POINT pos = unitScreenPos(unit);

                    if (pos.x >= 0 && pos.y >= 0 && pos.x < D2::ScreenWidth && pos.y < D2::ScreenHeight) {
                        if (unit->pPath->xTarget || unit->pPath->yTarget) {
                            POINT target = WorldToScreen({ unit->pPath->xTarget, unit->pPath->yTarget });
                            target.y += 12;
                            DrawScreenLine(pos, target, 0x99);
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
        wchar_t msg[512];
        for (int c = 0; c < 128; c++) {
            for (D2::Types::UnitAny* unit = D2::ServerSideUnitHashTables[d].table[c]; unit != NULL; unit = unit->pListNext) {
                if (unit->pPath) {
                    // Turn this into a stat overlay with flags, etc for debugging
                    POINT pos = unitScreenPos(unit);

                    if (pos.x >= 0 && pos.y >= 0 && pos.x < D2::ScreenWidth && pos.y < D2::ScreenHeight) {
                        swprintf_s(msg, L"%s\n%s\n%s", (isAttackable(unit) ? L"Combat" : L"Non-Combat"), align[D2::GetUnitStat(unit, 172, 0)], D2::GetUnitName(unit));
                        DWORD fontNum = 12, width = 0, height = 0;
                        D2::SetFont(fontNum);
                        height = D2::GetTextSize(msg, &width, &fontNum);
                        D2::DrawGameText(msg, pos.x - (width >> 1), pos.y + height, 0, 1);
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
            if (isHostile(unit) && unitHP(unit) > 0) {
                DWORD color = 10;

                if (isAttackable(unit)) {
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
                }
                else {
                    color = 28;
                }

                DrawMinimapX(unit->pPath->xPos, unit->pPath->yPos, color);
            }
        }
    }
}

void gamePostDraw() {
    drawBranding(true);
    DWORD fontnum = 8, height, width;
    D2::SetFont(fontnum);
    if (drawSwatch) {
        wchar_t msg[3] = { 0 };
        int color, gridsize = 24;
        for (int x = 0; x < 32; x++) {
            for (int y = 0; y < 8; y++) {
                color = (x << 3) | y;
                D2::DrawRectangle(x * gridsize, y * gridsize, x * gridsize + gridsize, y * gridsize + gridsize, color, 0xFF);
                msg[0] = wHex[color >> 4];
                msg[1] = wHex[color & 15];
                height = D2::GetTextSize(msg, &width, &fontnum);
                D2::DrawGameText(msg, x * gridsize + (gridsize - width) / 2, (y + 1) * gridsize, 0, 0);
            }
        }
    }
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
    try {
        return ChatInputCallbacks.at(wMsg)(wMsg); // Find the callback, and then call it.
    }
    catch (...) {
        return TRUE; // Ignore the exception. Command not found.
    }
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
    MemoryPatch(D2::DisableBattleNetPatch) << BYTE(0xC3); // Prevent battle.net connections
    MemoryPatch(D2::EnableDebugPrint) << true; // Enable in-game debug prints
    MemoryPatch(D2::NullDebugPrintf) << JUMP(printf_newline); // Enable even more console debug prints

    MemoryPatch(D2::CustomDebugPrintPatch) << CALL(CustomDebugPrint);

    MemoryPatch(D2::ShakePatch) << BYTE(0xC3);

    *D2::NoPickUp = true;

    ChatInputCallbacks[L"~debug"] = [](wchar_t* wMsg) -> BOOL {
        drawDebug = !drawDebug;
        return FALSE;
    };

    ChatInputCallbacks[L"~swatch"] = [](wchar_t* wMsg) -> BOOL {
        drawSwatch = !drawSwatch;
        return FALSE;
    };

    // https://github.com/blizzhackers/d2bs/blob/6f2bc2fe658164590f3cb2196efa50acfcf154c2/Room.cpp#L35
    ChatInputCallbacks[L"~reveal"] = [](wchar_t* wMsg) -> BOOL {
        cout << "Revealing automap..." << endl;
        RevealCurrentLevel();
        return FALSE;
    };

    cout << "Charon loaded. Available commands:" << endl;

    for (const InputCallbackPair& kv : ChatInputCallbacks) {
        wcout << endl << "  " << kv.first;
    }

    cout << endl << endl;
}
