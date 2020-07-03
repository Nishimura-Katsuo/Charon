/**
* @description patch the Line Of Sight, so we can see monsters on the other side of a wall
* @Author Jaenster
*/

#include "headers/feature.h"
#include "headers/common.h"
#include "headers/hook.h"

namespace Omnivision {
	
    class : public Feature {
    public:
        void init() {
            //gamelog << COLOR(4) << "Installing debug omnivision..." << std::endl;
        }

        void gameLoop() {
            if (State["debugMode"] && !State["omnivision"]) {
                MemoryPatch(0x4DC713) << JUMP((LPVOID)0x4DC73A) << BYTE(0x90) << BYTE(0x90);
                State["omnivision"] = true;
            }
            else if (!State["debugMode"] && State["omnivision"]) {
                MemoryPatch(0x4DC713) << REVERT(7);
                State["omnivision"] = false;
            }
        }
    } feature;

}
