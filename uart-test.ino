#include <SoftwareSerial.h>

SoftwareSerial mySerial(13, 15);

byte data[9] = {0x96, 0x01, 0x02, 0x03, 0x04, 0x05, 0x00, 0x0F, 0x87};
int i = 0;
void setup() {
 mySerial.begin(9600);
 Serial.begin(9600);
 delay(2000);
}

void loop() {
  for(int t = 0; t < 60; t++) {
    for(i = 0; i < 9; i++) {
      mySerial.write(data[i]);
    }
  }
  delay(3000);
}
