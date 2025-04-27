/*!
 *  @file CAP1188.cpp
 *
 *  @mainpage Adafruit CAP1188 I2C/SPI 8-chan Capacitive Sensor
 *
 *  @section intro_sec Introduction
 *
 * 	This is a library for the Adafruit CAP1188 I2C/SPI 8-chan Capacitive
 * Sensor http://www.adafruit.com/products/1602
 *
 *  These sensors use I2C/SPI to communicate, 2+ pins are required to
 *  interface
 *
 * 	Adafruit invests time and resources providing this open source code,
 *  please support Adafruit and open-source hardware by purchasing products from
 * 	Adafruit!
 *
 *  @section author Author
 *
 *  Limor Fried/Ladyada (Adafruit Industries).
 *  cad435 (heavily modified for OpenKNX)
 *
 * 	@section license License
 *
 * 	BSD (see license.txt)
 *
 * 	@section  HISTORY
 *
 *     v1.0 - First release
 *     v1.1 - heavily modified for OpenKNX
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
  

  _addr = i2caddr;

  _wire = theWire;

  _wire->setSCL(CAP1188_I2C_SCL);
  _wire->setSDA(CAP1188_I2C_SDA);

  _wire->begin();

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
  //guard clause if we need to evaluate or if we can just return
  if (millis() - lastEvaluated < 100)
    return;
  
  lastEvaluated = millis();

  for (uint8_t i = 0; i < 8; i++)
  {
    RawDeltaCount[i] = getRawDeltaCount(i);
  }
  uint8_t reg = touched();
  //get the bits for the channel out of the register and write it to the array
  for (uint8_t i = 0; i < 8; i++)
  {
    uint8_t mask = (1 << i);
    uint8_t value = (reg & mask) >> i;
    if (value > 1)
      ChannelTouched[i] = true;
    else
      ChannelTouched[i] = false;
  }

  //sum up all Raw Delta Counts
  int8_t sum = 0;
  for (uint8_t i = 0; i < 8; i++)
  {
    sum += abs(RawDeltaCount[i]);
  }

  if (sum > proximityTrehshold)
    ProximitySensed = true;
  else
    ProximitySensed = false;
}



/*!
 *   @brief  Reads the touched status (CAP1188_SENINPUTSTATUS)
 *   @return Returns read from CAP1188_SENINPUTSTATUS where 1 is touched, 0 not
 * touched.
 */
uint8_t CAP1188::touched() {
  uint8_t t = readRegister(CAP1188_SENSOR_INPUT_STATUS);
  if (t) {
    writeRegister(CAP1188_MAIN_CONTROL, readRegister(CAP1188_MAIN_CONTROL) & ~CAP1188_MAIN_INTERRUPT); // clear interrupt
  }
  return t;
}

/*!
 *   @brief  Controls the output polarity of LEDs.
 *   @param  inverted
 *           0 (default) - The LED8 output is inverted.
 *           1 - The LED8 output is non-inverted.
 */
void CAP1188::LEDpolarity(uint8_t inverted) {
  writeRegister(CAP1188_LED_POLARITY, inverted);
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

  Serial.println("Current Value: " + String(current_value, BIN));

  if (value)
    current_value |= (1 << bit);
  else
    current_value &= ~(1 << bit);

  writeRegister(reg, current_value);

  current_value = readRegister(reg);

  Serial.println("New Value: " + String(current_value, BIN));  
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


void CAP1188::setProximityThreshold(uint8_t threshold)
{
  proximityTrehshold = threshold;
}

bool CAP1188::isTouched(uint8_t channel)
{
  if (channel > 7) {
    return false;
  }
  return ChannelTouched[channel];
}