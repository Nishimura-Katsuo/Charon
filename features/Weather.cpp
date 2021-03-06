/**
 * @description Make weather toggleable
 * @Author Jaenster
 */

#include "headers/feature.h"
#include "headers/common.h"
#include "headers/hook.h"

namespace Weather {
    bool bDrawWeather = true;
   
    void (*DrawWeather)() = *(void (*)())0x473910;
    
    void drawWeatherIntercept() {
        if (bDrawWeather) DrawWeather();
    }

    class : public Feature {
    public:
        void init() {
            MemoryPatch(0x476D23) << CALL(drawWeatherIntercept);

            HotkeyCallbacks[VK_INSERT] = [&](LPARAM options) -> BOOL {
                toggleWeather();
                gamelog << COLOR(2) << "Draw weather " << (bDrawWeather  ? "true" : "false") << std::endl;
                return FALSE;
            };
        }

        void toggleWeather() {
            bDrawWeather = !bDrawWeather;
        }

    } feature;
}
