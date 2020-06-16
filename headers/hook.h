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

class BYTES {
    BYTE value;
    size_t length;
public:
    BYTES(BYTE value, size_t length);
    friend class GamePatch;
};

class OFFSET {
    size_t length;
public:
    OFFSET(size_t length);
    friend class GamePatch;
};

class CALL {
    LPVOID pFunc;
public:
    CALL(LPVOID pFunc);
    friend class GamePatch;
};

class JUMP {
    LPVOID pFunc;
public:
    JUMP(LPVOID pFunc);
    friend class GamePatch;
};

class GamePatch {
    DWORD pAddr = NULL;

    template <class T> GamePatch& d(const T data);

public:
    GamePatch(DWORD dwAddr);
    GamePatch& operator << (const bool data);
    GamePatch& operator << (const char data);
    GamePatch& operator << (const wchar_t data);
    GamePatch& operator << (const unsigned char data);
    GamePatch& operator << (const short data);
    GamePatch& operator << (const unsigned short data);
    GamePatch& operator << (const int data);
    GamePatch& operator << (const unsigned int data);
    GamePatch& operator << (const long data);
    GamePatch& operator << (const unsigned long data);
    GamePatch& operator << (const long long data);
    GamePatch& operator << (const unsigned long long data);
    GamePatch& operator << (const float data);
    GamePatch& operator << (const double data);
    GamePatch& operator << (const BYTES bytes);
    GamePatch& operator << (const OFFSET offset);
    GamePatch& operator << (const CALL call);
};
