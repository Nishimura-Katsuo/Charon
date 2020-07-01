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

class FoundExit {
public:
    DPOINT origin;
    DPOINT target;
    DWORD type;
};

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
    std::map<DWORD, std::vector<FoundExit>> RevealedExits;

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

    void gameAutomapPostDraw() {
        D2::Types::UnitAny* player = D2::PlayerUnit;
        if (player) {
            D2::Types::Level* level = player->pPath->pRoom1->pRoom2->pLevel;
            if (level) {
                for (D2::Types::Room2* room = player->pPath->pRoom1->pRoom2->pLevel->pRoom2First; room; room = room->pRoom2Next) {
                    if (room->pPreset) {
                        for (D2::Types::PresetUnit* ps = room->pPreset; ps; ps = ps->pPresetNext) {
                            if (ps->dwType == 5) {
                                DrawAutomapRadialShape({ (double)room->dwPosX * 5 + ps->dwPosX, (double)room->dwPosY * 5 + ps->dwPosY }, 4, 8, 0x83, M_PI / 8);
                            }
                        }
                    }
                }

                for (FoundExit exit : RevealedExits[level->dwLevelNo]) {
                    //DrawLine(WorldToAutomap(exit.origin), WorldToAutomap(exit.target), 0x83);
                }
            }
        }
    }

    void CheckExits(D2::Types::Room2* current) {
        FoundExit exit;
        int x1 = 0, y1 = 0;
        int x2 = 0, y2 = 0;

        for (unsigned int c = 0; c < current->dwRoomsNear; c++) {
            if (current->pLevel->dwLevelNo != current->pRoom2Near[c]->pLevel->dwLevelNo && (current->dwPosX == current->pRoom2Near[c]->dwPosX || current->dwPosY == current->pRoom2Near[c]->dwPosY)) {
                exit.origin = { ((double)current->dwPosX + (double)current->dwSizeX / 2) * 5, ((double)current->dwPosY + (double)current->dwSizeY / 2) * 5 };
                exit.target = { ((double)current->pRoom2Near[c]->dwPosX + (double)current->pRoom2Near[c]->dwSizeX / 2) * 5, ((double)current->pRoom2Near[c]->dwPosY + (double)current->pRoom2Near[c]->dwSizeY / 2) * 5 };
                RevealedExits[current->pLevel->dwLevelNo].push_back(exit);
            }
        }
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
                        if (room2->pRoom1 == nullptr) {
                            D2::AddRoomData(room2->pLevel->pMisc->pAct, room2->pLevel->dwLevelNo, room2->dwPosX, room2->dwPosY, room2->pRoom1);

                            if (room2->pRoom1 != nullptr) {
                                RevealAutomapRoom(room2->pRoom1, TRUE, revealdata[level->dwLevelNo].layer);
                                CheckExits(room2);
                                // Not sure that we need to unload the room again... we're going to need it again later.
                                // D2::RemoveRoomData(room2->pLevel->pMisc->pAct, room2->pLevel->dwLevelNo, room2->dwPosX, room2->dwPosY, room2->pRoom1);
                            }
                        }
                        else {
                            RevealAutomapRoom(room2->pRoom1, TRUE, revealdata[level->dwLevelNo].layer);
                            CheckExits(room2);
                        }
                    }

                    room2 = revealdata[level->dwLevelNo].room2 = revealdata[level->dwLevelNo].room2->pRoom2Next;
                }
            }
        }
    }
} feature;
