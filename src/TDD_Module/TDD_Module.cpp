#include "TDD_Module.h"
#include "ModuleVersionCheck.h"


//empty constructor
TDD_Module::TDD_Module() { }

const std::string TDD_Module::name()
{
    return "TDD_Module";
}

const std::string TDD_Module::version()
{
    return MODULE_VirtualButton_Version;
}

void TDD_Module::setup()
{
    logDebugP("setup(), CPU: %i", get_core_num()); 

#pragma region CAP1188-Initialisation

    /**  Initialisation of CAP1188 **/
    cap = new CAP1188(CAP1188_RST_PIN);

    if (cap->begin(CAP1188_I2CADDR, new TwoWire(&CAP1188_I2C_WIRECLASS, CAP1188_I2C_SDA, CAP1188_I2C_SCL)))
    {
        logDebugP("CAP1188 begin success");
        uint8_t GlobalSen = ParamTTD_CAP_Sensitivity; //Get the sensitivity from the parameter
        cap->SetGlobalSensitivity(GlobalSen); //Set the sensitivity of the CAP1188
        logDebugP("SetGlobalSensitivity: %i", GlobalSen); //Print the sensitivity to the debug output


        
        bool disableAnalogFilter = ParamTTD_CAP_EnableAnalogFilter; //Get the analog filter from the parameter
        cap->disableAnalogNoiseFilter(!disableAnalogFilter); //Enable the analog filter
        logDebugP("disableAnalogNoiseFilter: !%i", disableAnalogFilter); //Print the analog filter to the debug output


        //read the proximity threshold from the parameter and set it to the CAP1188
        cap->setProximityThreshold(ParamTTD_TTDProximityThreshold); //Set the proximity threshold
        logDebugP("proximityThreshold: %i", ParamTTD_TTDProximityThreshold); //Print the string to the debug output


        uint8_t touchThreshold = ParamTTD_TTDTouchThreshold;
        for (int i = 0; i < 8; i++)
            cap->setTouchThreshold(i, touchThreshold); //Set the touch threshold for each pin

        logDebugP("TouchThreshold: %i", touchThreshold); //Print the string to the debug output

        uint8_t MTP[] = CAP1188_MTP_CHANNELS; //Define the multiple touch tap pattern
        cap->enableMultipleTouchTapPattern(MTP, CAP1188_MTP_CHANNELS_COUNT); //Define the multiple touch tap pattern
    }
    else
    {
        logDebugP("CAP1188 begin failed, init skipped. TTD Module will not work!");
    }


#pragma endregion CAP1188-Initialisation

#pragma region LED-Initialisation


    /**  Initialisation of LED's **/
    logDebugP("LED-Initialisation"); //Print the string to the debug output

    uint8_t grpsz[] = TTD_LEDGROUP_SIZE; //buffer the group sizes
    EOrder grpcolorOrder[] = TTD_LEDGROUP_COLORORDER; //buffer the color orders

    LedController = new LedGroupController(TTD_LEDGROUP_COUNT, grpsz); //Create a new instance of the LedGroupController class
    LedGroupFunction ledgrp_fkt[] = TTD_LEDGROUP_FUNCTIONS; //buffer the group functions

    uint8_t PixelIndex = 0;

    //Initialize each LED-Group with inner/outer color
    //Get the color from the parameter, this is used for all groups, but the inner group will be brighter than the outer groups
    CRGB col[] = {ledHelper->rgb565ToCRGB(ParamTTD_LEDColorOuter), ledHelper->rgb565ToCRGB(ParamTTD_LEDColor), ledHelper->rgb565ToCRGB(ParamTTD_LEDColorOuter)}; 
    for (uint8_t i = 0; i < LedController->getOverallLedGroupCount(); i++)
    {
        CRGB groupColor = col[i]; //Group 1 = inner (Innenleiste), Groups 0 & 2 = outer (Außen)
        LedController->AddLedGroup(grpsz[i], ledgrp_fkt[i], groupColor, grpcolorOrder[i]); //Initialize the group with the LED indices
        logDebugP("LedGroup %i initialized with %i LEDs", i, grpsz[i]); //Print the string to the debug output

        PixelIndex += grpsz[i]; //Increment the pixel index
    }
    uint8_t brightness_ON_Inner = PercentToUint8(ParamTTD_LEDBrightness_Active); //Get inner brightness from the parameter
    uint8_t brightness_OFF_Inner = PercentToUint8(ParamTTD_LEDBrightness_IDLE); //Get inner brightness from the parameter
    uint8_t brightness_ON_Outer = PercentToUint8(ParamTTD_LEDBrightness_Active_Outer); //Get outer brightness from the parameter
    uint8_t brightness_OFF_Outer = PercentToUint8(ParamTTD_LEDBrightness_IDLE_Outer); //Get outer brightness from the parameter

    //this use of directly accessing the groups parameters should only be used when in a setup scenario as it will completely skip the state-machine of the controller
    // Group 0 = outer, Group 1 = inner, Group 2 = outer
    LedController->getLedGroup(0)->maxBrightness = brightness_ON_Outer;
    LedController->getLedGroup(0)->minBrightness = brightness_OFF_Outer;
    LedController->getLedGroup(1)->maxBrightness = brightness_ON_Inner;
    LedController->getLedGroup(1)->minBrightness = brightness_OFF_Inner;
    LedController->getLedGroup(2)->maxBrightness = brightness_ON_Outer;
    LedController->getLedGroup(2)->minBrightness = brightness_OFF_Outer;

    //debug output
    logDebugP("LEDColor Outer: %i|%i|%i", col[0].r, col[0].g, col[0].b); //Print the outer color to the debug output
    logDebugP("LEDColor Inner: %i|%i|%i", col[1].r, col[1].g, col[1].b); //Print the inner color to the debug output
    logDebugP("LEDBrightness Inner Active: %i, IDLE: %i", brightness_ON_Inner, brightness_OFF_Inner);
    logDebugP("LEDBrightness Outer Active: %i, IDLE: %i", brightness_ON_Outer, brightness_OFF_Outer);
    //Set the group to IDLE, will also start the state-machine
    LedController->setGroupActive(false); //Set the group to IDLE

#pragma endregion LED-Initialisation

    setupComplete = true; // Set the setup complete flag to true
    logDebugP("Setup Done");

}

