/*!
 *  @file CAP1188.cpp
 *
 *
 *
 *  This library is originally based on the Adafruit CAP1188 library.
 * 
 * 	it is heavily modified for OpenKNX to add functionality by cad435
 *
 *  
 * 
 */

#include "CAP1188.h"

/*!
 *    @brief  Instantiates a new CAP1188 class using hardware I2C
 *    @param  resetpin
 *            number of pin where reset is connected
 *
 */
CAP1188::CAP1188(int8_t resetpin) {
  // I2C
  _resetpin = resetpin;
}

/*!
 *    @brief  Setups the i2c depending on selected mode (I2C / SPI, Software /
 * Hardware). Displays useful debug info, as well as allow multiple touches
 * (CAP1188_MTBLK), links leds to touches (CAP1188_LEDLINK), and increase the
 * cycle time value (CAP1188_STANDBYCFG)
 *    @param  i2caddr
 *            optional i2caddres (default to 0x29)
 *    @param  theWire
 *            optional wire object
 *    @return True if initialization was successful, otherwise false.
 */
boolean CAP1188::begin(uint8_t i2caddr, TwoWire *theWire) {
  
  Evaluate_TimeDelay_ms = 1000 / EvaluateFrequency;

  _addr = i2caddr;

  _wire = theWire;

  _wire->setSCL(CAP1188_I2C_SCL);
  _wire->setSDA(CAP1188_I2C_SDA);

  _wire->begin();
  _wire->setClock(400000); // I2C fast mode (400kHz), max supported by CAP1188

  if (_resetpin != -1) {
    pinMode(_resetpin, OUTPUT);
    digitalWrite(_resetpin, LOW);
    delay(100);
    digitalWrite(_resetpin, HIGH);
    delay(100);
    digitalWrite(_resetpin, LOW);
    delay(100);
  }


  if ((readRegister(CAP1188_PRODUCT_ID) != 0x50) ||(readRegister(CAP1188_MANUFACTURER_ID) != 0x5D)) {
    logTraceP("Failed to find expected CAP1188, check wiring?");
    return false;
  }

  logTraceP("Found CAP1188 with HW-Rev:: " + String(readRegister(CAP1188_REVISION), HEX));

  // allow multiple touches
  writeRegister(CAP1188_MULTIPLE_TOUCH_CONFIG, 0);
  // Have LEDs follow touches
  writeRegister(CAP1188_SENSOR_INPUT_LED_LINK, 0xFF);
  // speed up a bit
  writeRegister(CAP1188_STANDBY_CONFIG, 0x30);

  return true;
}


void CAP1188::evaluate()
{


    //get the current time
    uint32_t currentTime = millis();
    //check if it's time for the next frame
    if (currentTime - _LastEvalTime >= Evaluate_TimeDelay_ms)        
      _LastEvalTime = currentTime; 
      //continue with the evaluation of the touch status
    else      
      return;//if it's not time for the next frame, return early and do nothing



  uint8_t reg = ReadTouched();
  //get the bits for the channel out of the register and write it to the array
  for (uint8_t i = 0; i < 8; i++)
  {
    //extract a single boolean bit from the register
    uint8_t mask = (1 << i);
    bool CurrentlyTouched = (reg & mask) >> i;

    //if the last state is different from the current state, set the changed flag to true
    if(ChannelTouched[i] != CurrentlyTouched)
      ChannelChangedSinceLastEvaluate[i] = true;
    else
      ChannelChangedSinceLastEvaluate[i] = false;
    
    // save the current state to the array
    ChannelTouched[i] = CurrentlyTouched;
  }

  //read raw delta counts for pads A-D
  for (uint8_t i = 0; i < 4; i++)
    RawDeltaCount[i] = getRawDeltaCount(i);

  //software proximity detection: sum absolute delta counts of pads A-D
  uint16_t deltaSum = 0;
  for (uint8_t i = 0; i < 4; i++)
    deltaSum += abs(RawDeltaCount[i]);

  //evaluate proximity state based on the sum of absolute delta counts
  bool proximityNow = (_proximityThreshold > 0) && (deltaSum >= _proximityThreshold);

  //if the last state is different from the current state, set the changed flag to true
  if(proximityNow != _proximitySensed)
    _proximityChangedSinceLastEvaluate = true;
  else
    _proximityChangedSinceLastEvaluate = false;

  // save the current state
  _proximitySensed = proximityNow;

  bool tapNow = readBit(CAP1188_GENERAL_STATUS, CAP1188_GENERAL_STATUS_MTP);

  if (tapNow != TapHappened)
    _tapChangedSinceLastEvaluate = true;
  else
    _tapChangedSinceLastEvaluate = false;
  TapHappened = tapNow;



}


