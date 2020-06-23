/**
 * Charon - For the finer life in Diablo 2.
 */

#include "../headers/common.h"
#include "../headers/pointers.h"
#include "../headers/feature.h"
#include <shellapi.h>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        Offset::DefineOffsets();
        D2::DefinePointers();
        for (Feature* f = Features; f; f = f->next) {
            f->init();
        }

        gamelog(3) << "Charon loaded." << std::endl;
        break;
    case DLL_PROCESS_DETACH:
        for (Feature* f = Features; f; f = f->next) {
            f->deinit();
        }
        break;
    }
    return TRUE;
}
