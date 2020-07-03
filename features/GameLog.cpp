#include "headers/feature.h"
#include "headers/common.h"
#include "headers/hook.h"
#include "headers/remote.h"
#include <list>

REMOTEFUNC(void __fastcall, PrintGameString, (const wchar_t* wMessage, int nColor), 0x49E3A0)
REMOTEFUNC(void __fastcall, PrintPartyString, (const wchar_t* wMessage, int nColor), 0x49E5C0) // Updated 1.14d //0049E5C0-BASE

struct GameLogMessage {
    DWORD timestamp;
    std::wstring text;
};

std::list<GameLogMessage> GameLogMessages;
GameOutput gamelog;

int GameOutput::GameOutputBuffer::sync() {
    std::wstring::size_type start = 0;
    std::wstring::size_type pos = this->str().find(L"\n");

    while (pos != std::wstring::npos) {
        std::wstring tmp = this->str().substr(start, pos - start);
        GameLogMessages.push_back({ GetTickCount(), tmp });
        start = pos + 1;
        pos = this->str().find(L"\n", start);
    }

    if (start > 0) {
        this->str(this->str().substr(start));
    }

    return 0;
}

GameOutput::GameOutput() : std::wostream(&buf) { }

std::wstring COLOR(BYTE color) {
    wchar_t ret[]{ 255, 99, 48, 0 };
    ret[2] += (color & 0xf);
    return ret;
}

namespace GameLog {

    class : public Feature {
        DWORD start = 0;
    public:
        void init() {
            //gamelog << COLOR(4) << "Game log installed..." << std::endl;
        }

        void gamePostDraw() {
            while (!GameLogMessages.empty()) {
                PrintGameString(GameLogMessages.front().text.c_str(), 0);
                GameLogMessages.pop_front();
            }
        }

        void oogPostDraw() {
            int top = 5, left = 10;
            DWORD height = 0, width = 0, fontno = 4;

            if (!start) {
                start = GetTickCount();
            }

            if (GetTickCount() - start < 300) {
                return;
            }

            while (!GameLogMessages.empty() && GameLogMessages.front().timestamp + 8000 < GetTickCount()) {
                GameLogMessages.pop_front();
            }

            D2::SetFont(fontno);

            for (GameLogMessage message : GameLogMessages) {
                height = D2::GetTextSize(message.text.c_str(), &width, &fontno);
                D2::DrawGameText(message.text.c_str(), left, top += (height - 10), 0, 0);
            }
        }
    } feature;

}
