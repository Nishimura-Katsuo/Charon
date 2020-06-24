/**
 * Enables all of the drawing hooks.
 *
 * @todo Reimplement the full render loop function some day.
 */

#include "headers/feature.h"
#include "headers/common.h"
#include "headers/pointers.h"
#include "headers/hook.h"
#include <iostream>

namespace D2 {
    int ScreenWidth = 0, ScreenHeight = 0;
}

/**
 * Drawing hooks.
 */
void preDrawUnitsPatch() {
    State["inGame"] = true;
    const DWORD addr = 0x473c00;
    for (Feature* f = Features; f; f = f->next) {
        f->gameUnitPreDraw();
    }
    __asm {
        call addr;
    }
}

void _gameUnitPostDraw() {
    for (Feature* f = Features; f; f = f->next) {
        f->gameUnitPostDraw();
    }
}

void gameAutomapDraw() {
    for (Feature* f = Features; f; f = f->next) {
        f->gameAutomapPreDraw();
    }
    D2::DrawAutomap();
    for (Feature* f = Features; f; f = f->next) {
        f->gameAutomapPostDraw();
    }
}

void oogDraw() {
    State["inGame"] = false;
    D2::DrawSprites();
    D2::GetScreenModeSize(D2::GetScreenMode(), &D2::ScreenWidth, &D2::ScreenHeight);
    DWORD old = D2::SetFont(DEFAULT_FONT);
    for (Feature* f = Features; f; f = f->next) {
        f->oogPostDraw();
    }
    D2::SetFont(old);
    // Since we patch to override DrawSprites, we need to call it ourselves.
}

void gameDraw() {
    DWORD old = D2::SetFont(DEFAULT_FONT);
    for (Feature* f = Features; f; f = f->next) {
        f->gamePostDraw();
    }
    D2::SetFont(old);
}

// This feature class registers itself.
class : public Feature {
public:
    void init() {
        MemoryPatch(0x476ce1) << CALL(preDrawUnitsPatch); // Hook the unit draw
        MemoryPatch(0x456fa5) << CALL(gameAutomapDraw); // Hook the automap draw
        MemoryPatch(0x44CB14) << CALL(gameDraw); // Hook the game draw
        MemoryPatch(0x4F9A0D) << CALL(oogDraw); // Hook the oog draw
        MemoryPatch(0x476d31) << JUMP(_gameUnitPostDraw);
    }
} feature;
