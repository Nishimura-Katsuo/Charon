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
    const BYTE RET = 0xC3;
}

class BYTES {
    BYTE value;
    size_t length;
public:
    BYTES(BYTE value, size_t length);
    friend class MemoryPatch;
};

typedef std::vector<BYTE> BYTESEQ;

class SKIP {
    size_t length;
public:
    SKIP(size_t length);
    friend class MemoryPatch;
};

class REWIND {
    size_t length;
public:
    REWIND(size_t length);
    friend class MemoryPatch;
};

class CALL {
    LPVOID pFunc;
public:
    CALL(LPVOID pFunc);
    friend class MemoryPatch;
};

class JUMP {
    LPVOID pFunc;
public:
    JUMP(LPVOID pFunc);
    friend class MemoryPatch;
};

class MemoryPatch {
    DWORD pAddr = NULL;

    template <class T> MemoryPatch& d(const T data);

public:
    MemoryPatch(DWORD dwAddr);
    MemoryPatch& operator << (const bool data);
    MemoryPatch& operator << (const char data);
    MemoryPatch& operator << (const wchar_t data);
    MemoryPatch& operator << (const unsigned char data);
    MemoryPatch& operator << (const short data);
    MemoryPatch& operator << (const unsigned short data);
    MemoryPatch& operator << (const int data);
    MemoryPatch& operator << (const unsigned int data);
    MemoryPatch& operator << (const long data);
    MemoryPatch& operator << (const unsigned long data);
    MemoryPatch& operator << (const long long data);
    MemoryPatch& operator << (const unsigned long long data);
    MemoryPatch& operator << (const float data);
    MemoryPatch& operator << (const double data);
    MemoryPatch& operator << (const BYTES bytes);
    MemoryPatch& operator << (const SKIP offset);
    MemoryPatch& operator << (const REWIND offset);
    MemoryPatch& operator << (const CALL call);
    MemoryPatch& operator << (const JUMP jump);
    MemoryPatch& operator << (BYTESEQ bytes);
};