void TDD_Module::processAfterStartupDelay()
{
    logDebugP("processAfterStartupDelay");
    delay(1000); //Wait for 1 second
}

// Handles FunctionProperty requests from ETS scripts via KNX bus.
// This is the firmware-side counterpart to the ETS JavaScript function TTD_readDeltaCounts().
//
// Protocol:
//   objectIndex = 161 (TDD_Module — next free after 158=Common, 159=FileTransfer, 160=Logic)
//   propertyId  = 1
//   Command 0:  Read raw CAP1188 delta counts for all 4 touch pads (A-D)
//
// Request format:  data[0] = 0 (command byte: read delta counts)
// Response format: [0, deltaA, deltaB, deltaC, deltaD]
//   - Byte 0: Status (0 = success)
//   - Bytes 1-4: Raw delta counts as uint8_t (int8_t cast to uint8_t, preserving bit pattern)
//     The ETS script converts these back to signed values for display.
//
// Thread safety: Both this function and cap->evaluate() run on Core 0, so no concurrency issue.
bool TDD_Module::processFunctionProperty(uint8_t objectIndex, uint8_t propertyId, uint8_t length, uint8_t *data, uint8_t *resultData, uint8_t &resultLength)
{
    // Only handle requests addressed to TDD_Module (objectIndex 161, propertyId 1)
    if (objectIndex != 161 || propertyId != 1)
        return false;

    if (data[0] == 0) // Command 0: read raw delta counts from CAP1188
    {
        resultData[0] = 0; // Status byte: 0 = success
        resultData[1] = (uint8_t)cap->getRawDeltaCount(0); // Pad A — cast int8_t to uint8_t for transmission
        resultData[2] = (uint8_t)cap->getRawDeltaCount(1); // Pad B
        resultData[3] = (uint8_t)cap->getRawDeltaCount(2); // Pad C
        resultData[4] = (uint8_t)cap->getRawDeltaCount(3); // Pad D
        resultLength = 5;
        logDebugP("FunctionProperty: Delta Counts A:%d B:%d C:%d D:%d",
            cap->getRawDeltaCount(0), cap->getRawDeltaCount(1),
            cap->getRawDeltaCount(2), cap->getRawDeltaCount(3));
        return true;
    }
    return false; // Unknown command
}

