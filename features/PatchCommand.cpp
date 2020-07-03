/**
 * Enables a command for patching bytes in-game: /patch
 */

#include "headers/feature.h"
#include "headers/common.h"
#include "headers/hook.h"
#include <iostream>
#include <iomanip>

using std::hex;

struct DeferredPatch {
    int size;
    unsigned long long value;
};

// This feature class registers itself.
class : public Feature {
public:
    void init() {
        ChatInputCallbacks[L"/patch"] = [](std::wstring cmd, InputStream wchat) -> BOOL {
            DWORD address;
            int size;
            unsigned long long value;
            std::vector<DeferredPatch> data;
            std::wstring possible;
            wchat >> hex >> address;

            if (wchat) {
                wchat >> possible;
                if (!wchat) {
                    gamelog << COLOR(3) << "No data specified." << std::endl;
                    goto usage;
                }

                do {
                    size = possible.size() >> 1;
                    if (size == 1 || size == 2 || size == 4 || size == 8) {
                        std::wstringstream hexread(possible);
                        hexread >> hex >> value;
                        if (hexread) {
                            data.push_back({ size, value });
                        }
                        else {
                            gamelog << COLOR(3) << "Data must be hex formatted and byte aligned (2, 4, 8, 16 long)!" << std::endl;
                            goto usage;
                        }
                    }
                    else {
                        gamelog << COLOR(3) << "Data must be hex formatted and byte aligned (2, 4, 8, 16 long)!" << std::endl;
                        goto usage;
                    }
                    wchat >> possible;
                } while (wchat);

                MemoryPatch patch(address);
                for (DeferredPatch patchdata : data) {
                    switch (patchdata.size) {
                    case 1:
                        patch << (char)patchdata.value;
                        break;
                    case 2:
                        patch << (short)patchdata.value;
                        break;
                    case 4:
                        patch << (long)patchdata.value;
                        break;
                    case 8:
                        patch << (long long)patchdata.value;
                        break;
                    default:
                        gamelog << COLOR(1) << "Nishi, your code is stupid. Please write it correctly." << std::endl;
                        return FALSE;
                    }
                }

                return FALSE;
            }

        usage:

            gamelog << COLOR(3) << "Example: " << cmd << " 7BB3A4 00000001" << std::endl
                << "Usage: " << cmd << " address data [data ...]" << std::endl;

            return FALSE;
        };
    }
} feature;
