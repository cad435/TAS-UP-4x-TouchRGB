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

        //enable proximity detection on all 4 touch pads
        uint8_t proxChannels[] = CAP1188_PROX_CHANNELS;
        cap->enableProximityDetection(proxChannels, CAP1188_PROX_CHANNELS_COUNT);
        logDebugP("Proximity detection enabled on %i channels", CAP1188_PROX_CHANNELS_COUNT);


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

    LedController = new LedGroupController(TTD_LEDGROUP_COUNT, grpsz); //Create a new instance of the LedGroupController class
    LedGroupFunction ledgrp_fkt[] = TTD_LEDGROUP_FUNCTIONS; //buffer the group functions

    uint8_t PixelIndex = 0;

    //Initialize each LED-Group
    for (uint8_t i = 0; i < LedController->getOverallLedGroupCount(); i++)
    {
        LedController->AddLedGroup(grpsz[i], ledgrp_fkt[i], ledHelper->rgb565ToCRGB(ParamTTD_LEDColor)); //Initialize the group with the LED indices
        logDebugP("LedGroup %i initialized with %i LEDs", i, grpsz[i]); //Print the string to the debug output

        PixelIndex += grpsz[i]; //Increment the pixel index
    }

    //further define each LED-Group initially
    //get the colors from the KNX Parameters
    CRGB col = ledHelper->rgb565ToCRGB(ParamTTD_LEDColor); //Convert the color from the parameter to CRGB
    uint8_t brightness_ON = PercentToUint8(ParamTTD_LEDBrightness_Active); //Get the brightness from the parameter
    uint8_t brightness_OFF = PercentToUint8(ParamTTD_LEDBrightness_IDLE); //Get the brightness from the parameter
    
    
    for (uint8_t i = 0; i < TTD_LEDGROUP_COUNT; i++)
    {
        LedGroup* group = LedController->getLedGroup(i);

        //this use of directly accessing the groups parameters should only be used when in a setup scenario as it will completely skip the state-machine of the controller 
        //
        group->maxBrightness = brightness_ON; //Set the maximum brightness
        group->minBrightness = brightness_OFF; //Set the minimum brightness
    }

    LedGroup* group = LedController->getLedGroup(0);

    //debug output
    logDebugP("LEDColor: %i|%i|%i", group->color.r, group->color.g, group->color.b); //Print the color to the debug output
    logDebugP("LEDBrightness_Active: %i", group->maxBrightness); //Print the brightness to the debug output
    logDebugP("LEDBrightness_IDLE: %i", group->minBrightness); //Print the brightness to the debug output
    //Set the group color and set it to IDLE
    //will also start of the state-machine
    LedController->setColor(col); //Set the color of the LED's to the value from the parameter
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
    //fire here as fast as we can, will be executed when openKNX feels like.    
    if(openknx.freeLoopTime())
    {
        cap->evaluate(); //Evaluate the CAP1188
    }
    
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
    

    if(openknx.freeLoopTime() && cap->isProximityDetected()) //If proximity is sensed, change the KO
    {
        KoTTD_ProximitySensed.value(true, Dpt(1,1)); //Send the proximity value to the KO
        logDebugP("Proximity detected! KO %i", KoTTD_ProximitySensed.asap()); //Print the proximity value to the debug output
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
    case TTD_KoLEDColor:
    {
        //serialLed->setLEDTargetColor(ledHelper->DPT_Colour_RGB_to_CRGB(KoTTD_LEDColor.value(DPT_Colour_RGB)));
        for (uint8_t i = 0; i < LedController->getOverallLedGroupCount(); i++)
        {
            LedGroup* g = LedController->getLedGroup(i);
            g->color = ledHelper->DPT_Colour_RGB_to_CRGB(KoTTD_LEDColor.value(DPT_Colour_RGB)); //Set the color of the LED group to the value from the parameter
        }
        break;
    }
    case TTD_KoLEDBrightness_Active:
    {
        uint8_t briActive = KoTTD_LEDBrightness_Active.value(DPT_Scaling); //Get the brightness from the parameter
        for (uint8_t i = 0; i < LedController->getOverallLedGroupCount(); i++)
        {
            LedGroup* g = LedController->getLedGroup(i);
            g->maxBrightness = briActive; //Set the brightness of the LED's to the value from the parameter
            g->isActive = true; //Set the group to ACTIVE after startup

        }
        break;
    }
    case TTD_KoLEDBrightness_IDLE:
    {
        uint8_t briIdle = KoTTD_LEDBrightness_IDLE.value(DPT_Scaling); //Get the brightness from the parameter
        for (uint8_t i = 0; i < LedController->getOverallLedGroupCount(); i++)
        {
            LedGroup* g = LedController->getLedGroup(i);
            g->minBrightness = briIdle; //Set the brightness of the LED's to the value from the parameter
            g->isActive = false; //Set the group to IDLE after startup
        }
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