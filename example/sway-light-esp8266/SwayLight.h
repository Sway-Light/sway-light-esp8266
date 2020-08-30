#ifndef _SWAYLIGHT_H_
#define _SWAYLIGHT_H_

#define CMD_SIZE 10
#include <SoftwareSerial.h>

class SwayLight {
  public:
    SwayLight(SoftwareSerial& serial);
    void setPower(bool turnOn);
    void setPower(bool turnOn, int afterSeconds);

  private:
    SoftwareSerial *_mcuSerial;
    byte _dataToHT32[CMD_SIZE];
    void _initData(void);
    
    void _setData(byte controlType, byte mode, int afterSeconds);
    void _setData(byte controlType, byte switchMode);
    void _setData(byte controlType, byte ledControl, byte param);
    void _setData(byte controlType, byte ledControl, 
                         byte param, byte red, byte green, byte blue);
    void _setCheckSum(void);
    void _sendDataToHT32(void);
    
    enum _CONST_BYTE {
      END_BYTE               = 0x87,
      START_BYTE             = 0x95
    };
    enum _POWER {
      OFF                    = 0x00,
      ON                     = 0x01
    };
    enum _CONTROL_TYPE {
      MODE_SWITCH            = 0x01,
      LIGHT                  = 0x02,
      MUSIC                  = 0x03
    };
    enum _LED {
      COLOR                  = 0x01,
      DISPLAY_OFFSET         = 0x02,
      ZOOM                   = 0x03,
      STYLE                  = 0x04
    };
};

#endif
