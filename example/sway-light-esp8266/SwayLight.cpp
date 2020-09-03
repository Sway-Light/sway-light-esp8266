#include "SwayLight.h"
#include "SwayLight_MQTT_topic.h"
#define DEBUG_FLAG 1

SwayLight::SwayLight(SoftwareSerial& serial) {
  _mcuSerial = &serial;
  _mcuSerial->begin(9600);
//  _mcuSerial->println("test msg");
}

void SwayLight::setDatetime(uint16_t Y, uint8_t M, uint8_t D, uint8_t h, uint8_t m, uint8_t s) {
  _setData(Y, M, D, h, m, s);
  Serial.println("setDatetime:");
  _sendDataToHT32();
}

void SwayLight::setPower(bool turnOn) {
  if(turnOn) {
    _setData(MODE_SWITCH, ON);
  }else {
    _setData(MODE_SWITCH, OFF);
  }
  _sendDataToHT32();
}

void SwayLight::setPower(bool turnOn, uint32_t afterSeconds) {
  if(turnOn) {
    _setData(MODE_SWITCH, ON, afterSeconds);
  }else {
    _setData(MODE_SWITCH, OFF, afterSeconds);
  }
  _sendDataToHT32();
}

void SwayLight::setMode(uint8_t mode) {
  _setData(_CONTROL_TYPE::MODE_SWITCH, mode);
  _sendDataToHT32();
}

void SwayLight::setLedColor(uint8_t controlMode, uint8_t controlType, uint32_t rgba) {
  _setLedData(controlMode, controlType, rgba);
  _sendDataToHT32();
}

void SwayLight::setLedOffset(uint8_t mode, uint8_t offsetValue) {
  _setLedData(mode, _LED::DISPLAY_OFFSET, offsetValue);
  _sendDataToHT32();
}

void SwayLight::setLedZoom(uint8_t zoomValue) {
  _setLedData(_CONTROL_TYPE::LIGHT, _LED::ZOOM, zoomValue);
  _sendDataToHT32();
}

void SwayLight::setLedStyle(uint8_t styleId) {
  _setLedData(_CONTROL_TYPE::MUSIC, _LED::STYLE, styleId);
  _sendDataToHT32();
}

void SwayLight::_initData() {
  for(int i = 0; i < CMD_SIZE; i++) {
    _dataToHT32[i] = 0x00;
  }
  _dataToHT32[0] = _CONST_BYTE::START_BYTE;
  _dataToHT32[CMD_SIZE - 1] = _CONST_BYTE::END_BYTE;
}

// set data
void SwayLight::_setData(uint16_t Y, uint8_t M, uint8_t D, uint8_t h, uint8_t m, uint8_t s) {
  /** 
   *  Byte -> |          [3]          |          [4]          |          [5]          |          [6]          |
   *  bit  -> | 0| 1| 2| 3| 4| 5| 6| 7| 8| 9|10|11|12|13|14|15|16|17|18|19|20|21|22|23|24|25|26|27|28|29|30|31|
   *  info -> |   YEAR    |   MONTH   |      DAY     |       HOUR   |     MINUTE      | x| x|      SECOND     |
   **/
  _initData();
  _dataToHT32[1] = _CONTROL_TYPE::SETTING;
  _dataToHT32[2] = _SETTINGS::SYNC_TIME;
  _dataToHT32[3] = ((uint8_t)(Y - 2020) << 4) + M;
  _dataToHT32[4] = (D << 3) + (h >> 2);
  _dataToHT32[5] = (h << 6) + m;
  _dataToHT32[6] = s;
  _setCheckSum();
}

void SwayLight::_setData(uint8_t controlType, uint8_t mode, uint32_t afterSeconds) {
  _initData();
  _dataToHT32[1] = controlType;
  _dataToHT32[2] = mode;
  _dataToHT32[3] = (afterSeconds >> 24) & 0xFF;
  _dataToHT32[4] = (afterSeconds >> 16) & 0xFF;
  _dataToHT32[5] = (afterSeconds >>  8) & 0xFF;
  _dataToHT32[6] = (afterSeconds      ) & 0xFF;
  _setCheckSum();
}

void SwayLight::_setData(uint8_t controlType, uint8_t switchMode) {
  _initData();
  _dataToHT32[1] = controlType;
  _dataToHT32[2] = switchMode;
  _setCheckSum();
}

void SwayLight::_setLedData(uint8_t controlMode, uint8_t ledControlType, uint8_t param) {
  _initData();
  _dataToHT32[1] = controlMode;
  _dataToHT32[2] = ledControlType;
  _dataToHT32[3] = param;
  _setCheckSum();
}

void SwayLight::_setLedData(uint8_t controlMode, uint8_t ledControlType, uint32_t rgba) {
  //  brightness,level   R    G    B
  //              [ 3] [ 4] [ 5] [ 6]
  _initData();
  _dataToHT32[1] = controlMode;
  _dataToHT32[2] = _LED::COLOR;
  _dataToHT32[3] = (rgba      ) & 0xFF;
  _dataToHT32[4] = (rgba >> 24) & 0xFF;
  _dataToHT32[5] = (rgba >> 16) & 0xFF;
  _dataToHT32[6] = (rgba >>  8) & 0xFF;
  _setCheckSum();
}

void SwayLight::_setCheckSum() {
  uint16_t sum = 0;
  for(int i = 1; i <= 6; i++) {
    sum += _dataToHT32[i];
  }
  _dataToHT32[7] = (uint8_t)(sum >> 8);
  _dataToHT32[8] = (uint8_t)(sum & 0x00FF);
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
