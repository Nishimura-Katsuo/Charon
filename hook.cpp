#include "headers/hook.h"
#include <iostream>

BOOL PatchCall(DWORD pAddr, LPVOID pFunc, DWORD dwExtraNopLength, BYTE instruction) {
    DWORD dwOld, dwFunc = (DWORD)pFunc - (pAddr + 5);

    if (dwExtraNopLength < 0) {
        throw "Extra NOP length cannot be less than zero!";
    }

    if (VirtualProtect((LPVOID)pAddr, 5 + dwExtraNopLength, PAGE_READWRITE, &dwOld)) {
        BYTE* bytes = (BYTE*)pAddr;
        bytes[0] = instruction;
        *(DWORD*)(bytes + 1) = dwFunc;
        std::memset(bytes + 5, ASM::NOP, dwExtraNopLength);
        return VirtualProtect((LPVOID)pAddr, 5 + dwExtraNopLength, dwOld, &dwOld);
    }

    return FALSE;
}

BOOL SetBytes(DWORD pAddr, BYTE value, DWORD dwLen) {
    DWORD dwOld;

    if (VirtualProtect((LPVOID)pAddr, dwLen, PAGE_READWRITE, &dwOld)) {
        std::memset((LPVOID)pAddr, value, dwLen);
        return VirtualProtect((LPVOID)pAddr, dwLen, dwOld, &dwOld);
    }

    return FALSE;
}
