#pragma once
#include <FastLED.h>


//led Group is only a struct which holds information, it does not do anything on its own

enum class LedGroupFunction : uint8_t {
    FLAT = 0,
    SINE = 1
};

struct LedGroup {
    CRGB* PixelStorage; // Pointer to the LED memory from the Controlling FastLed instance
    uint8_t lenght; // Number of LEDs in this group

    LedGroupFunction function; // Function to apply to this group (e.g., FLAT, COSINE)

    CRGB color; // Color of the LED group

    int16_t* fkt_preRendered; // Function pre calculated at startup for performance Is the same length as the number of LEDs in the group. Ranges from 0 to 255

    uint8_t minBrightness = 0; // Minimum brightness level
    uint8_t maxBrightness = 0; // Maximum brightness level

    bool isActive = false; // Whether the group is active (ON) or idle (OFF)

};