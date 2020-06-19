#include "../headers/common.h"
#include <iostream>

using std::cout;
using std::endl;

// We can use this to mark objects directly on the screen (could probably simulate item beams like diablo 3)
void WorldToScreen(long* x, long* y) {
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

void DrawMinimapLine(long x1, long y1, long x2, long y2, DWORD dwColor, DWORD dwOpacity) {
    WorldToAutomap(&x1, &y1);
    WorldToAutomap(&x2, &y2);
    D2::DrawLine(x1, y1, x2, y2, dwColor, dwOpacity);
}

void DrawMinimapDot(long x1, long y1, DWORD dwColor, DWORD dwOpacity) {
    WorldToAutomap(&x1, &y1);
    D2::DrawLine(x1 - 1, y1, x1 + 1, y1, dwColor, dwOpacity);
    D2::DrawLine(x1, y1 - 1, x1, y1 + 1, dwColor, dwOpacity);
}

void DrawMinimapX(long x, long y, DWORD dwColor, DWORD dwOpacity, int size) {
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

bool isHostile(D2::Types::UnitAny* unit) {
    return D2::GetUnitStat(unit, 172, 0) == 0;
}

bool isAttackable(D2::Types::UnitAny* unit) {
    return unit->dwFlags & 0x4;
}

bool isEnemy(D2::Types::UnitAny* unit) {
    return unitHP(unit) > 0 && isHostile(unit) && isAttackable(unit);
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

void __fastcall CustomDebugPrint(DWORD unk, char* szMsg, DWORD color) {
    std::cout << (LPVOID)unk << " " << szMsg;
}

void RevealRoom(D2::Types::Room2* room2) {

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

D2::Types::Room2* GetRoomTileOtherRoom2(D2::Types::Room2* room2, DWORD roomtileno)
{
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
    //cout << "This feature is not implemented yet." << endl;
    
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
