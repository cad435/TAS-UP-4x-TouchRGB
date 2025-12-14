#include "LedGroupController.h"


LedGroupController::LedGroupController() {}

LedGroupController::LedGroupController(uint8_t _numLedGroups, uint8_t* _ledGroupSizes) {
    OverallLedGroupCount = _numLedGroups;
    ledGroups = new LedGroup[_numLedGroups];

    //calculate the count of LED's in the complete chain
    OverallLedCount = 0;
    for (uint8_t i = 0; i < OverallLedGroupCount; i++)
    {
        OverallLedCount += _ledGroupSizes[i];
    }
    
    FastLedPhysicalMemory = new CRGB[OverallLedCount]; //Allocate memory for the complete LED chain
    PreviousPixelArray = new CRGB[OverallLedCount];
    TargetPixelArray = new CRGB[OverallLedCount];

    FastLED.addLeds<TTD_LED_TYPE, TTD_RGB_DIO_PIN, TTD_RGB_ORDER>(FastLedPhysicalMemory, OverallLedCount);

}

void LedGroupController::AddLedGroup(uint8_t _ledCount, LedGroupFunction _function, CRGB color) {
    // Create a new LedGroug

    if (InitializedGroups >= OverallLedCount)
    {
        //logDebugP("Cannot intialize ned LED group with idx %i as there is a maximum number of %i Groups specified!", InitializedGroups, OverallLedCount); //Print the string to the debug output
        return;
    }
    

    ledGroups[InitializedGroups].lenght = _ledCount;
    ledGroups[InitializedGroups].function = _function;
    ledGroups[InitializedGroups].color = color;
    ledGroups[InitializedGroups].PixelStorage = new CRGB[_ledCount];

    // Allocate memory for function divisor based on function type
    ledGroups[InitializedGroups].fkt_preRendered = new int16_t[_ledCount];

    if (_function == LedGroupFunction::SINE) {
        for (uint8_t i = 0; i < _ledCount; i++) {
            //sin(i/pi*10/_ledCount)*255
            ledGroups[InitializedGroups].fkt_preRendered[i] = static_cast<uint8_t>(std::sin((static_cast<float>(i) / PI * 10.0f / _ledCount)) * 255); // Cosine wave from 0 to 255

            if(ledGroups[InitializedGroups].fkt_preRendered[i] < 0)
                ledGroups[InitializedGroups].fkt_preRendered[i] = 0; // Clamp to 0
        }
    } else {
        for (uint8_t i = 0; i < _ledCount; i++)
            ledGroups[InitializedGroups].fkt_preRendered[i] = 255; // Flat function
    }

    InitializedGroups++;

}


void LedGroupController::setGroupActive(bool isActive) {
    for (uint8_t i = 0; i < OverallLedGroupCount; i++) {
        ledGroups[i].isActive = isActive;
    }
    state = ControllerState::CHANGE_SHEDULED;
}

void LedGroupController::setColor(CRGB color) {
    for (uint8_t i = 0; i < OverallLedGroupCount; i++) {
        ledGroups[i].color = color;
    }
    state = ControllerState::CHANGE_SHEDULED;
}

void LedGroupController::evaluate()
{
    //get the current time
    uint32_t currentTime = millis();
    //check if it's time for the next frame
    if (currentTime - LastFrameTime >= FrameTime_ms)
    {
        LastFrameTime = currentTime;
        Process();
    }

}

