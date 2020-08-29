#include "SwayLight.h"

SwayLight::SwayLight(SoftwareSerial& serial) {
  _mcuSerial = &serial;
  _mcuSerial->begin(9600);
  _mcuSerial->println("test msg");
}


void SwayLight::setPower(bool turnOn) {
}

void SwayLight::setPower(bool turnOn, int afterSeconds) {  
}


void SwayLight::setCheckSum() {
  uint16_t sum = 0;
  for(int i = 1; i <= 5; i++) {
    sum += _dataToHT32[i];
  }
  _dataToHT32[6] = (byte)(sum >> 8);
  _dataToHT32[7] = (byte)(sum & 0x00FF);
}
