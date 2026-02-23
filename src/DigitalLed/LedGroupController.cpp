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

    //set FastledPhysicalMemory to black/off
    for (uint8_t i = 0; i < OverallLedCount; i++)
    {
        FastLedPhysicalMemory[i] = CRGB(0,0,0);
        PreviousPixelArray[i] = CRGB(0,0,0);
        TargetPixelArray[i] = CRGB(0,0,0);
    }

    FastLED.addLeds<TTD_LED_TYPE, TTD_RGB_DIO_PIN, TTD_BASE_COLORORDER>(FastLedPhysicalMemory, OverallLedCount);

    BaseColorOrder = TTD_BASE_COLORORDER; //Set the base color order for the controller, this is used for the groups which do not have a specific color order set.

}

void LedGroupController::AddLedGroup(uint8_t _ledCount, LedGroupFunction _function, CRGB color, EOrder colorOrder) {
    // Create a new LedGroug

    if (InitializedGroups >= OverallLedGroupCount)
    {
        //logDebugP("Cannot intialize ned LED group with idx %i as there is a maximum number of %i Groups specified!", InitializedGroups, OverallLedCount); //Print the string to the debug output
        return;
    }
    

    ledGroups[InitializedGroups].length = _ledCount;
    ledGroups[InitializedGroups].function = _function;
    ledGroups[InitializedGroups].colorOrder = colorOrder; //set color order before setColor, as setColor reads it
    setColor(&ledGroups[InitializedGroups], color); //set the color with color order remapping
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
    } else if(_function == LedGroupFunction::ZERO)
    {
        for (uint8_t i = 0; i < _ledCount; i++)
            ledGroups[InitializedGroups].fkt_preRendered[i] = 0; // ZERO function
    }
    else if(_function == LedGroupFunction::GAUSSIAN)
    {
        for (uint8_t i = 0; i < _ledCount; i++) {
            //gaussian function centered in the middle of the group, with a standard deviation of 1/6 of the group length, so that it covers most of the group
            float x = static_cast<float>(i) - static_cast<float>(_ledCount) / 2.0f;
            float sigma = static_cast<float>(_ledCount) / 6.0f;
            ledGroups[InitializedGroups].fkt_preRendered[i] = static_cast<uint8_t>(std::exp(-(x * x) / (2 * sigma * sigma)) * 255); // Gaussian function
        }
    }
    else
    {
        for (uint8_t i = 0; i < _ledCount; i++)
            ledGroups[InitializedGroups].fkt_preRendered[i] = 255; // Flat function
    }

    InitializedGroups++;

}


void LedGroupController::setAllGroupsActive(bool isActive) {

    if(isActive)
    {
        for (uint8_t i = 0; i < OverallLedGroupCount; i++) {
            ledGroups[i].luminosityState = LuminosityState::BRIGHT; //if we want to set all groups active, set their luminosity state to bright
        }
        state = ControllerState::CHANGE_SHEDULED; //immediately shedule a change when going from inactive to active, so the LED's will update to the new brightness without delay
    }
    else
    {
        //if we want to set all groups inactive, set their luminosity state to dim. 
        //Rest of the logic for delayed dimming will be handled in the Process function, which will start the delay when it sees a group with DELAY_START and then set the group to DIM after the delay time has passed
        for (uint8_t i = 0; i < OverallLedGroupCount; i++) {
            ledGroups[i].luminosityState = LuminosityState::DELAY_START; 
        }
    }


}

void LedGroupController::setColor(LedGroup* grp, CRGB color) {

    if (grp->colorOrder == BaseColorOrder) {
        //no remapping needed, just set the color directly
        grp->color = color;
    } else {
        //the group has a different color order than the base, we need to remap the channels
        //
        //EOrder encodes channel positions as 3 octal digits: (RedPos)(GreenPos)(BluePos)
        //e.g. GRB = 0102 octal → Red at wire[1], Green at wire[0], Blue at wire[2]
        //
        //Strategy: build the desired wire byte layout for the group's hardware,
        //then read back what CRGB values FastLED needs (with base order) to produce those wire bytes.

        //build desired wire bytes: place R,G,B where the group hardware expects them
        uint8_t wire[3];
        wire[(grp->colorOrder >> 6) & 0x07] = color.r; //Red position
        wire[(grp->colorOrder >> 3) & 0x07] = color.g; //Green position
        wire[ grp->colorOrder       & 0x07] = color.b; //Blue position

        //read back what CRGB must contain so FastLED (with base order) produces the correct wire layout
        grp->color = CRGB(
            wire[(BaseColorOrder >> 6) & 0x07], //what FastLED reads as R
            wire[(BaseColorOrder >> 3) & 0x07], //what FastLED reads as G
            wire[ BaseColorOrder       & 0x07]  //what FastLED reads as B
        );
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

    //main state machine for handling the LED updates and transitions
    switch (state)
    {
    case IDLE:
        // Do nothing
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
            memcpy(&(TargetPixelArray[StartIdx]), ledGroups[i].PixelStorage, ledGroups[i].length*sizeof(CRGB));
            StartIdx += ledGroups[i].length;
        }
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

        FastLED.show(); //make a last call to display the LED's
        //Serial.println("CHANGE_FINISHED");
        state = ControllerState::IDLE;
        break;
    default:
        break;
    }


    //only use the Luminosity State of the first group for evaluation, otherwise it'll get to complex for no real benefit
    switch(ledGroups[0].luminosityState)
    {
        case LuminosityState::BRIGHT:
            //do nothing, the group is already bright
            break;
        case LuminosityState::DELAY_START:
            //Serial.printf("Starting proximity lost delay, all groups will dim in %i ms if proximity is not regained!\n", ProximityLostDelay_ms); //Print the string to the debug output
            //if we just entered the delay start, we need to set the delay ticks
            ProximityLostDelayTimestamp = millis();

            for(uint8_t i = 0; i < OverallLedGroupCount; i++) //Set all other groups to delay as well, so they stay bright during the delay time
                ledGroups[i].luminosityState = LuminosityState::DELAY;
            break;
        case LuminosityState::DELAY:
            //if we are in delay, we need to check if the delay time has passed and if so, set the group to dim
            if (millis() - ProximityLostDelayTimestamp > ProximityLostDelay_ms) //delay expired
            {
                for (uint8_t i = 0; i < OverallLedGroupCount; i++)
                    ledGroups[i].luminosityState = LuminosityState::DIM; //set all groups to dim
                state = ControllerState::CHANGE_SHEDULED; //schedule a change to update the LED's
                //Serial.println("Proximity lost delay expired, all groups dimmed!"); //Print the string to the debug output
            }
            
            break;
        case LuminosityState::DIM:
            //do nothing, the group is already dim
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


        uint8_t brightness = 0;
        if(group.luminosityState == LuminosityState::BRIGHT || group.luminosityState == LuminosityState::DELAY)
            brightness = group.maxBrightness; //if the group is currently bright or in delay, use max brightness
        else
            brightness = group.minBrightness; //if the group is currently dim, use min brightness
           
         


        for (uint8_t pixel = 0; pixel < group.length; pixel++) {

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