/**
 * Reveals the automap when moving to new areas.
 */

#define _USE_MATH_DEFINES

#include "headers/feature.h"
#include "headers/common.h"
#include "headers/hook.h"
#include "headers/remote.h"
#include <iostream>
#include <cmath>

REMOTEFUNC(DWORD __stdcall, GetAutomapSize, (void), 0x45A710);
REMOTEFUNC(D2::Types::AutomapCell* __fastcall, NewAutomapCell, (), 0x457C30);
REMOTEFUNC(void __fastcall, AddAutomapCell, (D2::Types::AutomapCell* aCell, D2::Types::AutomapCell** node), 0x457B00);
REMOTEFUNC(void __stdcall, RevealAutomapRoom, (D2::Types::Room1* pRoom1, DWORD dwClipFlag, D2::Types::AutomapLayer* aLayer), 0x458F40);
REMOTEFUNC(D2::Types::AutomapLayer2* __fastcall, GetLayer, (DWORD dwLevelNo), 0x61E470);
REMOTEREF(D2::Types::AutomapLayer*, AutomapLayer, 0x7A5164);

struct RevealData {
    bool isRevealed = false;
    D2::Types::Room2* room2 = nullptr;
    D2::Types::AutomapLayer* layer = nullptr;
};

// This feature class registers itself.
class : public Feature {
    DWORD currentLevel = 0;
    DWORD revealStart = 0;
    bool inGame = false;

    std::map<DWORD, RevealData> revealdata;

public:
    void init() {
        gamelog << COLOR(4) << "Installing map reveal..." << std::endl;
        MemoryPatch(0x4DC000) << ASM::RET; // Disable fade effects from switching areas (so we can reveal sooner)
    }

    void gameLoop() {
        if (!inGame) {
            revealdata.clear();
            inGame = true;
        }
        D2::Types::UnitAny* me = D2::PlayerUnit;

        if (me && me->pPath && me->pPath->pRoom1 && me->pPath->pRoom1->pRoom2 && me->pPath->pRoom1->pRoom2->pLevel) {
            DWORD levelno = me->pPath->pRoom1->pRoom2->pLevel->dwLevelNo;
            if (levelno != currentLevel) {
                currentLevel = levelno;
                revealStart = GetTickCount();
            }

            if (GetTickCount() - revealStart > 200) {
                RevealCurrentLevel();
            }
        }
    }

    void oogLoop() {
        currentLevel = 0;
        inGame = false;
    }

    bool EnsureRoom(D2::Types::Room2* current) {
        if (current->pRoom1 != nullptr) {
            return true;
        }

        D2::AddRoomData(current->pLevel->pMisc->pAct, current->pLevel->dwLevelNo, current->dwPosX, current->dwPosY, current->pRoom1);
        return current->pRoom1 != nullptr;
    }

    void RevealCurrentLevel() {
        D2::Types::UnitAny* me = D2::PlayerUnit;
        int c = 20;

        if (me) for (D2::Types::Level* level = me->pAct->pMisc->pLevelFirst; level != nullptr; level = level->pNextLevel) {
            if (level && level->dwLevelNo > 0) {
                if (!revealdata[level->dwLevelNo].isRevealed && level->pRoom2First != nullptr && AutomapLayer->nLayerNo == GetLayer(level->dwLevelNo)->nLayerNo) {
                    revealdata[level->dwLevelNo].layer = AutomapLayer;
                    revealdata[level->dwLevelNo].room2 = level->pRoom2First;
                    revealdata[level->dwLevelNo].isRevealed = true;
                }

                D2::Types::Room2* room2 = revealdata[level->dwLevelNo].room2;

                for (; c > 0 && room2 != nullptr; c--) {
                    if (room2->pLevel && room2->pLevel->pMisc && room2->pLevel->pMisc->pAct) {
                        if (EnsureRoom(room2)) {
                            RevealAutomapRoom(room2->pRoom1, TRUE, revealdata[level->dwLevelNo].layer);
                            // Not sure that we need to unload the room again... we're going to need it again later.
                            // D2::RemoveRoomData(room2->pLevel->pMisc->pAct, room2->pLevel->dwLevelNo, room2->dwPosX, room2->dwPosY, room2->pRoom1);
                        }
                    }

                    room2 = revealdata[level->dwLevelNo].room2 = revealdata[level->dwLevelNo].room2->pRoom2Next;
                }
            }
        }
    }
} feature;
