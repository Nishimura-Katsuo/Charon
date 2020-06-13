#pragma once

#include <cstring>

#define ASM_NOP 0x90
#define ASM_CALL 0xE8
#define ASM_JMP 0xE9

template <BYTE instruction, DWORD dwLen>
BOOL PatchFuncRef(DWORD pAddr, LPVOID pFunc) {
    DWORD dwOld, dwFunc = (DWORD)pFunc - (pAddr + 5);
    BYTE bytes[dwLen];

    std::memset(bytes, ASM_NOP, dwLen);
    bytes[0] = instruction;
    *(DWORD*)&bytes[1] = dwFunc;

    if (VirtualProtect((LPVOID)pAddr, dwLen, PAGE_READWRITE, &dwOld)) {
        std::memcpy((LPVOID)pAddr, bytes, dwLen);
        return VirtualProtect((LPVOID)pAddr, dwLen, dwOld, &dwOld);
    }

    return FALSE;
}

template <DWORD dwLen> BOOL PatchCall(DWORD pAddr, LPVOID pFunc) { return PatchFuncRef<ASM_CALL, dwLen>(pAddr, pFunc); }
template <DWORD dwLen> BOOL PatchJump(DWORD pAddr, LPVOID pFunc) { return PatchFuncRef<ASM_JMP, dwLen>(pAddr, pFunc); }
