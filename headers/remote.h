#pragma once

#include "common.h"
#include "D2Structs.h"

typedef unsigned long ASMPTR;

#ifdef DEFINE_REMOTE_REFERENCES

#define GLOBALFUNC(r, n, a, o) namespace FuncDefs { typedef r n a; } FuncDefs::n *n = (FuncDefs::n*)o;
#define GLOBALREF(t, n, o) namespace VarDefs { typedef t n; } VarDefs::n &n = *(VarDefs::n *)o;
#define GLOBALPTR(t, n, o) namespace VarDefs { typedef t n; } VarDefs::n *n = (VarDefs::n *)o;

#else

#define GLOBALFUNC(r, n, a, o) namespace FuncDefs { typedef r n a; } extern FuncDefs::n *n;
#define GLOBALREF(t, n, o) namespace VarDefs { typedef t n; } extern VarDefs::n &n;
#define GLOBALPTR(t, n, o) namespace VarDefs { typedef t n; } extern VarDefs::n *n;

#endif

#pragma warning( disable : 4229 )

namespace D2 {

	// Basically, if the address represents an array (not a pointer to an array) use this
	// global pointer 'GLOBALPTR' macro, otherwise use global reference 'GLOBALREF' instead
	GLOBALPTR(Types::UnitHashTable, ClientSideUnitHashTables, 0x7A5270);
	GLOBALPTR(Types::UnitHashTable, ServerSideUnitHashTables, 0x7A5E70);
	GLOBALREF(Types::UnitAny*, PlayerUnit, 0x7A6A70);
	GLOBALREF(DWORD, NoPickUp, 0x7A6A90);

	// For referencing D2's functions use this specialized macro instead
	GLOBALFUNC(DWORD __fastcall, SetFont, (DWORD dwFileNo), 0x502EF0);
	GLOBALFUNC(void __fastcall, DrawGameText, (const wchar_t* wStr, int xPos, int yPos, DWORD dwColor, BOOL bMultiLineCenterFlag), 0x502320);
	GLOBALFUNC(DWORD __fastcall, GetTextSize, (const wchar_t* wStr, DWORD* dwWidth, DWORD* dwFileNo), 0x502520);
	GLOBALFUNC(DWORD __stdcall, GetUnitStat, (Types::UnitAny* pUnit, DWORD dwStat, DWORD dwStat2), 0x625480);
	GLOBALFUNC(int __stdcall, GetUnitState, (Types::UnitAny* pUnit, DWORD dwStateNo), 0x639DF0);
	GLOBALFUNC(wchar_t* __fastcall, GetUnitName, (Types::UnitAny* unit), 0x464a60);
	GLOBALFUNC(void __stdcall, GetScreenModeSize, (int nResolutionMode, int* pResRightMax, int* pResBottomMax), 0x4F5570);
	GLOBALFUNC(DWORD __stdcall, GetScreenMode, (), 0x4F5160);
	GLOBALFUNC(DWORD __fastcall, GetUiFlag, (DWORD dwVarNo), 0x4538D0);
	GLOBALFUNC(void __stdcall, AddRoomData, (Types::Act* ptAct, int LevelId, int Xpos, int Ypos, D2::Types::Room1* pRoom), 0x61A070);
	GLOBALFUNC(void __stdcall, RemoveRoomData, (Types::Act* ptAct, int LevelId, int Xpos, int Ypos, D2::Types::Room1* pRoom), 0x61A0C0);
	GLOBALFUNC(void __stdcall, InitLevel, (Types::Level* pLevel), 0x6424A0);
	GLOBALFUNC(Types::LevelTxt* __stdcall, GetLevelText, (DWORD levelno), 0x61DB70);
	GLOBALFUNC(Types::ObjectTxt* __stdcall, GetObjectText, (DWORD objno), 0x640E90);
	GLOBALFUNC(Types::ItemTxt* __stdcall, GetItemText, (DWORD itemno), 0x6335F0);

}

#pragma warning( default : 4229 )

#define REMOTEFUNC(r, n, a, o) namespace FuncDefs { typedef r n a; } FuncDefs::n *n = (FuncDefs::n*)o;
#define REMOTEREF(t, n, o) namespace VarDefs { typedef t n; } VarDefs::n &n = *(VarDefs::n *)o;
#define REMOTEPTR(t, n, o) namespace VarDefs { typedef t n; } VarDefs::n *n = (VarDefs::n *)o;
