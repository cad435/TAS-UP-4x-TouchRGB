#pragma once
#include <Arduino.h>
#include <FastLED.h>
#include "LedGroup.h"

class LedGroupController
{

enum TDD_LED_STATE
    {
      STATIC, //LED's are static, no animation
      TODO, //LED's are sheduled for a change, but not yet changed
      RUNNING //lED's are running, animation is running
    };

private:

    TDD_LED_STATE ledState = STATIC; // Current state of the LED controller

    LedGroup* groups[8] = {nullptr}; // Array of pointers to LedGroup objects
    uint8_t groupCount = 0; // Number of groups managed by this controller
    uint8_t OverallLEDCount = 0; // Total number of LEDs across all groups
    CRGB FastLedHWMemory_Current[TTD_LED_COUNT] = {0}; // Current color of the LEDs

    //those variables are used for dynmaic blending
    uint16_t fadingAmount = 0; // Fading amount for the LEDs
    CRGB ledValues_Original[TTD_LED_COUNT] = {0}; // Original color of the LEDs before blending
    CRGB ledValues_Target[TTD_LED_COUNT] = {0}; // Target color of the LEDs to blend towards

    uint32_t lastTimeLEDRun = 0; // Timestamp of the last LED update
    const uint32_t LEDFPSTime_ms = 1000 / TTD_LED_FPS; // Time in milliseconds for each frame based on the defined FPS
    void FixedFPSLedLoop();

public:
    LedGroupController(uint8_t _OverallLedCount, uint8_t _groupCount); // Constructor to initialize the controller with a specified number of groups

    void initGroup(uint8_t groupIndex, uint8_t _GroupLedCount, uint8_t* _ledIndizes); // Initialize a group with the specified index and LED indices

    void setGroupColor(uint8_t groupIndex, CRGB color);

    void setActiveBrightness(uint8_t groupIndex, uint8_t brightness);
    void setIDLEBrightness(uint8_t groupIndex, uint8_t brightness);

    void evaluate(); // Evaluate the LED groups and update their colors if necessary
};

