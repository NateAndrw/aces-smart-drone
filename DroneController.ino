// PROJECT  :DRONE CONTROLLER
// PURPOSE  :Send Joystick Position Values Via Radio
// COURSE   :ICS4U-E
// AUTHOR   :Class
// DATE     :2026 04 02
// MCU      :328p
// STATUS   :Working

//Libraries
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

//Radio
RF24 radio(9, 10);
const byte address[6] = "00001";

//JoyStick
const int joystickPin = A0;

//Deadzone (These Values Were Achived Via Personal Testing)
const int deadzoneMin = 480;
const int deadzoneMax = 540;

//Throttle
int throttle = 1000;

void setup() {
  Serial.begin(9600);

  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_LOW);
  radio.stopListening();

  Serial.println("Controller Ready (Clean)");
}

void loop() {

  int joyValue = analogRead(joystickPin);

  //Deadzone
  if (joyValue >= deadzoneMin && joyValue <= deadzoneMax) {
    throttle = 1000;
  } else if (joyValue > deadzoneMax) {
    throttle = map(joyValue, deadzoneMax, 1023, 1000, 2000);
  } else {
    throttle = 1000;
  }

  //Send Radio
  radio.write(&throttle, sizeof(throttle));

  //Serial Monitor
  Serial.print("Joy: ");
  Serial.print(joyValue);
  Serial.print(" | Throttle: ");
  Serial.println(throttle);

  delay(20);
}
