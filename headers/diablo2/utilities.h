#pragma once

#include "D2Structs.h"

extern DPOINT xvector, yvector;
void DrawLine(POINT a, POINT b, DWORD dwColor);
POINT WorldToScreen(DPOINT pos);
POINT WorldToScreen(D2::Types::Path* path, DPOINT adjust = { 0, 0 });
POINT WorldToAutomap(DPOINT pos);
POINT WorldToAutomap(D2::Types::Path* path, DPOINT adjust = { 0, 0 });
void DrawDot(POINT pos, DWORD dwColor);
void DrawAutomapX(D2::Types::ItemPath* arg, DWORD dwColor, double size = 5);
void DrawWorldX(D2::Types::ItemPath* arg, DWORD dwColor, double size = 1);
void DrawAutomapX(D2::Types::Path* arg, DWORD dwColor, double size = 5);
void DrawWorldX(D2::Types::Path* arg, DWORD dwColor, double size = 1);
DWORD unitHP(D2::Types::UnitAny* unit);
bool isFriendly(D2::Types::UnitAny* unit);
bool isHostile(D2::Types::UnitAny* unit);
bool isAttackable(D2::Types::UnitAny* unit);
bool isEnemy(D2::Types::UnitAny* unit);
void __fastcall CustomDebugPrint(DWORD unk, char* szMsg, DWORD color);
D2::Types::Room2* GetRoomTileOtherRoom2(D2::Types::Room2* room2, DWORD roomtileno);
void RevealCurrentLevel();
