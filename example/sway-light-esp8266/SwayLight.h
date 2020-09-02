#ifndef _SWAYLIGHT_H_
#define _SWAYLIGHT_H_

#define CMD_SIZE 10
#include <SoftwareSerial.h>

class SwayLight {
  public:
    SwayLight(SoftwareSerial& serial);
    void setPower(bool turnOn);
    void setPower(bool turnOn, uint32_t afterSeconds);
    void setColor(uint8_t controlType, uint8_t controlMode, uint32_t rgba);

  private:
    SoftwareSerial *_mcuSerial;
    uint8_t _dataToHT32[CMD_SIZE];
    
    void _initData(void);
    void _setData(uint8_t controlType, uint8_t mode, uint32_t afterSeconds);
    void _setData(uint8_t controlType, uint8_t switchMode);
    void _setLedData(uint8_t controlMode, uint8_t ledControlType, uint8_t param);
    void _setLedData(uint8_t controlMode, uint8_t ledControlType, uint32_t rgba);
    void _setCheckSum(void);
    void _sendDataToHT32(void);
};

#endif
