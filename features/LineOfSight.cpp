/**
* @description patch the Line Of Sight, so we can see monsters on the other side of a wall
* @Author Jaenster
*/

#include "headers/feature.h"
#include "headers/common.h"
#include "headers/pointers.h"
#include "headers/hook.h"

namespace LineOfSight {
	
    
	void (*JumpTo)() = *(void (*)())0x4DC73A; // jump to spot


    class : public Feature {
    public:
        void init() {
            std::cout << "Installing line of sight vision" << std::endl;
        
			MemoryPatch(0x4DC713) << JUMP(JumpTo) << BYTE(0x90) << BYTE(0x90);

        }
    } feature;
}