/**
 * Charon - For the finer life in Diablo 2.
 */


#define DEFINE_REMOTE_REFERENCES true

#include <windows.h>
#include "headers/common.h"
#include "headers/feature.h"
#include "headers/remote.h"
#include <shellapi.h>
#include <iostream>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        for (Feature* f = Features; f; f = f->next) {
            f->init();
        }

        gamelog << COLOR(2) << "Charon loaded." << std::endl;
        break;
    case DLL_PROCESS_DETACH:
        for (Feature* f = Features; f; f = f->next) {
            f->deinit();
        }
        break;
    }

    return TRUE;
}
