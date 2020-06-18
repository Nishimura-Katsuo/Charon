#pragma once

void WorldToScreen(long* x, long* y);
POINT WorldToScreen(POINT point);
void ScreenToWorld(long* x, long* y);
void WorldToAutomap(long* x, long* y);
POINT WorldToAutomap(POINT point);
void DrawMinimapLine(long x1, long y1, long x2, long y2, DWORD dwColor = 0x62, DWORD dwOpacity = 0xFF);
void DrawMinimapDot(long x1, long y1, DWORD dwColor = 0x62, DWORD dwOpacity = 0xFF);
void DrawMinimapX(long x, long y, DWORD dwColor = 0x62, DWORD dwOpacity = 0xFF, int size = 5);
void DrawAutomapLine(POINT p1, POINT p2, DWORD dwColor, DWORD dwOpacity = 0xFF);
void DrawWorldLine(POINT p1, POINT p2, DWORD dwColor, DWORD dwOpacity = 0xFF);
void DrawScreenLine(POINT p1, POINT p2, DWORD dwColor, DWORD dwOpacity = 0xFF);
DWORD unitHP(D2::Types::UnitAny* unit);
bool isFriendly(D2::Types::UnitAny* unit);
bool isHostile(D2::Types::UnitAny* unit);
bool isAttackable(D2::Types::UnitAny* unit);
bool isEnemy(D2::Types::UnitAny* unit);
POINT unitScreenPos(D2::Types::UnitAny* unit);
void __fastcall CustomDebugPrint(DWORD unk, char* szMsg, DWORD color);
void RevealCurrentLevel();