void TDD_Module::setup1()
{
    logDebugP("setup1(), CPU: %i", get_core_num()); 
    while(!setupComplete); // wait till core0 has completed the setup

    //this will than proceed to loop1();
}


//handles all the capacitive things and the actual dataflows
//this is essentially the loop which handles the outgoing KNX-Messages
void TDD_Module::loop()
{

    //logDebugP("Delta: %4d %4d %4d %4d %4d %4d %4d %4d", cap->getRawDeltaCount(0), cap->getRawDeltaCount(1), cap->getRawDeltaCount(2), cap->getRawDeltaCount(3), cap->getRawDeltaCount(4), cap->getRawDeltaCount(5), cap->getRawDeltaCount(6), cap->getRawDeltaCount(7));

    //fire here as fast as we can, will be executed when openKNX feels like.    
    if(openknx.freeLoopTime())
    {
        cap->evaluate(); //Evaluate the CAP1188
    }

    //Debugprint hasChanged for each pad in one line
    //logDebugP("hasChanged: %i %i %i %i %i %i %i %i", cap->hasChanged(CAP1188::Pad_A), cap->hasChanged(CAP1188::Pad_B), cap->hasChanged(CAP1188::Pad_C), cap->hasChanged(CAP1188::Pad_D), cap->hasChanged(CAP1188::Pad_E), cap->hasChanged(CAP1188::Pad_F), cap->hasChanged(CAP1188::Pad_G), cap->hasChanged(CAP1188::Pad_H));
    
    //debugprint free loop time
    //logDebugP("freeLoopTime: %i", openknx.freeLoopTime());
    

    if(openknx.freeLoopTime() && cap->hasChanged(CAP1188::Pad_A)) //If the pad A has Changed, change the KO
    {
        KoTTD_StatePAD_A.value(cap->isTouched(CAP1188::Pad_A), Dpt(1,1)); //Send the touch value to the KO
        logDebugP("PAD_A Value %i to KO %i", cap->isTouched(CAP1188::Pad_A), KoTTD_StatePAD_A.asap()); //Print the touch value to the debug output
    }
            
    
    if(openknx.freeLoopTime() && cap->hasChanged(CAP1188::Pad_B)) //If the pad B has Changed, change the KO
    {
        KoTTD_StatePAD_B.value(cap->isTouched(CAP1188::Pad_B), Dpt(1,1)); //Send the touch value to the KO
        logDebugP("PAD_B Value %i to KO %i", cap->isTouched(CAP1188::Pad_B), KoTTD_StatePAD_B.asap()); //Print the touch value to the debug output
    }
            

    if(openknx.freeLoopTime() && cap->hasChanged(CAP1188::Pad_C)) //If the pad C has Changed, change the KO
    {
        KoTTD_StatePAD_C.value(cap->isTouched(CAP1188::Pad_C), Dpt(1,1)); //Send the touch value to the KO
        logDebugP("PAD_C Value %i to KO %i", cap->isTouched(CAP1188::Pad_C), KoTTD_StatePAD_C.asap()); //Print the touch value to the debug output
    }
                         
    if(openknx.freeLoopTime() && cap->hasChanged(CAP1188::Pad_D)) //If the pad D has Changed, change the KO
    {
        KoTTD_StatePAD_D.value(cap->isTouched(CAP1188::Pad_D), Dpt(1,1)); //Send the touch value to the KO
        logDebugP("PAD_D Value %i to KO %i", cap->isTouched(CAP1188::Pad_D), KoTTD_StatePAD_D.asap()); //Print the touch value to the debug output
    }     
    

    if(openknx.freeLoopTime() && cap->isProximityChanged()) //If proximity state changed since last evaluate, update the KO
    {
        KoTTD_ProximitySensed.value(cap->isProximityDetected(), Dpt(1,1)); //Send the current proximity value to the KO
        logDebugP("Proximity changed to %i, KO %i", cap->isProximityDetected(), KoTTD_ProximitySensed.asap()); //Print the proximity value to the debug output
    }

    if(openknx.freeLoopTime() && cap->TapHappened) //If a tap happened, change the KO
    {
        //KoTTD_StateTap.value(cap->TapHappened, Dpt(1,1)); //Send the tap value to the KO
        //logDebugP("Tap Value %i to KO %i", cap->TapHappened, KoTTD_StateTap.asap()); //Print the tap value to the debug output
        logDebugP("Tap happened, but not implemented yet!"); //Print a debug message that tap happened, but not implemented yet
    }




}

