/**
 * Declare game pointers.
 */
#pragma once

#include "headers/diablo2/D2Structs.h"

namespace Offset {
    extern DWORD Base;
    void DefineOffsets();
}

namespace D2 {
#define FUNCPTR(m, n, r, a, o) typedef r n##_##t a; extern n##_##t* n;
#define VARPTR(m, n, t, o) extern t* n;
#define ASMPTR(m, n, o) extern DWORD n;
#include "headers/diablo2/pointerdefs.h"
    void DefinePointers();
}
