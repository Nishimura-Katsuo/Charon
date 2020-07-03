/**
 * Enables a bunch of debug enhancements.
 * It's crazy! Seriously!
 */

#include "headers/common.h"
#include "headers/feature.h"
#include "headers/hook.h"
#include "headers/remote.h"
#include <iostream>

const wchar_t* align[] = { L"Hostile", L"Neutral", L"Friendly" };

ASMPTR EnableDebugPrint = 0x8846DC;
REMOTEFUNC(void __fastcall, DrawFloor, (void* unknown), 0x4DED10);

void __fastcall _drawFloor(void* unknown) {
    if (!State["debugMode"]) {
        DrawFloor(unknown);
    }
}

// This is based on the actual source for printf... uses varargs.
int __stdcall printf_newline(const char* format, ...) {
    va_list arg;
    int done;

    va_start(arg, format);
    done = vfprintf(stdout, format, arg);
    va_end(arg);

    // We want to force a newline after debug prints :)
    puts("");

    return done;
}

// This feature class registers itself.
class : public Feature {
public:
    void toggleDebug() {
        State["debugMode"] = !State["debugMode"];

        MemoryPatch(EnableDebugPrint) << (bool)State["debugMode"]; // Enable in-game debug prints
        if (State["debugMode"]) {
            gamelog << COLOR(2) << "Debugging on." << std::endl;
        }
        else {
            gamelog << COLOR(1) << "Debugging off." << std::endl;
        }
    }

    void init() {
        State["debugMode"] = false;

        MemoryPatch(0x476CDC) << CALL(_drawFloor); // Allow disabling the floor.
        MemoryPatch(0x51A480) << JUMP(printf_newline); // Enable even more console debug prints

        ChatInputCallbacks[L"/debug"] = [&](std::wstring cmd, InputStream wchat) -> BOOL {
            toggleDebug();
            return FALSE;
        };

        HotkeyCallbacks[VK_F12] = [&](LPARAM options) -> BOOL {
            toggleDebug();
            return FALSE;
        };
    }

    void gameUnitPreDraw() {
        BYTE d;
        // Server side tracks enemies
        if (State["debugMode"]) {
            D2::Types::Room1* current = D2::PlayerUnit->pPath->pRoom1;
            D2::Types::CollMap* coll = current->Coll;
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
    }

    void gameUnitPostDraw() {
        BYTE d;

        // Server side tracks enemies
        if (State["debugMode"] && State["debugVerbose"]) {
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
} featuer;
