#pragma once
#include "LedGroup.h"
#include "TAS_Hardware.h"
#include "OpenKNX.h"
#include <FastLED.h>

//this is a LEDGroupController which processes all the LEDGroups and manages their states and colors

class LedGroupController {
public:

    //empty constructor
    LedGroupController();

    LedGroupController(uint8_t _numLedGroups, uint8_t* _ledGroupSize); // Constructor, takes number of LED'groups to manage as well as an array of their sizes

     //adds LED group to the controller
     //!!! LED-Groups MUST be added in the same order as the physical LED chain is built !!!
     //Adds a LED group to the controller, takes the number of LED's in the group, the function to apply to the group and the color of the group as parameters. 
     //The color order can be specified if it is different from the default, which is set in TAS_Hardware.h. The color order is defined in the FastLED library, e.g. RGB, GRB, BRG, etc.
    void AddLedGroup(uint8_t _ledCount, LedGroupFunction _function, CRGB color, EOrder colorOrder);

    void setGroupActive(bool isActive);
    void setColor(LedGroup* grp, CRGB color);

    CRGB* getFastLedMemoryPointer() { return FastLedPhysicalMemory; } //returns the pointer to the FastLed memory

    LedGroup* getLedGroup(uint8_t index) { return &ledGroups[index]; } //returns the LedGroup at the given index

    uint8_t getOverallLedGroupCount() { return OverallLedGroupCount; } //returns the number of LED groups

    void evaluate(); //evaluates all LED groups and updates their states accordingly

private:

    CRGB* FastLedPhysicalMemory = nullptr; // FastLed Memory Pointer
    
    enum ControllerState {
        IDLE,
        CHANGE_SHEDULED,
        CHANGING,
        CHANGE_FINISHED
    };

    ControllerState state = ControllerState::IDLE;

    //will process all the pixels and update their states
    void Process();

    void CalculatePixelWithFunction(); //calculates the pixel values based on the functions of each group

    //for setup and parametrisations
    LedGroup* ledGroups = nullptr;
    uint8_t OverallLedGroupCount = 0; //how much groups we controll
    uint8_t OverallLedCount = 0; //Accumulated number of LEDs in all groups
    uint8_t InitializedGroups = 0; //for running the setup, how many groups have been initialized already

    //for animation
    uint16_t FrameTime_ms = 1000 / TTD_LED_FPS; //Frame time in ms, e.g. how often the LED should be updated.
    CRGB* PreviousPixelArray = nullptr; //Pointer to the previous pixel array for blending
    CRGB* TargetPixelArray = nullptr; //Pointer to the target pixel array for blending
    uint16_t fadingAmount = 0;

    //for keeping the fixed FPS timing
    uint32_t LastFrameTime = 0; //Time of the last frame update

    EOrder BaseColorOrder = GRB; //Base color order for the controller, this is used for the groups which do not have a specific color order set.


};