/*!
 *   @brief  Reads the touched status (CAP1188_SENINPUTSTATUS)
 *   @return Returns read from CAP1188_SENINPUTSTATUS where 1 is touched, 0 not
 * touched.
 */
uint8_t CAP1188::ReadTouched() {
  uint8_t t = readRegister(CAP1188_SENSOR_INPUT_STATUS);

  // Always clear the INT bit in Main Control register (0x00).
  // The chip latches all status registers (Sensor Input Status, General Status incl. MTP bit)
  // until INT is cleared. Without this, the MTP bit stays stuck at 1 after releasing all pads
  // because the chip never gets the signal to update its status registers.
  writeRegister(CAP1188_MAIN_CONTROL, readRegister(CAP1188_MAIN_CONTROL) & ~CAP1188_MAIN_INTERRUPT);

  return t;
}

/*!
 *    @brief  Reads from selected register
 *    @param  reg
 *            register address
 *    @return
 */
uint8_t CAP1188::readRegister(uint8_t reg) {

  _wire->setSCL(CAP1188_I2C_SCL);
  _wire->setSDA(CAP1188_I2C_SDA);

  _wire->beginTransmission(_addr);
  _wire->write(reg);
  _wire->endTransmission();

  delayMicroseconds(100);

  _wire->requestFrom(_addr, 1);
  if (_wire->available()) {
    uint8_t d = _wire->read();
    delayMicroseconds(100);
    return d;
  }
  delayMicroseconds(100);
  return 0;
}

/*!
 *   @brief  Writes 8-bits to the specified destination register
 *   @param  reg
 *           register address
 *   @param  value
 *           value that will be written at selected register
 */
void CAP1188::writeRegister(uint8_t reg, uint8_t value) {

  _wire->setSCL(CAP1188_I2C_SCL);
  _wire->setSDA(CAP1188_I2C_SDA);

  _wire->beginTransmission(_addr);
  _wire->write(reg);
  _wire->write(value);
  _wire->endTransmission();

  delayMicroseconds(100);
}


int8_t CAP1188::getRawDeltaCount(uint8_t channel) {
  return readRegister(CAP1188_SENSOR_INPUT_1_DELTA + channel);
}


void CAP1188::SetGlobalSensitivity(uint8_t sens) {

  uint8_t current_value = readRegister(CAP1188_SENSITIVITY_CONTROL);


  current_value = (current_value & 0x8F) | ((sens<<4) & 0x70);

  writeRegister(CAP1188_SENSITIVITY_CONTROL, current_value);
  
}

void CAP1188::writeBit(uint8_t reg, uint8_t bit, bool value)
{
  uint8_t current_value = readRegister(reg);

  //Serial.println("Current Value: " + String(current_value, BIN));

  if (value)
    current_value |= (1 << bit);
  else
    current_value &= ~(1 << bit);

  writeRegister(reg, current_value);

  current_value = readRegister(reg);

  //Serial.println("New Value: " + String(current_value, BIN));  
}

bool CAP1188::readBit(uint8_t reg, uint8_t bit)
{
  uint8_t current_value = readRegister(reg);
  //Serial.println("Current Value: " + String(current_value, BIN));
  return (current_value & (1 << bit)) != 0;
}

void CAP1188::disableAnalogNoiseFilter(bool disable)
{
  writeBit(CAP1188_CONFIGURATION, CAP1188_CONFIG_DIS_ANA_NOISE, disable);
}


//channel numbers starting form 0!!
void CAP1188::setTouchThreshold(uint8_t channel, uint8_t threshold)
{
  if (channel > 7) {
    return;
  }

  writeRegister(CAP1188_SENSOR_INPUT_1_THRESH + channel, threshold);

}

