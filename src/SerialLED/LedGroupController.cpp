#include "LedGroupController.h"


LedGroupController::LedGroupController(uint8_t _OverallLedCount, uint8_t _groupCount)
{
    groupCount = _groupCount; // Set the number of groups managed by this controller
    OverallLEDCount = _OverallLedCount; // Set the total number of LEDs across all groups
    for (uint8_t i = 0; i < groupCount; i++)
    {
        groups[i] = new LedGroup(); // Initialize each group in the array
    }

    FastLED.addLeds<TTD_LED_TYPE, TTD_RGB_DIO_PIN, TTD_RGB_ORDER>(FastLedHWMemory_Current, _OverallLedCount);
    FastLED.clear(true);
    
}

void LedGroupController::initGroup(uint8_t groupIndex, uint8_t _GroupLedCount, uint8_t* _ledIndizes)
{
    if (groupIndex < groupCount && groups[groupIndex] != nullptr)
    {
                                 // use the "target" memory for the group, so that the group will always write to the "should be" state --> The Controllers Blending transistions the "should be" (=target) state to the "current" state
        groups[groupIndex]->begin(ledValues_Target, _GroupLedCount, _ledIndizes); // Initialize the specified group
    }
}


void LedGroupController::setGroupColor(uint8_t groupIndex, CRGB color)
{
    if (groupIndex < groupCount && groups[groupIndex] != nullptr)
    {
        groups[groupIndex]->setColor(color); // Set the color for the specified group
        ledState = TODO; // Set the LED state to TODO, indicating that a change is scheduled
    }
}

void LedGroupController::evaluate()
{
    if (millis() - lastTimeLEDRun >= LEDFPSTime_ms) //If the time is up, run the LED's
    {
        FixedFPSLedLoop(); //Run the LED's
        lastTimeLEDRun = millis(); //Set the last time the LED was run to the current time
    }
}

//runs with a fixed FPS, defined in TTD_LED_FPS
void LedGroupController::FixedFPSLedLoop()
{
    
    switch (ledState)
    {
        case STATIC: // If the LEDs are static, do nothing
            break;
        case TODO: // If the LEDs are scheduled for a change, prepare everything
            memcpy(ledValues_Original, FastLedHWMemory_Current, OverallLEDCount*sizeof(CRGB)); //Copy the current LED's to the original LED's
            fadingAmount = 0; //Set the fading amount to 0
            ledState = RUNNING; // Set the LED state to running
            break;
        case RUNNING: // If the LEDs are running, update them
            if (fadingAmount > 255)
                fadingAmount = 255; //If the fading amount is greater than 255, set it to 255

            blend( ledValues_Original, ledValues_Target, FastLedHWMemory_Current, OverallLEDCount,  (fract8)fadingAmount); //Blend the LED's to the target color
            fadingAmount += TTD_LED_TRANSITION_DELTA; //Increase the fading amount
            if (fadingAmount >= 255) //If the fading amount is greater than or equal to 255, set the LED's to the target color
            {
                memcpy(FastLedHWMemory_Current, ledValues_Target, OverallLEDCount*sizeof(CRGB)); //Copy the target LED's to the current LED's
                ledState = STATIC; //Set the LED state to static
            }
            FastLED.show(); //Show the LED's
            break;   
        default:
            break;
    }
}

void LedGroupController::setActiveBrightness(uint8_t groupIndex, uint8_t brightness)
{
    if (groupIndex < groupCount && groups[groupIndex] != nullptr)
    {
        groups[groupIndex]->BrightnessActive = brightness; // Set the base brightness for ON state for the specified group
        ledState = TODO; // Set the LED state to TODO, indicating that a change is scheduled
    }
}

void LedGroupController::setIDLEBrightness(uint8_t groupIndex, uint8_t brightness)
{
    if (groupIndex < groupCount && groups[groupIndex] != nullptr)
    {
        groups[groupIndex]->BrightnessIDLE = brightness; // Set the base brightness for OFF state for the specified group
        ledState = TODO; // Set the LED state to TODO, indicating that a change is scheduled
    }
}