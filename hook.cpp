#include "headers/hook.h"
#include <iostream>


DWORD calcRelAddr(DWORD instructionAddress, DWORD targetAddress, DWORD instructionLength) {
    return targetAddress - instructionAddress - instructionLength;
}

template <class T>
DWORD SetData(DWORD pAddr, const T& value) {
    DWORD dwOld, dwSize = sizeof(T);

    if (VirtualProtect((LPVOID)pAddr, dwSize, PAGE_READWRITE, &dwOld)) {
        T* addr = (T*)pAddr;
        *addr = value;
        VirtualProtect((LPVOID)pAddr, dwSize, dwOld, &dwOld);
        return dwSize;
    }

    return FALSE;
}

DWORD PatchCall(BYTE instruction, DWORD pAddr, DWORD pFunc) {
    DWORD dwOld, dwLen = sizeof(BYTE) + sizeof(DWORD);

    if (VirtualProtect((LPVOID)pAddr, dwLen, PAGE_READWRITE, &dwOld)) {
        BYTE* bytes = (BYTE*)pAddr;
        bytes[0] = instruction;
        *(DWORD*)(bytes + 1) = calcRelAddr(pAddr, pFunc, dwLen);
        VirtualProtect((LPVOID)pAddr, dwLen, dwOld, &dwOld);
        return dwLen;
    }

    return FALSE;
}

DWORD SetBytes(DWORD pAddr, BYTE value, DWORD dwLen) {
    DWORD dwOld;

    if (VirtualProtect((LPVOID)pAddr, dwLen, PAGE_READWRITE, &dwOld)) {
        std::memset((LPVOID)pAddr, value, dwLen);
        VirtualProtect((LPVOID)pAddr, dwLen, dwOld, &dwOld);
        return dwLen;
    }

    return FALSE;
}

BYTES::BYTES(BYTE value, size_t length) {
    this->value = value;
    this->length = length;
}

SKIP::SKIP(size_t length) {
    this->length = length;
}

REWIND::REWIND(size_t length) {
    this->length = length;
}

CALL::CALL(LPVOID pFunc) {
    this->pFunc = pFunc;
}

JUMP::JUMP(LPVOID pFunc) {
    this->pFunc = pFunc;
}

template <class T>
MemoryPatch& MemoryPatch::d(const T data) {
    pAddr += SetData(pAddr, data);
    return *this;
}

MemoryPatch::MemoryPatch(DWORD dwAddr) {
    pAddr = dwAddr;
}

MemoryPatch& MemoryPatch::operator << (const bool data) { return d<bool>(data); }
MemoryPatch& MemoryPatch::operator << (const char data) { return d<char>(data); }
MemoryPatch& MemoryPatch::operator << (const wchar_t data) { return d<wchar_t>(data); }
MemoryPatch& MemoryPatch::operator << (const unsigned char data) { return d<unsigned char>(data); }
MemoryPatch& MemoryPatch::operator << (const short data) { return d<short>(data); }
MemoryPatch& MemoryPatch::operator << (const unsigned short data) { return d<unsigned short>(data); }
MemoryPatch& MemoryPatch::operator << (const int data) { return d<int>(data); }
MemoryPatch& MemoryPatch::operator << (const unsigned int data) { return d<unsigned int>(data); }
MemoryPatch& MemoryPatch::operator << (const long data) { return d<long>(data); }
MemoryPatch& MemoryPatch::operator << (const unsigned long data) { return d<unsigned long>(data); }
MemoryPatch& MemoryPatch::operator << (const long long data) { return d<long long>(data); }
MemoryPatch& MemoryPatch::operator << (const unsigned long long data) { return d<unsigned long long>(data); }
MemoryPatch& MemoryPatch::operator << (const float data) { return d<float>(data); }
MemoryPatch& MemoryPatch::operator << (const double data) { return d<double>(data); }

MemoryPatch& MemoryPatch::operator << (const BYTES bytes) {
    pAddr += SetBytes(pAddr, bytes.value, bytes.length);
    return *this;
}

MemoryPatch& MemoryPatch::operator << (const SKIP offset) {
    pAddr += offset.length;
    return *this;
}

MemoryPatch& MemoryPatch::operator << (const REWIND offset) {
    pAddr -= offset.length;
    return *this;
}

MemoryPatch& MemoryPatch::operator << (const CALL call) {
    pAddr += PatchCall(ASM::CALL, pAddr, (DWORD)call.pFunc);
    return *this;
}

MemoryPatch& MemoryPatch::operator << (const JUMP jump) {
    pAddr += PatchCall(ASM::JMP, pAddr, (DWORD)jump.pFunc);
    return *this;
}

MemoryPatch& MemoryPatch::operator << (BYTESEQ bytes) {
    DWORD dwOld, dwSize = bytes.size();

    if (VirtualProtect((LPVOID)pAddr, dwSize, PAGE_READWRITE, &dwOld)) {
        BYTE* addr = (BYTE*)pAddr;
        for (size_t c = 0; c < dwSize; c++) {
            addr[c] = bytes[c];
        }
        VirtualProtect((LPVOID)pAddr, dwSize, dwOld, &dwOld);
        pAddr += dwSize;
    }

    return *this;
}
