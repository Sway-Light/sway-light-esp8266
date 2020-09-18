#ifndef _SWAYLIGHT_H_
#define _SWAYLIGHT_H_

#define CMD_SIZE 10
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <string.h>
#define CLIENT_ID "esp8266"

class SwayLight {
  public:
    SwayLight(SoftwareSerial& serial);
    uint8_t dataFromHt32[CMD_SIZE];
    uint8_t currIndex;
    void setDatetime(uint32_t timestamp);
    void setPower(bool turnOn);
    void setPower(bool turnOn, uint8_t enableDay, uint8_t hour, uint8_t min, uint8_t sec);
    void setMode(uint8_t mode);
    void setLedColor(uint8_t controlType, uint8_t controlMode, uint32_t rgba);
    void setLedOffset(uint8_t mode, uint8_t offsetValue);
    void setLedZoom(uint8_t zoomValue);
    void setLedStyle(uint8_t styleId);

    // recive
    bool isValid(void);
    uint8_t getControlType(void);
    uint8_t getStatus(void);
    uint8_t getLedType(void);
    uint8_t getLedParamVal(void);
    uint8_t getRed(void);
    uint8_t getGreen(void);
    uint8_t getBlue(void);
    void clearReciveBuff(void);
    void printReciveBuff(void);
    bool isFromMyself(void);

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
