/**
 * Charon - For the finer life in Diablo 2.
 */

#include "headers/common.h"

std::wstring version = L"Charon v0.97";

bool debugMode = true, drawSwatch = false, debugVerbose = false;
wchar_t wHex[] = L"0123456789ABCDEF";
const wchar_t* align[] = { L"Hostile", L"Neutral", L"Friendly" };
DWORD currentLevel, revealDelay = 0;

InputCallbackMap ChatInputCallbacks;
typedef std::function<std::wstring()> AutomapInfoCallback;
std::vector<AutomapInfoCallback> AutomapInfoHooks;

DWORD gametime = 0;

using std::hex;

void drawBranding(bool inGame) {
    DWORD width = 0, height = 0, fileno = 1;

    if (!inGame) {
        height = D2::GetTextSize(version.c_str(), &width, &fileno);
        D2::DrawGameText(version.c_str(), D2::ScreenWidth - width - 5, D2::ScreenHeight - 5, inGame ? 0 : 4, 0);
    }
}

void gameUnitPreDraw() {
    BYTE d;
    // Server side tracks enemies
    if (debugMode) {
        D2::Types::Room1* current = D2::PlayerUnit[0]->pPath->pRoom1;
        D2::Types::CollMap *coll = current->Coll;
        WORD* p = coll->pMapStart;
        DWORD color, x, y;

        for (x = 0; x < coll->dwSizeGameX; x++) {
            for (y = 0; y < coll->dwSizeGameY; y++) {
                color = coll->getCollision(x, y, 0x4) ? 0x62 : coll->getCollision(x, y, 0xC09) ? 0x4B : coll->getCollision(x, y, 0x180) ? 0x8E : coll->getCollision(x, y, 0x10) ? 0x4 : 0x18;
                DrawWorldX({ (double)coll->dwPosGameX + (double)x + 0.5, (double)coll->dwPosGameY + (double)y + 0.5 }, color, 0.5);
            }
        }

        for (unsigned int c = 0; c < current->dwRoomsNear; c++) {
            coll = current->pRoomsNear[c]->Coll;
            p = coll->pMapStart;
            for (x = 0; x < coll->dwSizeGameX; x++) {
                for (y = 0; y < coll->dwSizeGameY; y++) {
                    color = coll->getCollision(x, y, 0x4) ? 0x62 : coll->getCollision(x, y, 0xC09) ? 0x4B : coll->getCollision(x, y, 0x180) ? 0x8E : coll->getCollision(x, y, 0x10) ? 0x4 : 0x18;
                    DrawWorldX({ (double)coll->dwPosGameX + (double)x + 0.5, (double)coll->dwPosGameY + (double)y + 0.5 }, color, 0.5);
                }
            }
        }

        d = 1;
        for (int c = 0; c < 128; c++) {
            for (D2::Types::UnitAny* unit = D2::ServerSideUnitHashTables[d].table[c]; unit != NULL; unit = unit->pListNext) {
                if (unit->pPath && unitHP(unit) > 0) {
                    POINT pos = WorldToScreen(unit->pPath), target = WorldToScreen({ (double)unit->pPath->xTarget, (double)unit->pPath->yTarget });

                    if (pos.x >= 0 && pos.y >= 0 && pos.x < D2::ScreenWidth && pos.y < D2::ScreenHeight && target.x >= 0 && target.y >= 0 && target.x < D2::ScreenWidth && target.y < D2::ScreenHeight) {
                        DrawLine(pos, target, 0x99);
                    }
                }
            }
        }

        // Client side tracks missiles
        d = 3;
        for (int c = 0; c < 128; c++) {
            for (D2::Types::UnitAny* unit = D2::ClientSideUnitHashTables[d].table[c]; unit != NULL; unit = unit->pListNext) {
                DrawWorldX(unit->pPath, 0x99);
                POINT pos = WorldToScreen(unit->pPath), target = WorldToScreen({ (double)unit->pPath->xTarget, (double)unit->pPath->yTarget });

                if (pos.x >= 0 && pos.y >= 0 && pos.x < D2::ScreenWidth && pos.y < D2::ScreenHeight && target.x >= 0 && target.y >= 0 && target.x < D2::ScreenWidth && target.y < D2::ScreenHeight) {
                    DrawLine(pos, target, 0x83);
                }
            }
        }
    }

    D2::Types::UnitAny* player = D2::GetPlayerUnit();
    if (player) {
        D2::Types::Level* level = player->pPath->pRoom1->pRoom2->pLevel;
        if (level) {
            // Loop trough all rooms of current lvl
            for (D2::Types::Room2* room = player->pPath->pRoom1->pRoom2->pLevel->pRoom2First; room; room = room->pRoom2Next) {
                if (room->pPreset) {
                    // Loop trough presets of current lvl
                    for (D2::Types::PresetUnit* ps = room->pPreset; ps; ps = ps->pPresetNext) {
                        //@ToDo; figure out which dots are important and which aren't
                        // for now all we do is draw a green dot
                        if (ps->dwType == 5) {
                            DrawWorldRadialShape({ (double)room->dwPosX * 5 + ps->dwPosX, (double)room->dwPosY * 5 + ps->dwPosY }, 4, 32, 0x83);
                        }
                    }
                }

            }
        }
    }
}

