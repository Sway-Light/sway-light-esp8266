#ifndef _SWAYLIGHT_H_
#define _SWAYLIGHT_H_

#define CMD_SIZE 10
#include <SoftwareSerial.h>

class SwayLight {
  public:
    SwayLight(SoftwareSerial& serial);
    void setDatetime(uint32_t timestamp);
    void setPower(bool turnOn);
    void setPower(bool turnOn, uint8_t enableDay, uint8_t hour, uint8_t min, uint8_t sec);
    void setMode(uint8_t mode);
    void setLedColor(uint8_t controlType, uint8_t controlMode, uint32_t rgba);
    void setLedOffset(uint8_t mode, uint8_t offsetValue);
    void setLedZoom(uint8_t zoomValue);
    void setLedStyle(uint8_t styleId);

  private:
    SoftwareSerial *_mcuSerial;
    uint8_t _dataToHT32[CMD_SIZE];
    
    void _initData(void);
    void _setData(uint32_t timestamp);
    void _setData(uint8_t controlType, uint8_t mode, uint8_t enableDay, uint8_t hour, uint8_t min, uint8_t sec);
    void _setData(uint8_t controlType, uint8_t switchMode);
    void _setLedData(uint8_t controlMode, uint8_t ledControlType, uint8_t param);
    void _setLedData(uint8_t controlMode, uint8_t ledControlType, uint32_t rgba);
    void _setCheckSum(void);
    void _sendDataToHT32(void);
};

#endif
