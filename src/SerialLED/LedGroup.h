#pragma once

#include <Arduino.h>
#include "hardware.h"
#include <FastLED.h>

class LedGroup
{
private:
    /* data */
    //inside the ledIndex are the indices of the LEDs that are part of this group. The indizes are the plces of the physical LED's in the WS2812 array, starting with 0.
    //Example: if we have one LED-String and we want to have LED 0, 2, 3 controlled as "one led", then ledIndex would be {0, 2, 3}.
    uint8_t ledIndex[TTD_LED_COUNT] = {0}; // Array to hold the index of each LED assigned to that group
    
    uint8_t ledCount = 0; // Number of LEDs in this group

    CRGB* FastLedMemory = nullptr; // Pointer to the FastLED memory where the colors are stored
public:
    LedGroup(/* args */);
    void begin(CRGB* _FastLedMemory, uint8_t _ledCount, uint8_t* _ledIndizes); // Initialize the LED group with ledCount and ledIndex
    void setColor(CRGB color); // Set the color of all LEDs in this group

    uint8_t BrightnessActive = 255; // Brightness when the button is in use or Proximity sensed
    uint8_t BrightnessIDLE = 0; // Base when the button is IDLE

    void setGroupActive(bool isActive); //True when the group is active ("Active Brightness" used), false when the group is idle ("Idle Brightness" used)

};