void gameUnitPostDraw() {
    BYTE d;

    // Server side tracks enemies
    if (debugMode && debugVerbose) {
        wchar_t msg[512];
        DWORD fontNum = 12, width = 0, height = 0;
        D2::SetFont(fontNum);
        d = 1;
        for (int c = 0; c < 128; c++) {
            for (D2::Types::UnitAny* unit = D2::ServerSideUnitHashTables[d].table[c]; unit != NULL; unit = unit->pListNext) {
                if (unit->pPath) {
                    POINT pos = WorldToScreen(unit->pPath);

                    if (pos.x >= 0 && pos.y >= 0 && pos.x < D2::ScreenWidth && pos.y < D2::ScreenHeight) {
                        swprintf_s(msg, L"%s\n%s\n%s", (isAttackable(unit) ? L"Combat" : L"Non-Combat"), align[D2::GetUnitStat(unit, 172, 0)], D2::GetUnitName(unit));
                        height = D2::GetTextSize(msg, &width, &fontNum);
                        D2::DrawGameText(msg, pos.x - (width >> 1), pos.y + height, 0, 1);
                    }
                }
            }
        }

        // Client side tracks missiles
        d = 3;
        POINT pos;
        for (int c = 0; c < 128; c++) {
            for (D2::Types::UnitAny* unit = D2::ClientSideUnitHashTables[d].table[c]; unit != NULL; unit = unit->pListNext) {
                pos = WorldToScreen(unit->pPath);
                swprintf_s(msg, L"dwOwnerType: %d", unit->dwOwnerType);
                height = D2::GetTextSize(msg, &width, &fontNum);
                D2::DrawGameText(msg, pos.x - (width >> 1), pos.y + height, 0, 1);
            }
        }
    }
}

void gameAutomapPreDraw() {
    // Draw behind automap. Not called when automap is off.
}

DWORD ItemRarityColor[32] = { 255, 29, 30, 32, 151, 132, 111, 155, 111 };

