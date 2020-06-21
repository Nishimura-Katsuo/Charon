#pragma once

#include <windows.h>
#include <vector>
#include <functional>

namespace Offset {
    extern DWORD Base, D2CLIENT, D2COMMON, D2GAME, D2LANG, D2NET, D2MULTI, D2LAUNCH, D2WIN, D2GFX, D2CMP, BNCLIENT, STORM;
    void DefineOffsets();
}

#ifdef DEFINE_XREF
#define XREF ExternalReference
#else
#define XREF extern ExternalReference
#endif

// Use XREF to declare these in this file instead of the class itself.
// We're using XREF as a switch to declare or define these so we can
// Write it only once.
template <DWORD& base, class F, DWORD offset>
class ExternalReference {
public:
    ExternalReference() {}

#pragma warning(suppress: 4180)
    F* getAddress() {
        return (F*)(base + offset);
    }

    operator F& () {
        return *(F*)(base + offset);
    }

#pragma warning(suppress: 4180)
    F& operator= (const F& value) {
        F* tmp = (F*)(base + offset);
        *tmp = value;
        return *tmp;
    }
};

/**
 * D2 Offsets here.
 */

namespace D2 {
    XREF<Offset::Base, DWORD __stdcall (), 0xF5160> GetScreenMode;                          // Updated 1.14d //004F5160-BASE
    XREF<Offset::D2CLIENT, long __fastcall (VOID), 0x5AFC0> GetMouseXOffset;               // Updated 1.14d //0045AFC0-BASE
    XREF<Offset::D2CLIENT, long __fastcall (VOID), 0x5AFB0> GetMouseYOffset;               // Updated 1.14d //0045AFB0-BASE
    XREF<Offset::D2GFX, void __stdcall (int X1, int Y1, int X2, int Y2, DWORD dwColor, DWORD dwUnk), 0xF6300> DrawRectangle;        // Updated 1.14d //004F6300-BASE
    XREF<Offset::D2GFX, void __stdcall (int X1, int Y1, int X2, int Y2, DWORD dwColor, DWORD dwUnk), 0xF6340> DrawRectangleB;       // Updated 1.14d //004F6340-BASE
    XREF<Offset::D2GFX, void __stdcall (int X1, int Y1, int X2, int Y2, DWORD dwColor, DWORD dwUnk), 0xF6380> DrawLine;             // Updated 1.14d //004F6380-BASE
}
