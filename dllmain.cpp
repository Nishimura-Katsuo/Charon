/**
 * Let's keep some of the messy stuff out of our main.cpp.
 */

#include "headers/common.h"
#include <shellapi.h>

void init(std::vector<LPWSTR>, DllMainArgs);

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    int argc;
    LPWSTR* argv;

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        argc = 0;
        argv = CommandLineToArgvW(GetCommandLineW(), &argc);
        Offset::DefineOffsets();
        D2::DefinePointers();
        init(std::vector<LPWSTR>(argv, argv + argc), DllMainArgs{ hModule, ul_reason_for_call, lpReserved });
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
