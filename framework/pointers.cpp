/**
 * Define game pointers.
 */
#include "../headers/diablo2/D2Structs.h"

namespace Offset {
    DWORD Base, D2CLIENT, D2COMMON, D2GAME, D2LANG, D2NET, D2MULTI, D2LAUNCH, D2WIN, D2GFX, D2CMP, BNCLIENT, STORM;

    void DefineOffsets() {
        Base = D2CLIENT = D2COMMON = D2GAME = D2LANG = D2NET = D2MULTI = D2LAUNCH = D2WIN = D2GFX = D2CMP = BNCLIENT = STORM = (DWORD)GetModuleHandle(NULL);
    }
}

namespace D2 {
#define FUNCPTR(m, n, r, a, o) typedef r n##_##t a; n##_##t* n;
#define VARPTR(m, n, t, o) t* n;
#define ASMPTR(m, n, o) DWORD n;
#include "../headers/diablo2/pointerdefs.h"
#undef FUNCPTR
#undef VARPTR
#undef ASMPTR
    void DefinePointers() {
#define FUNCPTR(m, n, r, a, o) n = (n##_##t*)(Offset##::##m + o);
#define VARPTR(m, n, t, o) n = (t*)(Offset##::##m + o);
#define ASMPTR(m, n, o) n = Offset##::##m + o;
#include "../headers/diablo2/pointerdefs.h"
    }
}
