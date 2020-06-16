/**
 * Utilities for hooking into the goodies.
 */
#pragma once

#include <windows.h>
#include <cstring>
#include <vector>

namespace ASM {
    const BYTE NOP = 0x90;
    const BYTE CALL = 0xE8;
    const BYTE JMP = 0xE9;
}

BOOL PatchCall(DWORD pAddr, LPVOID pFunc, DWORD dwExtraNopLength = 0, BYTE instruction = ASM::CALL);
BOOL SetBytes(DWORD pAddr, BYTE value, DWORD dwLen);

template <class T>
BOOL SetData(DWORD pAddr, std::vector<T> values) {
    DWORD dwOld, dwSize = values.size(), dwLen = sizeof(T) * dwSize;

    if (VirtualProtect((LPVOID)pAddr, dwLen, PAGE_READWRITE, &dwOld)) {
        T* addr = (T*)pAddr;
        for (size_t c = 0; c < dwSize; c++) {
            addr[c] = values[c];
        }
        return VirtualProtect((LPVOID)pAddr, dwLen, dwOld, &dwOld);
    }

    return FALSE;
}
