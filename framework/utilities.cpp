#include "../headers/common.h"
#include "../headers/diablo2/utilities.h"
#include <iostream>
#include <cmath>

using std::cout;
using std::endl;

DPOINT xvector = { 16.0, 8.0 }, yvector = { -16.0, 8.0 };

void DrawLine(POINT a, POINT b, DWORD dwColor) {
    D2::DrawLine(a.x, a.y, b.x, b.y, dwColor, 0xFF);
}

POINT WorldToScreen(DPOINT pos) {
    POINT ret{
        (long)(pos.x * xvector.x + pos.y * yvector.x) - D2::GetMouseXOffset(),
        (long)(pos.x * xvector.y + pos.y * yvector.y) - D2::GetMouseYOffset()
    };
    return ret;
}

POINT WorldToScreen(D2::Types::Path *path, DPOINT adjust) {
    return WorldToScreen({ (double)path->xPos + adjust.x + (double)path->xOffset / 65536.0, (double)path->yPos + adjust.y + (double)path->yOffset / 65536.0 });
}

POINT WorldToAutomap(DPOINT pos) {
    POINT ret{
        (long)((pos.x * xvector.x + pos.y * yvector.x) / (double)*D2::Divisor) - D2::Offset->x + 8,
        (long)((pos.x * xvector.y + pos.y * yvector.y) / (double)*D2::Divisor) - D2::Offset->y - 8
    };
    return ret;
}

POINT WorldToAutomap(D2::Types::Path *path, DPOINT adjust) {
    return WorldToAutomap({ (double)path->xPos + adjust.x + (double)path->xOffset / 65536.0, (double)path->yPos + adjust.y + (double)path->yOffset / 65536.0 });
}

void DrawDot(POINT pos, DWORD dwColor) {
    D2::DrawLine(pos.x - 1, pos.y, pos.x + 1, pos.y, dwColor, 0xFF);
    D2::DrawLine(pos.x, pos.y - 1, pos.x, pos.y + 1, dwColor, 0xFF);
}

void DrawAutomapX(D2::Types::Path *arg, DWORD dwColor, double size) {
    POINT a = WorldToAutomap(arg, { -size, 0 }), b = WorldToAutomap(arg, { size, 0 });
    D2::DrawLine(a.x, a.y, b.x, b.y, dwColor, 0xFF);
    a = WorldToAutomap(arg, { 0, -size }), b = WorldToAutomap(arg, { 0, size });
    D2::DrawLine(a.x, a.y, b.x, b.y, dwColor, 0xFF);
}

void DrawWorldX(D2::Types::Path* arg, DWORD dwColor, double size) {
    POINT a = WorldToScreen(arg, { -size, 0 }), b = WorldToScreen(arg, { size, 0 });
    D2::DrawLine(a.x, a.y, b.x, b.y, dwColor, 0xFF);
    a = WorldToScreen(arg, { 0, -size }), b = WorldToScreen(arg, { 0, size });
    D2::DrawLine(a.x, a.y, b.x, b.y, dwColor, 0xFF);
}

DWORD unitHP(D2::Types::UnitAny* unit) {
    return D2::GetUnitStat(unit, 6, 0) >> 8;
}

bool isFriendly(D2::Types::UnitAny* unit) {
    return D2::GetUnitStat(unit, 172, 0) == 2;
}

bool isHostile(D2::Types::UnitAny* unit) {
    return D2::GetUnitStat(unit, 172, 0) == 0;
}

bool isAttackable(D2::Types::UnitAny* unit) {
    return unit->dwFlags & 0x4;
}

bool isEnemy(D2::Types::UnitAny* unit) {
    return unitHP(unit) > 0 && isHostile(unit) && isAttackable(unit);
}

void __fastcall CustomDebugPrint(DWORD unk, char* szMsg, DWORD color) {
    std::cout << (LPVOID)unk << " " << szMsg;
}

void RevealRoom(D2::Types::Room2* room2) {

    if (room2->pLevel) {
        // Get room
        D2::Types::Level* level = room2->pLevel;
        DWORD dwLevelNo = level->dwLevelNo;
        bool added = false;
        bool initOtherArea = false;

        // If level or room1 isnt loaded
        if (!room2->pLevel || !room2->pRoom1) {
            D2::AddRoomData(room2->pLevel->pMisc->pAct, room2->pLevel->dwLevelNo, room2->dwPosX, room2->dwPosY, NULL);
            added = true;
        }

        // safety check to see if its loaded now
        if (!room2->pRoom1 || !room2->pLevel) {
            return;
        }

        // Init the automap layer of those areas we are not at ourselfs
        if (room2->pLevel->dwLevelNo && dwLevelNo != room2->pLevel->dwLevelNo) {
            D2::InitAutomapLayer_I(D2::GetLayer(room2->pLevel->dwLevelNo)->nLayerNo);
            initOtherArea = true;
        }

        D2::RevealAutomapRoom(room2->pRoom1, TRUE, *D2::AutomapLayer);

        // If we added a room to reveal it, clean up
        if (added) {
            D2::RemoveRoomData(room2->pLevel->pMisc->pAct, room2->pLevel->dwLevelNo, room2->dwPosX, room2->dwPosY, NULL);
        }

        // If we had to init another area, init our current area
        if (initOtherArea) {
            D2::InitAutomapLayer_I(D2::GetLayer(dwLevelNo)->nLayerNo);
        }
    }
}

D2::Types::Room2* GetRoomTileOtherRoom2(D2::Types::Room2* room2, DWORD roomtileno) {
    D2::Types::RoomTile* roomtile = room2->pRoomTiles;
    while (roomtile) {
        if (*roomtile->nNum == roomtileno) {
            return roomtile->pRoom2;
        }
        roomtile = roomtile->pNext;
    }
    return 0;
}

void RevealCurrentLevel() {
    // Only when we exists we can load stuff
    D2::Types::UnitAny* player = D2::GetPlayerUnit();
    if (player == nullptr) return;

    // Your not in any level?
    D2::Types::Level* level = player->pPath->pRoom1->pRoom2->pLevel;
    if (level == nullptr) return;

    for (D2::Types::Room2* room = player->pPath->pRoom1->pRoom2->pLevel->pRoom2First; room; room = room->pRoom2Next) {
        RevealRoom(room);
    }
}