/*!
 *   @brief  Sets the software threshold for proximity detection.
 *           isProximityDetected() will return true when the sum of absolute
 *           delta counts from the configured channels exceeds this value.
 *   @param  threshold  Sum threshold (0 = proximity detection disabled)
 */
void CAP1188::setProximityThreshold(uint8_t threshold)
{
  _proximityThreshold = threshold;
}

bool CAP1188::isTouched(uint8_t channel)
{
  if (channel > 7) {
    return false;
  }
  
  return ChannelTouched[channel];
}

bool CAP1188::hasChanged(uint8_t channel) {

  if (channel > 7) {
    return false;
  }

  //will be reset once evaluated with "true"
  //that means once called you have to immediately process the information, otherwise its lost!

  if(ChannelChangedSinceLastEvaluate[channel])
  {
    ChannelChangedSinceLastEvaluate[channel] = false; //reset the changed flag, so it will only return true once per change
    return true;
  }

  return false;

}


bool CAP1188::isProximityChanged() {

  //will be reset once evaluated with "true"
  //that means once called you have to immediately process the information, otherwise its lost!

  if(_proximityChangedSinceLastEvaluate)
  {
    _proximityChangedSinceLastEvaluate = false; //reset the changed flag, so it will only return true once per change
    return true;
  }

  return false;

}

bool CAP1188::isTapChanged() {

  if(_tapChangedSinceLastEvaluate)
  {
    _tapChangedSinceLastEvaluate = false;
    return true;
  }

  return false;

}


/**
 * @brief  Enables Multi-Touch Pattern (MTP) detection on the CAP1188.
 *         MTP triggers when ALL specified pads are pressed simultaneously.
 *         The result is readable via the MTP bit (bit 1) in the General Status register (0x02).
 *
 *         CAP1188 uses three separate registers for multi-touch functionality:
 *           0x2A — Multiple Touch Configuration:   controls touch BLOCKING (not MTP!)
 *           0x2B — Multiple Touch Pattern Config:  enables/configures MTP detection
 *           0x2D — Multiple Touch Pattern:         selects which pads participate in the pattern
 *
 * @param  PadsEvaluated       Array of pad indices (0-7) to include in the pattern
 * @param  PadsEvaluatedCount  Number of pads in the array
 */
void CAP1188::enableMultipleTouchTapPattern(uint8_t PadsEvaluated[], uint8_t PadsEvaluatedCount)
{
  // --- Step 1: Configure MTP detection in register 0x2B (Multiple Touch Pattern Config) ---
  // Note: Do NOT write these bits to 0x2A — that register controls touch blocking,
  // and accidentally setting bit 7 there enables MULT_BLK_EN which blocks simultaneous touches!

  writeBit(CAP1188_MULTIPLE_TOUCH_PATTERN_CONFIG, CAP1188_MTP_EN, true);        // Bit 7: Enable MTP detection circuitry
  writeBit(CAP1188_MULTIPLE_TOUCH_PATTERN_CONFIG, CAP1188_MTP_COMP_PTRN, true); // Bit 1: Match specific pad pattern (not just touch count)
  writeBit(CAP1188_MULTIPLE_TOUCH_PATTERN_CONFIG, CAP1188_MTP_TH0, true);       // Bit 2: MTP threshold bits [1:0] = 11b → 100% of touch threshold
  writeBit(CAP1188_MULTIPLE_TOUCH_PATTERN_CONFIG, CAP1188_MTP_TH1, true);       // Bit 3: (all pads must fully exceed their individual thresholds)
  writeBit(CAP1188_MULTIPLE_TOUCH_PATTERN_CONFIG, CAP1188_MTP_ALERT, true);     // Bit 0: Enable MTP alert → sets MTP bit in General Status (0x02)

  // --- Step 2: Select which pads participate in the pattern via register 0x2D ---
  // Default is 0xFF (all 8 pads), so we clear first and then set only the desired pads.
  writeRegister(CAP1188_MULTIPLE_TOUCH_PATTERN_REG, 0x00);
  for (uint8_t i = 0; i < PadsEvaluatedCount; i++)
  {
    writeBit(CAP1188_MULTIPLE_TOUCH_PATTERN_REG, PadsEvaluated[i], true); // Set bit for each pad in the pattern
  }
}