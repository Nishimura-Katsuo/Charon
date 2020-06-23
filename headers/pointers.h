/**
 * Declare game pointers.
 */
#pragma once

#include "D2Structs.h"

namespace D2 {
#define FUNCPTR(m, n, r, a, o) typedef r n##_##t a; extern n##_##t* n;
#define VARPTR(m, n, t, o) extern t* n;
#define ASMPTR(m, n, o) extern DWORD n;
#include "pointerdefs.h"
    void DefinePointers();
}
