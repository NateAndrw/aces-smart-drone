// PROJECT  :DRONE RECIEVER
// PURPOSE  :Receive Joystick Position Values and Convert to ESC PWM Signals
// COURSE   :ICS4U-E
// AUTHOR   :Class
// DATE     :2026 04 02
// MCU      :328p
// STATUS   :Working

//Libraries
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

//Radio Config
RF24 radio(9, 10);
const byte address[6] = "00001";

//ESC Config
Servo esc1;  // Front Left  (D5)
Servo esc2;  // Front Right (D6)
Servo esc3;  // Back Left   (D7)
Servo esc4;  // Back Right  (D8)

//Throttle Config
int throttle = 1000;
int receivedThrottle = 1000;

//Kill Switch
unsigned long lastSignalTime = 0;
const int signalTimeout = 300;

//Manual Trim
int trim = 175;

//Startup Test
bool startupTestDone = false;
unsigned long startupTime = 0;

void setup() {
  Serial.begin(9600);

  esc1.attach(5);
  esc2.attach(6);
  esc3.attach(7);
  esc4.attach(8);

  armESCs();

  // Start test timer AFTER arming
  startupTime = millis();

  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening();

  lastSignalTime = millis();

  Serial.println("Drone Ready (STARTUP TEST ENABLED)");
}

void loop() {

  // ===== STARTUP SPIN TEST (5 seconds at ~1100) =====
  if (!startupTestDone) {
    if (millis() - startupTime < 5000) {
      int testPower = 1100;

      esc1.writeMicroseconds(testPower);
      esc2.writeMicroseconds(testPower);
      esc3.writeMicroseconds(testPower);
      esc4.writeMicroseconds(testPower);

      Serial.println("Startup Test Running...");
      delay(20);
      return; // skip rest of loop
    } else {
      startupTestDone = true;
      Serial.println("Startup Test Complete");
    }
  }

  // ===== RADIO RECEIVER =====
  if (radio.available()) {
    while (radio.available()) {
      radio.read(&receivedThrottle, sizeof(receivedThrottle));
    }

    if (receivedThrottle >= 1000 && receivedThrottle <= 2000) {
      throttle = receivedThrottle;
      lastSignalTime = millis();
    }
  }

  // ===== FAILSAFE =====
  if (millis() - lastSignalTime > signalTimeout) {
    throttle = 1000;
  }

  // ===== MOTOR OUTPUT =====
  int leftPower;
  int rightPower;

  if (throttle <= 1000) {
    leftPower = 1000;
    rightPower = 1000;
  } else {
    leftPower  = constrain(throttle + trim, 1000, 2000);
    rightPower = constrain(throttle, 1000, 2000);
  }

  // ===== ESC OUTPUT =====
  esc1.writeMicroseconds(leftPower);
  esc3.writeMicroseconds(leftPower);

  esc2.writeMicroseconds(rightPower);
  esc4.writeMicroseconds(rightPower);

  // ===== SERIAL =====
  Serial.print("Throttle: ");
  Serial.print(throttle);
  Serial.print(" | L: ");
  Serial.print(leftPower);
  Serial.print(" | R: ");
  Serial.println(rightPower);

  delay(20);
}

//Arm ESCs
void armESCs() {
  Serial.println("Arming ESCs...");

  esc1.writeMicroseconds(1000);
  esc2.writeMicroseconds(1000);
  esc3.writeMicroseconds(1000);
  esc4.writeMicroseconds(1000);

  delay(5000);

  Serial.println("ESCs Armed");
}
