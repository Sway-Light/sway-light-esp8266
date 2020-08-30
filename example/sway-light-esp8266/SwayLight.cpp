#include "SwayLight.h"
#define DEBUG_FLAG 1

SwayLight::SwayLight(SoftwareSerial& serial) {
  _mcuSerial = &serial;
  _mcuSerial->begin(9600);
//  _mcuSerial->println("test msg");
}

void SwayLight::setPower(bool turnOn) {
  _initData();
  if(turnOn) {
    _setData(MODE_SWITCH, ON);
  }else {
    _setData(MODE_SWITCH, OFF);
  }
  _sendDataToHT32();
}

void SwayLight::setPower(bool turnOn, int afterSeconds) {
  
}

void SwayLight::_initData() {
  for(int i = 0; i < CMD_SIZE; i++) {
    _dataToHT32[i] = 0x00;
  }
  _dataToHT32[0] = _CONST_BYTE::START_BYTE;
  _dataToHT32[CMD_SIZE - 1] = _CONST_BYTE::END_BYTE;
}

// set data
void SwayLight::_setData(byte controlType, byte mode, int afterSeconds) {
  _dataToHT32[1] = controlType;
  _dataToHT32[2] = mode;
  _dataToHT32[3] = (afterSeconds >> 24) & 0xFF;
  _dataToHT32[4] = (afterSeconds >> 16) & 0xFF;
  _dataToHT32[5] = (afterSeconds >>  8) & 0xFF;
  _dataToHT32[6] = (afterSeconds      ) & 0xFF;
  _setCheckSum();
}

void SwayLight::_setData(byte controlType, byte switchMode) {
  _dataToHT32[1] = controlType;
  _dataToHT32[2] = switchMode;
  _setCheckSum();
}

void SwayLight::_setData(byte controlType, byte ledControl, byte param) {
  _dataToHT32[1] = controlType;
}

void SwayLight::_setData(byte controlType, byte ledControl, byte param, byte red, byte green, byte blue) {
  _dataToHT32[1] = controlType;
}

void SwayLight::_setCheckSum() {
  uint16_t sum = 0;
  for(int i = 1; i <= 6; i++) {
    sum += _dataToHT32[i];
  }
  _dataToHT32[7] = (byte)(sum >> 8);
  _dataToHT32[8] = (byte)(sum & 0x00FF);
}

void SwayLight::_sendDataToHT32(void) {
  for(int i = 0; i < CMD_SIZE; i++) {
    _mcuSerial->write(_dataToHT32[i]);
  }
  #if DEBUG_FLAG
  Serial.print("DEBUG: index-> ");
  for(int i = 0; i < CMD_SIZE; i++) {
    Serial.printf("[ %d]", i);
  }
  Serial.println();
  Serial.print("DEBUG: send-> ");
  for(int i = 0; i < CMD_SIZE; i++) {
    Serial.printf("%4X", _dataToHT32[i]);
  }
  Serial.print("\n\n");
  #endif
}
