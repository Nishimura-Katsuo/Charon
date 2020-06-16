#include "headers/hook.h"
#include <iostream>

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

DWORD PatchCall(BYTE instruction, DWORD pAddr, LPVOID pFunc) {
    DWORD dwOld, dwFunc = (DWORD)pFunc - (pAddr + 5), dwLen = sizeof(BYTE) + sizeof(DWORD);

    if (VirtualProtect((LPVOID)pAddr, dwLen, PAGE_READWRITE, &dwOld)) {
        BYTE* bytes = (BYTE*)pAddr;
        bytes[0] = instruction;
        *(DWORD*)(bytes + 1) = dwFunc;
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
OFFSET::OFFSET(size_t length) {
    this->length = length;
}
CALL::CALL(LPVOID pFunc) {
    this->pFunc = pFunc;
}
JUMP::JUMP(LPVOID pFunc) {
    this->pFunc = pFunc;
}

template <class T>
GamePatch& GamePatch::d(const T data) {
    pAddr += SetData(pAddr, data);
    return *this;
}

GamePatch::GamePatch(DWORD dwAddr) {
    pAddr = dwAddr;
}

GamePatch& GamePatch::operator << (const bool data) { return d<bool>(data); }
GamePatch& GamePatch::operator << (const char data) { return d<char>(data); }
GamePatch& GamePatch::operator << (const wchar_t data) { return d<wchar_t>(data); }
GamePatch& GamePatch::operator << (const unsigned char data) { return d<unsigned char>(data); }
GamePatch& GamePatch::operator << (const short data) { return d<short>(data); }
GamePatch& GamePatch::operator << (const unsigned short data) { return d<unsigned short>(data); }
GamePatch& GamePatch::operator << (const int data) { return d<int>(data); }
GamePatch& GamePatch::operator << (const unsigned int data) { return d<unsigned int>(data); }
GamePatch& GamePatch::operator << (const long data) { return d<long>(data); }
GamePatch& GamePatch::operator << (const unsigned long data) { return d<unsigned long>(data); }
GamePatch& GamePatch::operator << (const long long data) { return d<long long>(data); }
GamePatch& GamePatch::operator << (const unsigned long long data) { return d<unsigned long long>(data); }
GamePatch& GamePatch::operator << (const float data) { return d<float>(data); }
GamePatch& GamePatch::operator << (const double data) { return d<double>(data); }

GamePatch& GamePatch::operator << (const BYTES bytes) {
    pAddr += SetBytes(pAddr, bytes.value, bytes.length);
    return *this;
}

GamePatch& GamePatch::operator << (const OFFSET offset) {
    pAddr += offset.length;
    return *this;
}

GamePatch& GamePatch::operator << (const CALL call) {
    pAddr += PatchCall(ASM::CALL, pAddr, call.pFunc);
    return *this;
}
