#define _USE_MATH_DEFINES

#include "../headers/common.h"
#include "../headers/diablo2/utilities.h"
#include <iostream>
#include <cmath>

DPOINT xvector = { 16.0, 8.0 }, yvector = { -16.0, 8.0 };
std::map<int, std::vector<FoundExit>> RevealedExits;

GameChat game;

int GameChatBuffer::sync() {
    if (!inGame) {
        std::wcout << this->str();
        this->str(L"");
    }
    else {
        std::wstring::size_type start = 0;
        std::wstring::size_type pos = this->str().find(L"\n");
        while (pos != std::wstring::npos) {
            std::wstring tmp = this->str().substr(start, pos - start);
            D2::PrintGameString(tmp.c_str(), color);
            start = pos + 1;
            pos = this->str().find(L"\n", start);
        }
        if (start > 0) {
            this->str(this->str().substr(start));
        }
    }
    return 0;
}

GameChat::GameChat() : std::wostream(&buf) { }

GameChat& GameChat::color(DWORD color) {
    buf.color = color;
    return *this;
}

void DrawLine(POINT a, POINT b, DWORD dwColor) {
    if (
        a.x >= 0 && a.x < D2::ScreenWidth ||
        b.x >= 0 && b.x < D2::ScreenWidth ||
        a.y >= 0 && a.y < D2::ScreenHeight ||
        b.y >= 0 && b.y < D2::ScreenHeight
    ) {
        D2::DrawLine(a.x, a.y, b.x, b.y, dwColor, 0xFF);
    }
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

void DrawAutomapX(DPOINT arg, DWORD dwColor, double size) {
    POINT a = WorldToAutomap({ (double)arg.x - size, (double)arg.y }), b = WorldToAutomap({ (double)arg.x + size, (double)arg.y });
    DrawLine({ a.x, a.y }, { b.x, b.y }, dwColor);
    a = WorldToAutomap({ (double)arg.x, (double)arg.y - size }), b = WorldToAutomap({ (double)arg.x, (double)arg.y + size });
    DrawLine({ a.x, a.y }, { b.x, b.y }, dwColor);
}

void DrawWorldX(DPOINT arg, DWORD dwColor, double size) {
    POINT a = WorldToScreen({ (double)arg.x - size, (double)arg.y }), b = WorldToScreen({ (double)arg.x + size, (double)arg.y });
    DrawLine({ a.x, a.y }, { b.x, b.y }, dwColor);
    a = WorldToScreen({ (double)arg.x, (double)arg.y - size }), b = WorldToScreen({ (double)arg.x, (double)arg.y + size });
    DrawLine({ a.x, a.y }, { b.x, b.y }, dwColor);
}

void DrawAutomapX(D2::Types::ItemPath* arg, DWORD dwColor, double size) {
    POINT a = WorldToAutomap({ (double)arg->dwPosX - size, (double)arg->dwPosY }), b = WorldToAutomap({ (double)arg->dwPosX + size, (double)arg->dwPosY });
    DrawLine({ a.x, a.y }, { b.x, b.y }, dwColor);
    a = WorldToAutomap({ (double)arg->dwPosX, (double)arg->dwPosY - size }), b = WorldToAutomap({ (double)arg->dwPosX, (double)arg->dwPosY + size });
    DrawLine({ a.x, a.y }, { b.x, b.y }, dwColor);
}

void DrawWorldX(D2::Types::ItemPath* arg, DWORD dwColor, double size) {
    POINT a = WorldToScreen({ (double)arg->dwPosX - size, (double)arg->dwPosY }), b = WorldToScreen({ (double)arg->dwPosX + size, (double)arg->dwPosY });
    DrawLine({ a.x, a.y }, { b.x, b.y }, dwColor);
    a = WorldToScreen({ (double)arg->dwPosX, (double)arg->dwPosY - size }), b = WorldToScreen({ (double)arg->dwPosX, (double)arg->dwPosY + size });
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

template <int len>
void DrawAutomapShape(DPOINT points[len], DWORD dwColor) {
    POINT pos[2];

    pos[0] = WorldToAutomap(points[len - 1]);
    pos[1] = WorldToAutomap(points[0]);
    DrawLine(pos[0], pos[1], dwColor);

    for (int c = 1; c < len; c++) {
        pos[0] = pos[1];
        pos[1] = WorldToAutomap(points[c]);
        DrawLine(pos[0], pos[1], dwColor);
    }
}

template <int len>
void DrawWorldShape(DPOINT points[len], DWORD dwColor) {
    POINT pos[2];

    pos[0] = WorldToScreen(points[len - 1]);
    pos[1] = WorldToScreen(points[0]);
    DrawLine(pos[0], pos[1], dwColor);

    for (int c = 1; c < len; c++) {
        pos[0] = pos[1];
        pos[1] = WorldToScreen(points[c]);
        DrawLine(pos[0], pos[1], dwColor);
    }
}

void DrawAutomapRadialShape(DPOINT center, int radius, int sides, DWORD dwColor, double angle) {
    POINT pos[2];
    double interval = M_PI * 2 / sides, i = angle;

    pos[0] = WorldToAutomap({ center.x + cos(i) * radius, center.y + sin(i) * radius });
    i += interval;
    pos[1] = WorldToAutomap({ center.x + cos(i) * radius, center.y + sin(i) * radius });
    i += interval;
    DrawLine(pos[0], pos[1], dwColor);

    for (int c = 1; c < sides; c++, i += interval) {
        pos[0] = pos[1];
        pos[1] = WorldToAutomap({ center.x + cos(i) * radius, center.y + sin(i) * radius });
        DrawLine(pos[0], pos[1], dwColor);
    }
}

void DrawAutomapRadialShape(DPOINT center, int radius, int sides, DWORD dwColor, DPOINT target) {
    DrawAutomapRadialShape(center, radius, sides, dwColor, atan2(target.y - center.y, target.x - center.x));
}

void DrawWorldRadialShape(DPOINT center, int radius, int sides, DWORD dwColor, double angle) {
    POINT pos[2];
    double interval = M_PI * 2 / sides, i = angle;

    pos[0] = WorldToScreen({ center.x + cos(i) * radius, center.y + sin(i) * radius });
    i += interval;
    pos[1] = WorldToScreen({ center.x + cos(i) * radius, center.y + sin(i) * radius });
    i += interval;
    DrawLine(pos[0], pos[1], dwColor);

    for (int c = 1; c < sides; c++, i += interval) {
        pos[0] = pos[1];
        pos[1] = WorldToScreen({ center.x + cos(i) * radius, center.y + sin(i) * radius });
        DrawLine(pos[0], pos[1], dwColor);
    }
}

void DrawWorldRadialShape(DPOINT center, int radius, int sides, DWORD dwColor, DPOINT target) {
    DrawWorldRadialShape(center, radius, sides, dwColor, atan2(target.y - center.y, target.x - center.x));
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

void CheckExits(D2::Types::Room2* current) {
    FoundExit exit;
    int x1 = 0, y1 = 0;
    int x2 = 0, y2 = 0;
 
    for (unsigned int c = 0; c < current->dwRoomsNear; c++) {
        if (current->pLevel->dwLevelNo != current->pRoom2Near[c]->pLevel->dwLevelNo && (current->dwPosX == current->pRoom2Near[c]->dwPosX || current->dwPosY == current->pRoom2Near[c]->dwPosY)) {
            exit.origin = { ((double)current->dwPosX + (double)current->dwSizeX / 2) * 5, ((double)current->dwPosY + (double)current->dwSizeY / 2) * 5 };
            exit.target = { ((double)current->pRoom2Near[c]->dwPosX + (double)current->pRoom2Near[c]->dwSizeX / 2) * 5, ((double)current->pRoom2Near[c]->dwPosY + (double)current->pRoom2Near[c]->dwSizeY / 2) * 5 };
            RevealedExits[current->pLevel->dwLevelNo].push_back(exit);
        }
    }
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
