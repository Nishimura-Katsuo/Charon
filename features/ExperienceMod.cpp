#include "headers/feature.h"
#include "headers/common.h"
#include "headers/hook.h"
#include "headers/remote.h"

REMOTEREF(DWORD, PlayerCountOverride, 0x883d70);

bool mult = false;

void __fastcall SetPlayerCount(DWORD count) {
    PlayerCountOverride = count > 1 ? count : 1;
    gamelog << "Difficulty (players) set to " << count << std::endl;
}

int __fastcall ExperienceHook(int exp) {
    double newxp = (double)exp;

    if (mult) {
        newxp *= (double)PlayerCountOverride * 2.7f / ((double)PlayerCountOverride + 1.0f);
    }

    exp = (int)newxp;

    if (State["debugMode"]) {
        gamelog << "Exp gained: " << exp << std::endl;
    }
    return exp;
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

            MemoryPatch(0x57e501) << ASM::MOV_ECX_EAX << CALL(ExperienceHook) << BYTESEQ{ 0xc2, 0x08, 0x00 };

            ChatInputCallbacks[L"/xp"] = [&](std::wstring cmd, InputStream wchat) -> BOOL {
                mult = !mult;
                gamelog << "Improved players X exp scaling " << (mult ? "on" : "off") << std::endl;

                return FALSE;
            };
        }
    } feature;

}
