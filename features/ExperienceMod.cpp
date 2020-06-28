#include "headers/feature.h"
#include "headers/common.h"
#include "headers/hook.h"
#include "headers/remote.h"

REMOTEREF(DWORD, PlayerCountOverride, 0x883d70);

void __fastcall SetPlayerCount(DWORD count) {
    PlayerCountOverride = count > 1 ? count : 1;
    gamelog << "Difficulty (players) set to " << count << std::endl;
}

namespace ExperienceMod {

    class : public Feature {
    public:
        void init() {
            gamelog << COLOR(4) << "Installing experience modifier..." << std::endl;
            // Rewrite players count modification and leave to our handler - 42 bytes total
            MemoryPatch(0x47c4e4)
                << ASM::PUSHAD
                << ASM::MOV_ECX_EAX
                << CALL(SetPlayerCount)
                << ASM::POPAD
                << NOP_TO(0x47c50e);
        }
    } feature;

}
