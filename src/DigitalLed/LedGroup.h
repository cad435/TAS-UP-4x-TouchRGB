#pragma once
#include <FastLED.h>


//led Group is only a struct which holds information, it does not do anything on its own

enum class LedGroupFunction : uint8_t {
    FLAT = 0,
    SINE,
    ZERO
};

enum LuminosityState {
        BRIGHT, //Brightness is at maxBrightness
        DELAY_START, //Proximity was lost but the group is still bright, this state is used to start the delay before changing to DIM
        DELAY, //Brightness is at maxBrightness but a change to IDLE is scheduled because Proximity was lost. change to IDLE will happen after some delay-time 
        DIM //Brightness is at minBrightness
    };

struct LedGroup {
    CRGB* PixelStorage; // Pointer to the LED memory from the Controlling FastLed instance
    uint8_t length; // Number of LEDs in this group

    LedGroupFunction function; // Function to apply to this group (e.g., FLAT, COSINE)

    CRGB color; // Color of the LED group
    EOrder colorOrder = GRB; // Color order for this group (e.g., GRB for SK6812, RGB for WS2811)

    int16_t* fkt_preRendered; // Function pre calculated at startup for performance Is the same length as the number of LEDs in the group. Ranges from 0 to 255

    uint8_t minBrightness = 0; // Minimum brightness level
    uint8_t maxBrightness = 0; // Maximum brightness level

    LuminosityState luminosityState = LuminosityState::DIM; // Whether the group is active (ON) or idle (OFF)

};