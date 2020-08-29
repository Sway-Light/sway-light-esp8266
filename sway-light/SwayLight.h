#ifndef _SWAYLIGHT_H_
#define _SWAYLIGHT_H_

#define CMD_SIZE 9
#include <SoftwareSerial.h>

class SwayLight {
  public:
    SwayLight(SoftwareSerial& serial);
    void setPower(bool turnOn);
    void setPower(bool turnOn, int afterSeconds);

  private:
    SoftwareSerial *_mcuSerial;
    byte _dataToHT32[9];
    void _initData(void);
    
    void sendDataToHT32(void);
    void setCheckSum(void);
    enum StartEndByte {
      END_BYTE               = 0x87,
      SWITCH_MODE_START_BYTE = 0x95,
      LIGHT_MODE_START_BYTE  = 0x96,
      MUSIC_MODE_START_BYTE  = 0x97
    };
    enum MODE {
      MUSIC                  = 0x4D,
      LIGHT                  = 0x50,
      ON                     = 0x53,
      OFF                    = 0x57
    };
    enum CONTROL_TYPE {
      COLOR                  = 0x01,
      DISPLAY_OFFSET         = 0x02,
      ZOOM                   = 0x03,
      STYLE                  = 0x04
    };
};


    

#endif
