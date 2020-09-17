#include "SwayLight.h"
#include "SwayLight_MQTT_topic.h"
#define DEBUG_FLAG 1

SwayLight::SwayLight(SoftwareSerial& serial) {
  _mcuSerial = &serial;
  _mcuSerial->begin(9600);
  currIndex = 0;
  //  _mcuSerial->println("test msg");
}

/**********   TRANSMIT   **********/

void SwayLight::setDatetime(uint32_t timestamp) {
  _setData(timestamp);
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

void SwayLight::setPower(bool turnOn, uint8_t enableDay, uint8_t hour, uint8_t min, uint8_t sec) {
  if(turnOn) {
    _setData(_CONTROL_TYPE::SETTING, ON, enableDay, hour, min, sec);
  }else {
    _setData(_CONTROL_TYPE::SETTING, OFF, enableDay, hour, min, sec);
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

/**********   RECIVE   **********/
bool SwayLight::isValid(void) {
  int sum = 0;
  uint16_t checksum = this->dataFromHt32[7] << 8 + this->dataFromHt32[8];
  for (int i = 1; i <= 6; i++) {
    sum += this->dataFromHt32[i];
  }
  if(sum == checksum) {
    Serial.println("Checksum correct");
  }else {
    Serial.println("Checksum ERROR!!");
  }
  return (sum == checksum);
}

uint8_t SwayLight::getControlType() {
  return this->dataFromHt32[1];
}

uint8_t SwayLight::getStatus(void) {
  return this->dataFromHt32[2];
}

uint8_t SwayLight::getLedType(void) {
  return this->dataFromHt32[2];
}

uint8_t SwayLight::getLedParamVal(void) {
  return this->dataFromHt32[3];
}

uint8_t SwayLight::getRed(void) {
  return this->dataFromHt32[4];
}
uint8_t SwayLight::getGreen(void) {
  return this->dataFromHt32[5];
}

uint8_t SwayLight::getBlue(void) {
  return this->dataFromHt32[6];
}

void SwayLight::clearReciveBuff(void) {
  for(int i = 0; i < CMD_SIZE; i++)
    this->dataFromHt32[i] = 0;
}

void SwayLight::printReciveBuff(void) {
  Serial.print("DEBUG: index-> ");
  for(int i = 0; i < CMD_SIZE; i++) {
    Serial.printf("[ %d]", i);
  }
  Serial.println();
  Serial.print("DEBUG: send-> ");
  for(int i = 0; i < CMD_SIZE; i++) {
    Serial.printf("%4X", this->dataFromHt32[i]);
  }
  Serial.print("\n\n");
}

void SwayLight::_initData() {
  for(int i = 0; i < CMD_SIZE; i++) {
    _dataToHT32[i] = 0x00;
  }
  _dataToHT32[0] = _CONST_BYTE::START_BYTE;
  _dataToHT32[CMD_SIZE - 1] = _CONST_BYTE::END_BYTE;
}

// set data
void SwayLight::_setData(uint32_t timestamp) {
  _initData();
  _dataToHT32[1] = _CONTROL_TYPE::SETTING;
  _dataToHT32[2] = _SETTINGS::SYNC_TIME;
  _dataToHT32[3] = (timestamp >> 24) & 0xFF;
  _dataToHT32[4] = (timestamp >> 16) & 0xFF;
  _dataToHT32[5] = (timestamp >>  8) & 0xFF;
  _dataToHT32[6] = timestamp & 0xFF;
  _setCheckSum();
}

void SwayLight::_setData(uint8_t controlType, uint8_t mode, uint8_t enableDay, uint8_t hour, uint8_t min, uint8_t sec) {
  _initData();
  _dataToHT32[1] = controlType;
  _dataToHT32[2] = mode;
  _dataToHT32[3] = enableDay;
  _dataToHT32[4] = hour;
  _dataToHT32[5] = min;
  _dataToHT32[6] = sec;
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
