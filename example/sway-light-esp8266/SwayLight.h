#ifndef _SWAYLIGHT_H_
#define _SWAYLIGHT_H_

#define CMD_SIZE 10
#include <SoftwareSerial.h>

class SwayLight {
  public:
    SwayLight(SoftwareSerial& serial);
    void setPower(bool turnOn);
    void setPower(bool turnOn, int afterSeconds);
    void setColor(byte controlType, byte controlMode, int rgba);

  private:
    SoftwareSerial *_mcuSerial;
    byte _dataToHT32[CMD_SIZE];
    void _initData(void);
    
    void _setData(byte controlType, byte mode, int afterSeconds);
    void _setData(byte controlType, byte switchMode);
    void _setLedData(byte controlMode, byte ledControlType, byte param);
    void _setLedData(byte controlMode, byte ledControlType, int rgba);
    void _setCheckSum(void);
    void _sendDataToHT32(void);
};

#endif
