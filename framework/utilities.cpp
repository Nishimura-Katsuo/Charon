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
    DrawLine({ pos.x - 1, pos.y }, { pos.x + 1, pos.y }, dwColor);
    DrawLine({ pos.x, pos.y - 1 }, { pos.x, pos.y + 1 }, dwColor);
}

void DrawAutomapX(D2::Types::ItemPath* arg, DWORD dwColor, double size) {
    POINT a = WorldToAutomap({ (double)arg->dwPosX - size, (double)arg->dwPosY }), b = WorldToAutomap({ (double)arg->dwPosX + size, (double)arg->dwPosY });
    DrawLine({ a.x, a.y }, { b.x, b.y }, dwColor);
    a = WorldToAutomap({ (double)arg->dwPosX, (double)arg->dwPosY - size }), b = WorldToAutomap({ (double)arg->dwPosX, (double)arg->dwPosY + size });
    DrawLine({ a.x, a.y }, { b.x, b.y }, dwColor);
}

void DrawWorldX(D2::Types::ItemPath* arg, DWORD dwColor, double size) {
    POINT a = WorldToScreen({ (double)arg->dwPosX - size, (double)arg->dwPosY }), b = WorldToAutomap({ (double)arg->dwPosX + size, (double)arg->dwPosY });
    DrawLine({ a.x, a.y }, { b.x, b.y }, dwColor);
    a = WorldToScreen({ (double)arg->dwPosX, (double)arg->dwPosY - size }), b = WorldToAutomap({ (double)arg->dwPosX, (double)arg->dwPosY + size });
    DrawLine({ a.x, a.y }, { b.x, b.y }, dwColor);
}

void DrawAutomapX(D2::Types::Path *arg, DWORD dwColor, double size) {
    POINT a = WorldToAutomap(arg, { -size, 0 }), b = WorldToAutomap(arg, { size, 0 });
    DrawLine({ a.x, a.y }, { b.x, b.y }, dwColor);
    a = WorldToAutomap(arg, { 0, -size }), b = WorldToAutomap(arg, { 0, size });
    DrawLine({ a.x, a.y }, { b.x, b.y }, dwColor);
}

void DrawWorldX(D2::Types::Path* arg, DWORD dwColor, double size) {
    POINT a = WorldToScreen(arg, { -size, 0 }), b = WorldToScreen(arg, { size, 0 });
    DrawLine({ a.x, a.y }, { b.x, b.y }, dwColor);
    a = WorldToScreen(arg, { 0, -size }), b = WorldToScreen(arg, { 0, size });
    DrawLine({ a.x, a.y }, { b.x, b.y }, dwColor);
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

void CheckExits(D2::Types::Room2* room) {

}

void RevealCurrentLevel() {
    D2::Types::UnitAny* me = D2::PlayerUnit[0];

    if (me) {
        D2::Types::Level* level = me->pPath->pRoom1->pRoom2->pLevel;

        if (level) {
        DWORD dwLevelNo = level->dwLevelNo;
            size_t saveExits = RevealedExits[dwLevelNo].size() < 1;

            for (D2::Types::Room2* room2 = level->pRoom2First; room2 != nullptr; room2 = room2->pRoom2Next) {
                if (room2->pLevel && room2->pLevel->pMisc && room2->pLevel->pMisc->pAct) {
                    if (room2->pRoom1 == nullptr) {
            D2::AddRoomData(room2->pLevel->pMisc->pAct, room2->pLevel->dwLevelNo, room2->dwPosX, room2->dwPosY, NULL);

                        if (room2->pRoom1) {
                            D2::RevealAutomapRoom(room2->pRoom1, TRUE, *D2::AutomapLayer);
                            if (saveExits) {
                                CheckExits(room2);
        }
            D2::RemoveRoomData(room2->pLevel->pMisc->pAct, room2->pLevel->dwLevelNo, room2->dwPosX, room2->dwPosY, NULL);
        }
        }
                    else {
                        D2::RevealAutomapRoom(room2->pRoom1, TRUE, *D2::AutomapLayer);
                        if (saveExits) {
                            CheckExits(room2);
    }
}
        }
    }
}
    }
}