//handles all the "shiny" things, eg. LED-Animations and so on
void TDD_Module::loop1()
{
    //call as fast as possible, will be only executed every 20ms
    if(openknx.freeLoopTime())
    {
        LedController->evaluate(); //Evaluate the LedGroupController
    }
}

//this is essentially the place where messages (incoming KO's) are handled and processed
void TDD_Module::processInputKo(GroupObject& iKo)
{
    switch(iKo.asap())
    {
    case TTD_KoLEDColor: //Farbe LED Innenleiste (group 1)
    {
        CRGB colInner = ledHelper->DPT_Colour_RGB_to_CRGB(KoTTD_LEDColor.value(DPT_Colour_RGB));
        logDebugP("LEDColor Inner changed to %i|%i|%i", colInner.r, colInner.g, colInner.b); //Print the inner color to the debug output
        LedController->setColor(LedController->getLedGroup(1), colInner); //Inner group = group 1
        break;
    }
    case TTD_KoLEDColorOuter: //Farbe LED Außen (groups 0 and 2)
    {
        CRGB colOuter = ledHelper->DPT_Colour_RGB_to_CRGB(KoTTD_LEDColorOuter.value(DPT_Colour_RGB));
        LedController->setColor(LedController->getLedGroup(0), colOuter); //Outer group 0
        LedController->setColor(LedController->getLedGroup(2), colOuter); //Outer group 2
        break;
    }
    case TTD_KoLEDBrightness_Active: //Helligkeit Innenleiste hell (group 1)
    {
        uint8_t briActive = KoTTD_LEDBrightness_Active.value(DPT_Scaling);
        LedController->getLedGroup(1)->maxBrightness = briActive;
        LedController->getLedGroup(1)->isActive = true;
        break;
    }
    case TTD_KoLEDBrightness_IDLE: //Helligkeit Innenleiste dunkel (group 1)
    {
        uint8_t briIdle = KoTTD_LEDBrightness_IDLE.value(DPT_Scaling);
        LedController->getLedGroup(1)->minBrightness = briIdle;
        LedController->getLedGroup(1)->isActive = false;
        break;
    }
    case TTD_KoLEDBrightness_Active_Outer: //Helligkeit Außen hell (groups 0 and 2)
    {
        uint8_t briActive = KoTTD_LEDBrightness_Active_Outer.value(DPT_Scaling);
        LedController->getLedGroup(0)->maxBrightness = briActive;
        LedController->getLedGroup(0)->isActive = true;
        LedController->getLedGroup(2)->maxBrightness = briActive;
        LedController->getLedGroup(2)->isActive = true;
        break;
    }
    case TTD_KoLEDBrightness_IDLE_Outer: //Helligkeit Außen dunkel (groups 0 and 2)
    {
        uint8_t briIdle = KoTTD_LEDBrightness_IDLE_Outer.value(DPT_Scaling);
        LedController->getLedGroup(0)->minBrightness = briIdle;
        LedController->getLedGroup(0)->isActive = false;
        LedController->getLedGroup(2)->minBrightness = briIdle;
        LedController->getLedGroup(2)->isActive = false;
        break;
    }
    default:
        break;
    }
}

int32_t TDD_Module::StringParam2Num(uint8_t* ParamData)
{
    //read the proximity threshold from the parameter and set it to the CAP1188
    String str = "";
    for (uint8_t i = 0; i < 32; i++) //safety-Guard, max 32 chars
    {
        if ((char)ParamData[i] == '\0') //If the character is null, break the loop
            break;
            str += (char)ParamData[i]; //Copy the string to the string variable

        if (i==31)
            return -1; //If the string is too long, return -1
    }   

    return str.toInt(); //Convert the string to int
}


TDD_Module openknxTDD_Module;