void gameAutomapPostDraw() {
    BYTE d;

    D2::Types::UnitAny* player = D2::GetPlayerUnit();
    if (player) {
        D2::Types::Level* level = player->pPath->pRoom1->pRoom2->pLevel;
        if (level) {
            // Loop trough all rooms of current lvl
            for (D2::Types::Room2* room = player->pPath->pRoom1->pRoom2->pLevel->pRoom2First; room; room = room->pRoom2Next) {
                if (room->pPreset) {
                    // Loop trough presets of current lvl
                    for (D2::Types::PresetUnit* ps = room->pPreset; ps; ps = ps->pPresetNext) {
                        //@ToDo; figure out which dots are important and which aren't
                        // for now all we do is draw a green dot
                        if (ps->dwType == 5) {
                            DrawAutomapRadialShape({ (double)room->dwPosX * 5 + ps->dwPosX, (double)room->dwPosY * 5 + ps->dwPosY }, 4, 9, 0x83);
                        }
                    }
                }

            }

            for (FoundExit exit : RevealedExits[level->dwLevelNo]) {
                //DrawLine(WorldToAutomap(exit.origin), WorldToAutomap(exit.target), 0x83);
            }
        }
    }

    // Client side tracks missiles
    d = 3;
    for (int c = 0; c < 128; c++) {
        for (D2::Types::UnitAny* unit = D2::ClientSideUnitHashTables[d].table[c]; unit != NULL; unit = unit->pListNext) {
            unit->dwOwnerType;
            switch (unit->dwOwnerType) {
                // Missiles cast directly by us (not owned units, like merc, hydras, etc)
                case 0:
                    DrawDot(WorldToAutomap(unit->pPath), 0x99);
                    break;
                // Most others
                case 1:
                default:
                    DrawDot(WorldToAutomap(unit->pPath), 0x99);
                    break;
            }
        }
    }

    // Server side tracks items
    d = 4;
    for (int c = 0; c < 128; c++) {
        for (D2::Types::UnitAny* unit = D2::ServerSideUnitHashTables[d].table[c]; unit != NULL; unit = unit->pListNext) {
            if (unit->dwMode == 3 || unit->dwMode == 5) {
                if (unit->pItemData->dwFlags & 0x4000000) {
                    DrawAutomapX(unit->pItemPath, ItemRarityColor[7], 3);
                }
                else if (unit->pItemData->dwQuality > 3 || (unit->pItemData->dwFlags & 0x400800 && unit->pItemData->dwQuality > 2 && D2::GetUnitStat(unit, 194, 0) >= 2)) { // @todo check number of sockets
                    DrawAutomapX(unit->pItemPath, ItemRarityColor[unit->pItemData->dwQuality], 3);
                }
                else {
                    D2::Types::ItemTxt *txt = D2::GetItemText(unit->dwTxtFileNo);
                    if (txt->nType >= 96 && txt->nType <= 102 || txt->nType == 74) {
                        DrawAutomapX(unit->pItemPath, 169, 3);
                    }
                }
            }
        }
    }

    // Server side tracks enemies
    d = 1;
    for (int c = 0; c < 128; c++) {
        for (D2::Types::UnitAny* unit = D2::ServerSideUnitHashTables[d].table[c]; unit != NULL; unit = unit->pListNext) {
            if (isHostile(unit) && unitHP(unit) > 0) {
                if (isAttackable(unit)) {
                    if (unit->pMonsterData->fBoss || unit->pMonsterData->fChamp) {
                        DrawAutomapX(unit->pPath, 0x0C);
                    }
                    else if (unit->pMonsterData->fMinion) {
                        DrawAutomapX(unit->pPath, 0x0B);
                    }
                    else {
                        DrawAutomapX(unit->pPath, 0x0A);
                    }
                }
                else {
                    DrawAutomapX(unit->pPath, 0x1B);
                }
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
    if (!gametime) {
        gametime = GetTickCount();
    }
    D2::Types::UnitAny* me = D2::PlayerUnit[0];

    if (me && me->pPath && me->pPath->pRoom1 && me->pPath->pRoom1->pRoom2 && me->pPath->pRoom1->pRoom2->pLevel) {
        DWORD levelno = me->pPath->pRoom1->pRoom2->pLevel->dwLevelNo;
        if (levelno != currentLevel) {
            currentLevel = levelno;
            revealDelay = 20;
        }

        if (revealDelay-- == 1) {
            RevealCurrentLevel();
        }
    }

    // This is where the hard logic should be performed. It's outside the render loop, but before the throttle sync,
    // so idle time can be used to do more complex tasks.
}

void gameDrawAutoMapInfo() {
    DWORD width = 0, height = 0, fileno = 1;
    height = D2::GetTextSize(L"test", &width, &fileno);
    DWORD bottom = *D2::DrawAutoMapStatsOffsetY - height;
    for (AutomapInfoCallback func : AutomapInfoHooks) {
        std::wstring msg = func();
        bottom += D2::GetTextSize(msg.c_str(), &width, &fileno);
        D2::DrawGameText(msg.c_str(), D2::ScreenWidth - 16 - width, bottom, 4, 0);
    }
}

void oogPostDraw() {
    drawBranding(false);
    //D2::DrawRectangle(400, 300, 500, 400, 131, 0xff);
}

void oogLoop() {
    currentLevel = 0;
    gametime = 0;
    // Out of game logic goes here.
}

BOOL __fastcall chatInput(wchar_t* wMsg) {
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

struct DeferredPatch {
    int size;
    unsigned long long value;
};

void init(std::vector<LPWSTR> argv, DllMainArgs dllargs) {
    // override the entire sleepy section - 32 bytes long
    MemoryPatch(D2::GameLoopPatch)
        << CALL(_gameLoop)
        << CALL(_throttle)
        << BYTES(ASM::NOP, 22);

    // override the entire sleepy section - 23 bytes long
    MemoryPatch(D2::oogLoopPatch)
        << CALL(_oogLoop)
        << CALL(_throttle)
        << BYTES(ASM::NOP, 13);

    MemoryPatch(D2::PreDrawUnitsPatch) << CALL(_preDrawUnitsPatch); // Hook the unit draw
    MemoryPatch(D2::DrawWorldEndPatch) << JUMP(gameUnitPostDraw);
    MemoryPatch(D2::GameAutomapDrawPatch) << CALL(_gameAutomapDraw); // Hook the automap draw
    MemoryPatch(D2::GameDrawPatch) << CALL(_gameDraw); // Hook the game draw
    MemoryPatch(D2::oogDrawPatch) << CALL(_oogDraw); // Hook the oog draw
    MemoryPatch(D2::MultiPatch) << CALL(multi) << ASM::NOP; // Allow multiple windows open
    MemoryPatch(D2::ChatInputPatch) << CALL(_chatInput); // Intercept game input
    MemoryPatch(D2::NullDebugPrintf) << JUMP(printf_newline); // Enable even more console debug prints
    MemoryPatch(D2::ShakePatch) << ASM::RET; // Ignore shaking requests
    MemoryPatch(D2::DisableBattleNetPatch) << ASM::RET; // Prevent battle.net connections
    MemoryPatch(D2::DrawNoFloorPatch) << CALL(_drawFloor);
    MemoryPatch(D2::DrawAutoMapInfo) << CALL(_drawAutoMapInfo);

    *D2::NoPickUp = true;

    ChatInputCallbacks[L"/toggle"] = [](std::wstring cmd, InputStream wchat) -> BOOL {
        std::wstring param;
        wchat >> param;

        if (wchat) {
            if (param == L"debug") {
                debugMode = !debugMode;
                MemoryPatch(D2::EnableDebugPrint) << debugMode; // Enable in-game debug prints
                if (debugMode) {
                    game.color(2) << "Debugging on." << std::endl;
                }
                else {
                    game.color(1) << "Debugging off." << std::endl;
                }
                return FALSE;
            }
            else if (param == L"swatch") {
                drawSwatch = !drawSwatch;
                if (drawSwatch) {
                    game.color(2) << "Swatch on." << std::endl;
                }
                else {
                    game.color(1) << "Swatch off." << std::endl;
                }
                return FALSE;
            }
        }

        game.color(3) << "Usage: " << cmd << " flag" << std::endl
            << "Example: " << cmd << " debug" << std::endl
            << "Available flags: debug, swatch" << std::endl;

        return FALSE;
    };

    ChatInputCallbacks[L"/patch"] = [](std::wstring cmd, InputStream wchat) -> BOOL {
        DWORD address;
        int size;
        unsigned long long value;
        std::vector<DeferredPatch> data;
        std::wstring possible;
        wchat >> hex >> address;

        if (wchat) {
            wchat >> possible;
            if (!wchat) {
                game.color(3) << "No data specified." << std::endl;
                goto usage;
            }

            do {
                size = possible.size() >> 1;
                if (size == 1 || size == 2 || size == 4 || size == 8) {
                    std::wstringstream hexread(possible);
                    hexread >> hex >> value;
                    if (hexread) {
                        data.push_back({ size, value });
                    }
                    else {
                        game.color(3) << "Data must be hex formatted and byte aligned (2, 4, 8, 16 long)!" << std::endl;
                        goto usage;
                    }
                }
                else {
                    game.color(3) << "Data must be hex formatted and byte aligned (2, 4, 8, 16 long)!" << std::endl;
                    goto usage;
                }
                wchat >> possible;
            } while (wchat);

            MemoryPatch patch(address);
            for (DeferredPatch patchdata : data) {
                switch (patchdata.size) {
                case 1:
                    patch << (char)patchdata.value;
                    break;
                case 2:
                    patch << (short)patchdata.value;
                    break;
                case 4:
                    patch << (long)patchdata.value;
                    break;
                case 8:
                    patch << (long long)patchdata.value;
                    break;
                default:
                    game.color(3) << "Nishi, your code is stupid. Please write it correctly." << std::endl;
                    return FALSE;
                }
            }

            return FALSE;
        }

        usage:

        game.color(3) << "Example: " << cmd << " 7BB3A4 00000001" << std::endl
            << "Usage: " << cmd << " address data [data ...]" << std::endl;

        return FALSE;
    };

    AutomapInfoHooks.push_back([]() -> std::wstring {
        return version;
    });

    AutomapInfoHooks.push_back([]() -> std::wstring {
        DWORD elapsed = GetTickCount() - gametime, seconds = (elapsed / 1000) % 60, minutes = (elapsed / 60000) % 60;
        wchar_t msg[16];
        swprintf_s(msg, L"%d:%02d", minutes, seconds);
        return msg;
    });

    game.color(3) << "Charon loaded. Available commands:" << std::endl << std::endl;

    for (const InputCallbackPair& kv : ChatInputCallbacks) {
        game.color(3) << "  " << kv.first << std::endl;
    }

    game.color(3) << std::endl;
}
