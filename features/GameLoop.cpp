/**
 * Enables throttle and game loop hooks.
 */

#include "headers/feature.h"
#include "headers/common.h"
#include "headers/hook.h"
#include <iostream>

void _gameLoop() {
    for (Feature* f = Features; f; f = f->next) {
        f->gameLoop();
    }
}

void _oogLoop() {
    // Out of game logic goes here.
    for (Feature* f = Features; f; f = f->next) {
        f->oogLoop();
    }
}

// This feature class registers itself.
class : public Feature {
public:
    void init() {
        // override the entire sleepy section - 32 bytes long
        MemoryPatch(0x451C2A)
            << CALL(_gameLoop)
            << BYTES(ASM::NOP, 2);

        // override the entire sleepy section - 23 bytes long
        MemoryPatch(0x4FA663)
            << CALL(_oogLoop)
            << BYTES(ASM::NOP, 18);

    }
} feature;
