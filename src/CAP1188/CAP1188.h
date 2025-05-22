/*!
 *  @file Adafruit_CAP1188.h
 *
 *  This is a library for the CAP1188 8-Channel Capacitive Sensor
 *
 *  Designed specifically to work with the CAP1188 I2C/SPI 8-chan Capacitive
 *  Sensor from Adafruit
 *
 *  Pick one up today in the adafruit shop!
 *  ------> https://www.adafruit.com/product/1602
 *
 *  These sensors use I2C/SPI to communicate, 2+ pins are required to interface
 *
 *  Adafruit invests time and resources providing this open source code,
 *  please support Adafruit andopen-source hardware by purchasing products
 *  from Adafruit!
 *
 *  Limor Fried/Ladyada (Adafruit Industries).
 *
 *  BSD license, all text above must be included in any redistribution
 */

#include <Arduino.h>
#include "CAP1188_Registers.h"
#include "CAP1188_Defines.h"
#include "OpenKNX.h"
#include <Wire.h>


/*!
 *    @brief  Class that stores state and functions for interacting with
 *            CAP1188 Sensor
 */
class CAP1188 {
public:

  //physically and in datasheet channels start with '1', but "Software-Wise" they start with '0'
  enum ChannelPads {
    Pad_A = 0,
    Pad_B = 1,
    Pad_C = 2,
    Pad_D = 3,
    Pad_E = 4,
    Pad_F = 5,
    Pad_G = 6,
    Pad_H = 7
  };

  // Hardware I2C
  CAP1188(int8_t resetpin = -1);

  boolean begin(uint8_t i2caddr = CAP1188_I2CADDR, TwoWire *theWire = &Wire);

  void LEDpolarity(uint8_t Channel, bool isInverted);

  void LEDOutputPushPull(uint8_t Channel, bool isPushPull);

  void SetGlobalSensitivity(uint8_t sens);

  void disableAnalogNoiseFilter(bool disable);

  //call peridically to read the touch status
  void evaluate();

  void setTouchThreshold(uint8_t channel, uint8_t threshold);
  void setProximityThreshold(uint8_t threshold);

  bool isTouched(uint8_t channel);
  bool hasChanged(uint8_t channel) {return ChannelChangedSinceLastEvaluate[channel];}
  bool ProximitySensed = false;


  uint16_t EvaluateFrequency = 1; //in Hz. Change this if the need for faster evaluation is there

private:

  uint32_t lastEvaluated = 0;

  uint8_t proximityTrehshold = 0;

  int8_t getRawDeltaCount(uint8_t channel);
  int8_t RawDeltaCount[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  bool ChannelTouched[8] = {false, false, false, false, false, false, false, false};
  bool ChannelChangedSinceLastEvaluate[8] = {false, false, false, false, false, false, false, false};

  uint8_t readRegister(uint8_t reg);
  void writeRegister(uint8_t reg, uint8_t value);
  void writeBit(uint8_t reg, uint8_t bit, bool value);
  int8_t _resetpin;
  TwoWire *_wire;

  uint16_t Evaluate_TimeDelay_ms;

  uint8_t touched();

  uint8_t _addr;

  bool write_then_read(const uint8_t *write_buffer, size_t write_len, uint8_t *read_buffer, size_t read_len, bool stop = false);
  bool write(const uint8_t *buffer, size_t len, bool stop, const uint8_t *prefix_buffer, size_t prefix_len);
};
