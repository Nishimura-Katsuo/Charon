/**
 * Enables throttle and game loop hooks.
 */

#include "headers/feature.h"
#include "headers/common.h"
#include "headers/pointers.h"
#include "headers/hook.h"
#include <iostream>
#include <chrono>
#include <thread>

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

// Keeps the game at a steady framerate without using too much CPU.
// D2 doesn't do a great job at it by default, so we're helping out.
void throttle() {
    using frameDuration = std::chrono::duration<int64_t, std::ratio<1, 25>>; // Limit the game to 25 fps always (matches OOG and single player)
    using std::chrono::system_clock;
    using std::this_thread::sleep_until;
    static system_clock::time_point nextFrame = system_clock::now(), now;

    now = system_clock::now();
    if (now < nextFrame) {
        sleep_until(nextFrame);
        nextFrame += frameDuration{ 1 };
    }
    else {
        nextFrame = system_clock::now() + frameDuration{ 1 };
    }
}

// This feature class registers itself.
class : public Feature {
public:
    void init() {
        // override the entire sleepy section - 32 bytes long
        MemoryPatch(0x451C2A)
            << CALL(_gameLoop)
            << CALL(throttle)
            << BYTES(ASM::NOP, 22);

        // override the entire sleepy section - 23 bytes long
        MemoryPatch(0x4FA663)
            << CALL(_oogLoop)
            << CALL(throttle)
            << BYTES(ASM::NOP, 13);

    }
} feature;
