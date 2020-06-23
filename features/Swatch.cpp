/**
 * Enables a color swatch command: /swatch
 */

#include "headers/feature.h"
#include "headers/hook.h"
#include "headers/common.h"
#include "headers/pointers.h"
#include <iostream>

wchar_t wHex[] = L"0123456789ABCDEF";

// This feature class registers itself.
class : public Feature {
public:
    void toggleSwatch() {
        State["drawSwatch"] = !State["drawSwatch"];

        if (State["drawSwatch"]) {
            gamelog(2) << "Swatch on." << std::endl;
        }
        else {
            gamelog(1) << "Swatch off." << std::endl;
        }
    }

    void init() {
        std::cout << "Installing color swatch..." << std::endl;
        ChatInputCallbacks[L"/swatch"] = [&](std::wstring cmd, InputStream wchat) -> BOOL {
            toggleSwatch();
            return FALSE;
        };

        HotkeyCallbacks[VK_F11] = [&](LPARAM options) -> BOOL {
            toggleSwatch();
            return FALSE;
        };
    }

    void gamePostDraw() {
        DWORD fontnum = 8, height, width;
        D2::SetFont(fontnum);
        if (State["drawSwatch"]) {
            wchar_t msg[3] = { 0 };
            int color, gridsize = 24;
            for (int x = 0; x < 32; x++) {
                for (int y = 0; y < 8; y++) {
                    color = (x << 3) | y;
                    D2::DrawRectangle(x * gridsize, y * gridsize, x * gridsize + gridsize, y * gridsize + gridsize, color, 0xFF);
                    msg[0] = wHex[color >> 4];
                    msg[1] = wHex[color & 15];
                    height = D2::GetTextSize(msg, &width, &fontnum);
                    D2::DrawGameText(msg, x * gridsize + (gridsize - width) / 2, (y + 1) * gridsize, 0, 0);
                }
            }
        }
    }
} feature;
