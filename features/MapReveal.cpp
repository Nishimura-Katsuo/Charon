/**
 * Reveals the automap when moving to new areas.
 */

#define _USE_MATH_DEFINES

#include "headers/feature.h"
#include "headers/common.h"
#include "headers/pointers.h"
#include "headers/hook.h"
#include <iostream>
#include <cmath>

class FoundExit {
public:
    DPOINT origin;
    DPOINT target;
    DWORD type;
};

struct RevealData {
    bool isRevealed = false;
    D2::Types::Room2* room2 = nullptr;
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
        D2::Types::UnitAny* me = D2::PlayerUnit[0];

        if (me && me->pPath && me->pPath->pRoom1 && me->pPath->pRoom1->pRoom2 && me->pPath->pRoom1->pRoom2->pLevel) {
            DWORD levelno = me->pPath->pRoom1->pRoom2->pLevel->dwLevelNo;
            if (levelno != currentLevel) {
                currentLevel = levelno;
                revealStart = GetTickCount();
            }

            if (GetTickCount() - revealStart > 150) {
                RevealCurrentLevel();
            }
        }
    }

    void oogLoop() {
        currentLevel = 0;
        inGame = false;
    }

    void gameAutomapPostDraw() {
        D2::Types::UnitAny* player = D2::GetPlayerUnit();
        if (player) {
            D2::Types::Level* level = player->pPath->pRoom1->pRoom2->pLevel;
            if (level) {
                // Loop trough all rooms of current lvl
                for (D2::Types::Room2* room = player->pPath->pRoom1->pRoom2->pLevel->pRoom2First; room; room = room->pRoom2Next) {
                    if (room->pPreset) {
                        // Loop trough presets of current lvl
                        for (D2::Types::PresetUnit* ps = room->pPreset; ps; ps = ps->pPresetNext) {
                            //@ToDo; figure out which dots are important and which aren't
                            // for now all we do is draw a green dot
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
        D2::Types::UnitAny* me = D2::PlayerUnit[0];

        if (me) {
            D2::Types::Level* level = me->pPath->pRoom1->pRoom2->pLevel;

            if (level && level->dwLevelNo > 0) {
                if (!revealdata[level->dwLevelNo].isRevealed) {
                    revealdata[level->dwLevelNo].room2 = level->pRoom2First;
                    revealdata[level->dwLevelNo].isRevealed = true;
                }

                D2::Types::Room2* room2 = revealdata[level->dwLevelNo].room2;

                for (int c = 0;  c < 10 && room2 != nullptr; c++) {
                    if (room2->pLevel && room2->pLevel->pMisc && room2->pLevel->pMisc->pAct) {
                        if (room2->pRoom1 == nullptr) {
                            D2::AddRoomData(room2->pLevel->pMisc->pAct, room2->pLevel->dwLevelNo, room2->dwPosX, room2->dwPosY, NULL);

                            if (room2->pRoom1) {
                                D2::RevealAutomapRoom(room2->pRoom1, TRUE, *D2::AutomapLayer);
                                CheckExits(room2);
                                D2::RemoveRoomData(room2->pLevel->pMisc->pAct, room2->pLevel->dwLevelNo, room2->dwPosX, room2->dwPosY, NULL);
                            }
                        }
                        else {
                            D2::RevealAutomapRoom(room2->pRoom1, TRUE, *D2::AutomapLayer);
                            CheckExits(room2);
                        }
                    }

                    room2 = revealdata[level->dwLevelNo].room2 = revealdata[level->dwLevelNo].room2->pRoom2Next;
                }
            }
        }
    }
} feature;