void LedGroupController::Process() {
    // Process all LED groups and update their states
    // This function would contain the logic to update the LED states based on their functions and active status

    uint8_t StartIdx = 0;

    switch (state)
    {
    case IDLE:
        // Do nothing
        return;
        break;
    case CHANGE_SHEDULED:
        //Serial.println("CHANGE_SHEDULED");
        // Start changing
        //                     <------
        memcpy(PreviousPixelArray, FastLedPhysicalMemory, OverallLedCount*sizeof(CRGB)); //Copy current pixel data to previous
        //Here we would calculate the TargetPixelArray based on the new settings
        CalculatePixelWithFunction(); //Calculate the pixel values based on the functions of each group

        //now every group.PixelStorage has the Target values calculated. The Actual FastLed Memory is untouched 
        //transfer the group.PixelStorage data into the TargetArray Color
        StartIdx = 0;
        for (size_t i = 0; i < OverallLedGroupCount; i++)
        {
            //memcopy each array onto the LED-TargetArray
            //                              <------
            memcpy(&(TargetPixelArray[StartIdx]), ledGroups[i].PixelStorage, ledGroups[i].lenght*sizeof(CRGB));
            StartIdx += ledGroups[i].lenght;
        }
        /*
        Serial.println("BLOCK STARTED");
        for (uint8_t i = 0; i < OverallLedCount; i++)
        {
            Serial.print(TargetPixelArray[i].r);
            Serial.print("|");
            Serial.print(TargetPixelArray[i].g);
            Serial.print("|");
            Serial.print(TargetPixelArray[i].b);

            Serial.print("#");
        }
        Serial.println();
        Serial.println("BLOCK FINISHED");
        */
        //reset the fading amount variable
        fadingAmount = 0;
        //change to the changing event
        state = CHANGING;
        break;
    case CHANGING:
        if (fadingAmount > 255)
                fadingAmount = 255; //If the fading amount is greater than 255, set it to 255

        blend(PreviousPixelArray, TargetPixelArray, FastLedPhysicalMemory, OverallLedCount, (fract8)fadingAmount); //Blend the LED's to the target color
        FastLED.show(); //Show the LED's

        fadingAmount += TTD_LED_TRANSITION_DELTA; //Increase the fading amount

        if (fadingAmount >= 255) //If the fading amount is greater than or equal to 255, set the LED's to the target color
            state = CHANGE_FINISHED; //Set the LED state to CHANGE_FINISHED
        break;
    case CHANGE_FINISHED:
        memcpy(FastLedPhysicalMemory, TargetPixelArray, OverallLedCount*sizeof(CRGB)); //Copy the target LED's to the current LED's

        /*Serial.println("BLOCK STARTED");
        for (uint8_t i = 0; i < OverallLedCount; i++)
        {
            Serial.print(FastLedPhysicalMemory[i].r);
            Serial.print("|");
            Serial.print(FastLedPhysicalMemory[i].g);
            Serial.print("|");
            Serial.print(FastLedPhysicalMemory[i].b);

            Serial.print("#");
        }
        Serial.println();
        Serial.println("BLOCK FINISHED");*/

        FastLED.show(); //make a last call to display the LED's
        //Serial.println("CHANGE_FINISHED");
        state = ControllerState::IDLE;
        break;
    default:
        break;
    }
}

void LedGroupController::CalculatePixelWithFunction() {
    // Calculate the pixel values for a given group based on its function and active status
    //this will be first stored into the actual FastLED memory
    for (uint8_t groupIndex = 0; groupIndex < OverallLedGroupCount; groupIndex++) {
        LedGroup& group = ledGroups[groupIndex];

        uint8_t brightness = group.isActive ? group.maxBrightness : group.minBrightness; //if group active use max brightness else min brightness
         
        //uint8_t brightness = group.minBrightness;

        //Serial.print("GRP BEGIN, Bri: ");
        //Serial.println(brightness);

        for (uint8_t pixel = 0; pixel < group.lenght; pixel++) {

            uint8_t divisor = group.fkt_preRendered[pixel]; // Get the pre-rendered function value for each pixel
            // Calculate final brightness
            uint8_t finalBrightness = ((uint16_t)brightness * (uint16_t)divisor) / 255;

            //Serial.print(finalBrightness);
            //Serial.print('|');

            // Set the pixel color with the calculated brightness
            group.PixelStorage[pixel] = CRGB(
                (uint8_t)((uint16_t)(group.color.r * finalBrightness) / 255),
                (uint8_t)((uint16_t)(group.color.g * finalBrightness) / 255),
                (uint8_t)((uint16_t)(group.color.b * finalBrightness) / 255)
            );


            /*Serial.print(group.PixelStorage[pixel].r);
            Serial.print("|");
            Serial.print(group.PixelStorage[pixel].g);
            Serial.print("|");
            Serial.print(Tgroup.PixelStorage[pixel].b);

            Serial.print("#");*/

        }
        //Serial.println();
        //Serial.println("GRP FIN");
    }    
}