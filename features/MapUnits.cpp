/**
 * Displays units on the map and automap.
 */

#include "headers/feature.h"
#include "headers/common.h"
#include "headers/hook.h"
#include "headers/remote.h"
#include <iostream>

DWORD ItemRarityColor[32] = { 255, 29, 30, 32, 151, 132, 111, 155, 111 };

// This feature class registers itself.
class : public Feature {
public:
    void init() {
        gamelog << COLOR(4) << "Installing unit tracker..." << std::endl;
    }

    void gameUnitPreDraw() {
        D2::Types::UnitAny* player = D2::PlayerUnit;
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
                                DrawWorldRadialShape({ (double)room->dwPosX * 5 + ps->dwPosX, (double)room->dwPosY * 5 + ps->dwPosY }, 4, 32, 0x83);
                            }
                        }
                    }

                }
            }
        }
    }

    void gameAutomapPostDraw() {
        BYTE d;

        // Client side tracks missiles
        d = 3;
        for (int c = 0; c < 128; c++) {
            for (D2::Types::UnitAny* unit = D2::ClientSideUnitHashTables[d].table[c]; unit != NULL; unit = unit->pListNext) {
                unit->dwOwnerType;
                switch (unit->dwOwnerType) {
                    // Missiles cast directly by us (not owned units, like merc, hydras, etc)
                case 0:
                    DrawDot(WorldToAutomap(unit->pPath), 0x99);
                    break;
                    // Most others
                case 1:
                default:
                    DrawDot(WorldToAutomap(unit->pPath), 0x99);
                    break;
                }
            }
        }

        // Server side tracks items
        d = 4;
        for (int c = 0; c < 128; c++) {
            for (D2::Types::UnitAny* unit = D2::ServerSideUnitHashTables[d].table[c]; unit != NULL; unit = unit->pListNext) {
                if (unit->dwMode == 3 || unit->dwMode == 5) {
                    if (unit->pItemData->dwFlags & 0x4000000) {
                        DrawAutomapX(unit->pItemPath, ItemRarityColor[7], 3);
                    }
                    else if (unit->pItemData->dwQuality > 3 || (unit->pItemData->dwFlags & 0x400800 && unit->pItemData->dwQuality > 2 && D2::GetUnitStat(unit, 194, 0) >= 2)) { // @todo check number of sockets
                        DrawAutomapX(unit->pItemPath, ItemRarityColor[unit->pItemData->dwQuality], 3);
                    }
                    else {
                        D2::Types::ItemTxt* txt = D2::GetItemText(unit->dwTxtFileNo);
                        if (txt->nType >= 96 && txt->nType <= 102 || txt->nType == 74) {
                            DrawAutomapX(unit->pItemPath, 169, 3);
                        }
                    }
                }
            }
        }

        // Server side tracks enemies
        d = 1;
        for (int c = 0; c < 128; c++) {
            for (D2::Types::UnitAny* unit = D2::ServerSideUnitHashTables[d].table[c]; unit != NULL; unit = unit->pListNext) {
                if (isHostile(unit) && unitHP(unit) > 0) {
                    if (isAttackable(unit)) {
                        if (unit->pMonsterData->fBoss || unit->pMonsterData->fChamp) {
                            DrawAutomapX(unit->pPath, 0x0C);
                        }
                        else if (unit->pMonsterData->fMinion) {
                            DrawAutomapX(unit->pPath, 0x0B);
                        }
                        else {
                            DrawAutomapX(unit->pPath, 0x0A);
                        }
                    }
                    else {
                        DrawAutomapX(unit->pPath, 0x1B);
                    }
                }
            }
        }
    }
} feature;
