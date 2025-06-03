#include "LedGroup.h"

// Constructor
LedGroup::LedGroup() {
    // Initialization code here
}

void LedGroup::begin(CRGB* _FastLedMemory, uint8_t _ledCount, uint8_t* _ledIndizes) {
    ledCount = _ledCount; // Set the number of LEDs in this group
    FastLedMemory = _FastLedMemory; // Store the pointer to the FastLED memory where the colors are stored
    for (uint8_t i = 0; i < ledCount; i++) {
        ledIndex[i] = _ledIndizes[i]; // Copy the LED indices to the group
    } 
    // Initialize the LED group with ledCount LEDs
}

void LedGroup::setGroupActive(bool isActive) {
    // Set the brightness based on whether the group is active or idle
    if (isActive) {
        for (uint8_t i = 0; i < ledCount; i++) {
            FastLedMemory[ledIndex[i]].nscale8_video(BrightnessActive); // Fade to active brightness
        }
    } else {
        for (uint8_t i = 0; i < ledCount; i++) {
            FastLedMemory[ledIndex[i]].nscale8_video(BrightnessIDLE); // Fade to idle brightness
        }
    }
}


void LedGroup::setColor(CRGB color) {
    //only set the memory here. Overlaying GroupController will handle the blending and fading of the colors.

    for (uint8_t i = 0; i < ledCount; i++) {
        FastLedMemory[ledIndex[i]] = color; // Set the color for each LED in this group
    }
}