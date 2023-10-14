#include <Adafruit_NeoPixel.h>
#include <NewPing.h>
#define LED_PIN 10
#define LED_COUNT 12                                    
#define BRIGHTNESS 50
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
#define TRIGGER_PIN       24  
#define ECHO_PIN          25  
#define TRIGGER_PIN       23  
#define PWM_OUTPUT_PIN    22  
#define MAX_DISTANCE      200
NewPing sonar[2] = {
  NewPing(24, 25, 200),
  NewPing(23, 22, 500),
};
#define LED_1_PIN 11
#define LED_2_PIN 18
int IN1_L = 2;
int IN2_L = 3;
int IN1_R = 4;
int IN2_R = 5;
int L_PWM = 8;
int R_PWM = 9;
const int intervalOn = 500;
const int intervalOff = 500;
bool propellerActive = false;
bool ledOn = false;
int pINA1 = 6;
int pINA2 = 7;
int PWMPIN = 12;
unsigned long previousMillis = 0;
const long interval = 1000;
unsigned long startTime = 0;
char c, i, drivingMode;
int drivePowerLeft, drivePowerRight, stopPowerLeft, stopPowerRight, suctionPower;
int stopDelay, reverseDelay, driveDelay;
void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  pinMode(IN1_L, OUTPUT);
  pinMode(IN2_L, OUTPUT);
  pinMode(IN1_R, OUTPUT);
  pinMode(IN2_R, OUTPUT);
  pinMode(pINA1, OUTPUT);
  pinMode(pINA2, OUTPUT);
  pinMode(L_PWM, OUTPUT);
  pinMode(R_PWM, OUTPUT);
  drivePowerLeft = 150;
  drivePowerRight = 150;
  suctionPower = 150;
  stopPowerLeft = 73; 
  stopPowerRight = 71; 
  stopDelay = 500;
  reverseDelay = 800;
  driveDelay = 100;
  pinMode(LED_1_PIN, OUTPUT);
  pinMode(LED_2_PIN, OUTPUT);
  digitalWrite(LED_1_PIN, LOW);
  digitalWrite(LED_2_PIN, LOW);
  strip.begin();                   
  strip.setBrightness(BRIGHTNESS); 
  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  }
  strip.show();
}
void loop() {
  detectCollision();
  delay(300);
  //ledControl();
  if (Serial1.available() > 0) {
    c = Serial1.read();
    Serial.println("////");
    switch (c) {
      case 'f':
        moveWithCollisionDetection(forward, backward);
        break;
      case 'b':
        moveIt(backward);
        break;
      case 'r':
        moveWithCollisionDetection(turnRight, backward);
        break;
      case 'l':
        moveWithCollisionDetection(turnLeft, backward);
        break;
      case 'a':
        moveWithCollisionDetection(leftUp, backward);
        break;
      case 'x':
        moveIt(leftDown);
        break;
      case 'c':
        moveWithCollisionDetection(rightUp, backward);
        break;
      case 'd':
        moveIt(rightDown);
        break;
      case 'p':
        Serial.println("p");
        while (true) {
          propeller();
          for (int i = 0; i < LED_COUNT; i++) {
            strip.setPixelColor(i, strip.Color(230, 30, 0));
            strip.show();
            unsigned long startTime = millis();
            while (millis() - startTime < 50) {}
          }
          for (int i = 0; i < LED_COUNT; i++) {
            strip.setPixelColor(i, strip.Color(0, 0, 0));
          }
          strip.show();
          if (Serial1.available() > 0 && Serial1.read() == 'z') {
            propellerActive = false;  // Deactivate propeller mode
            stop_propeller();
            stop();
            for (int i = 0; i < LED_COUNT; i++) {
              strip.setPixelColor(i, strip.Color(0, 0, 0));
            }
            strip.show();
            break;
          }
        }
        break;
        case 'e':
          digitalWrite(LED_1_PIN, HIGH);
          digitalWrite(LED_2_PIN, HIGH);
          while (true) {
            if (Serial1.available() > 0 && Serial1.read() == 'g') {
              digitalWrite(LED_1_PIN, LOW);
              digitalWrite(LED_2_PIN, LOW);
              break;
            }
          }
          break;
    }
  }
}
void moveWithCollisionDetection(void (*movementFunction)(), void (*backupFunction)()) {
  while (true) {
    (*movementFunction)();
    for (int i = 0; i < LED_COUNT; i++) {
      strip.setPixelColor(i, strip.Color(0, 255, 0));
    }
    strip.show();
    if (detectCollision()) {
      Serial1.print("i");
      stop();
      unsigned long startTime = millis();
      while (millis() - startTime < stopDelay) {}
      (*backupFunction)();
      unsigned long startTime2 = millis();
      while (millis() - startTime2 < reverseDelay) {
        for (int i = 0; i < LED_COUNT; i++) {
          strip.setPixelColor(i, strip.Color(255, 0, 0));
        }
        strip.show();
      }
      stop();
      for (int i = 0; i < LED_COUNT; i++) {
        strip.setPixelColor(i, strip.Color(0, 0, 0));
      }
      strip.show();
      break;
    }
    if (Serial1.available() > 0 && Serial1.read() == 's') {
      stop();
      for (int i = 0; i < LED_COUNT; i++) {
        strip.setPixelColor(i, strip.Color(0, 0, 0));
      }
      strip.show();
      break;
    }
  }
}
void moveIt(void (*movementFunction)()) {
  while (true) {
    (*movementFunction)();
    for (int i = 0; i < LED_COUNT; i++) {
      strip.setPixelColor(i, strip.Color(0, 255, 0));
    }
    strip.show();
    if (Serial1.available() > 0 && Serial1.read() == 's') {
      stop();
      for (int i = 0; i < LED_COUNT; i++) {
        strip.setPixelColor(i, strip.Color(0, 0, 0));
      }
      strip.show();
      break;
    }
  }
}
void forward() {
  Serial.println(" forward!");
  stop();
  delay(10);
  digitalWrite(IN1_L, LOW);
  digitalWrite(IN2_L, HIGH);
  analogWrite(L_PWM, drivePowerLeft);
  digitalWrite(IN1_R, LOW);
  digitalWrite(IN2_R, HIGH);
  analogWrite(R_PWM, drivePowerRight);
  unsigned long startTime = millis();
  while (millis() - startTime < driveDelay) {}
}
void backward() {
  Serial.println(" backward!");
  stop();
  delay(10);
  digitalWrite(IN1_L, HIGH);
  digitalWrite(IN2_L, LOW);
  analogWrite(L_PWM, drivePowerLeft);
  digitalWrite(IN1_R, HIGH);
  digitalWrite(IN2_R, LOW);
  analogWrite(R_PWM, drivePowerRight);
  unsigned long startTime = millis();
  while (millis() - startTime < driveDelay) {}
}
void turnRight() {
  Serial.println(" turnRight!");
  stop();
  delayMicroseconds(10);
  digitalWrite(IN1_L, LOW);
  digitalWrite(IN2_L, HIGH);
  analogWrite(L_PWM, drivePowerLeft);
  digitalWrite(IN1_R, LOW);
  digitalWrite(IN2_R, LOW);
  unsigned long startTime = millis();
  while (millis() - startTime < driveDelay) {}
}
void turnLeft() {
  Serial.println(" turnLeft!");
  stop();
  delay(10);
  digitalWrite(IN1_L, LOW);
  digitalWrite(IN2_L, LOW);
  digitalWrite(IN1_R, LOW);
  digitalWrite(IN2_R, HIGH);
  analogWrite(R_PWM, drivePowerRight);
  unsigned long startTime = millis();
  while (millis() - startTime < driveDelay) {}
}
void leftUp() {
  Serial.println(" leftUp!");
  stop();
  delay(10);
  digitalWrite(IN1_L, LOW);
  digitalWrite(IN2_L, HIGH);
  analogWrite(L_PWM, drivePowerLeft * 0.6);
  digitalWrite(IN1_R, LOW);
  digitalWrite(IN2_R, HIGH);
  analogWrite(R_PWM, drivePowerRight * 0.8);
  unsigned long startTime = millis();
  while (millis() - startTime < driveDelay) {}
}
void leftDown() {
  Serial.println(" leftDown!");
  stop();
  delay(10);
  digitalWrite(IN1_L, HIGH);
  digitalWrite(IN2_L, LOW);
  analogWrite(L_PWM, drivePowerLeft * 0.5);
  digitalWrite(IN1_R, HIGH);
  digitalWrite(IN2_R, LOW);
  analogWrite(R_PWM, drivePowerRight);
  unsigned long startTime = millis();
  while (millis() - startTime < driveDelay) {}
}
void rightUp() {
  Serial.println(" rightUp!");
  stop();
  delayMicroseconds(10);
  digitalWrite(IN1_L, LOW);
  digitalWrite(IN2_L, HIGH);
  analogWrite(L_PWM, drivePowerLeft);
  digitalWrite(IN1_R, LOW);
  digitalWrite(IN2_R, HIGH);
  analogWrite(R_PWM, drivePowerRight * 0.7);
  unsigned long startTime = millis();
  while (millis() - startTime < driveDelay) {}
}
void rightDown() {
  Serial.println(" rightDown!");
  stop();
  delayMicroseconds(10);
  digitalWrite(IN1_L, HIGH);
  digitalWrite(IN2_L, LOW);
  analogWrite(L_PWM, drivePowerLeft);
  digitalWrite(IN1_R, HIGH);
  digitalWrite(IN2_R, LOW);
  analogWrite(R_PWM, drivePowerRight * 0.5);
  unsigned long startTime = millis();
  while (millis() - startTime < driveDelay) {}
}
void stop() {
  Serial.println("MOTOR STOPPED!");
  digitalWrite(IN1_L, LOW);
  digitalWrite(IN2_L, LOW);
  digitalWrite(IN1_R, LOW);
  digitalWrite(IN2_R, LOW);
}
void propeller() {
  stop();
  stop_propeller();
  delay(10);
  digitalWrite(pINA1, LOW);
  digitalWrite(pINA2, HIGH);
  analogWrite(PWMPIN, 150);
  digitalWrite(IN1_L, HIGH);
  digitalWrite(IN2_L, LOW);
  analogWrite(L_PWM, stopPowerLeft);
  digitalWrite(IN1_R, HIGH);
  digitalWrite(IN2_R, LOW);
  analogWrite(R_PWM, stopPowerRight);
  unsigned long startTime = millis();
  while (millis() - startTime < 500) {}
}
void stop_propeller() {
  digitalWrite(pINA1, LOW);
  digitalWrite(pINA2, LOW);
  digitalWrite(IN1_L, LOW);
  digitalWrite(IN2_L, LOW);
  digitalWrite(IN1_R, LOW);
  digitalWrite(IN2_R, LOW);
}
bool detectCollision() {
  static unsigned long startTime = 0;
  unsigned long currentMillis = millis();
  int ping1 = sonar[0].ping_cm();
  int ping2 = sonar[1].ping_cm();
  Serial.print("PING 1: ");
  Serial.println(ping1);
  Serial.print(" PING 2: ");
  Serial.println(ping2);
  if ( ping2 <= 50 && ping2 != 0) {
    return true;
  }
  
  else if ( ping1 <= 50 && ping1 != 0) {
    return true;
  }
  return false;
}
void ledControl() {
  static unsigned long previousTime = 0;
  unsigned long currentTime = millis();

  int cds1 = analogRead(A0);
  int cds2 = analogRead(A1);
  Serial.print("CDS1: ");
  Serial.print(cds1);
  Serial.print(" CDS2: ");
  Serial.println(cds2);

  if (cds1 > 400 && cds2 > 400  ) {
    digitalWrite(LED_1_PIN, HIGH);
    digitalWrite(LED_2_PIN, HIGH);
    if (currentTime - previousTime >= 1000) {
      previousTime = currentTime;
    }
  }
  else {
    digitalWrite(LED_1_PIN, LOW);
    digitalWrite(LED_2_PIN, LOW);
    previousTime = currentTime;
  }
}
void handlePropellerMode() {
  static bool isOn = false;
  static unsigned long previousMillis = 0;
  const unsigned long interval = 300;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    if (isOn) {
      for (int i = 0; i < LED_COUNT; i++) {
        strip.setPixelColor(i, strip.Color(0, 0, 0));
        Serial.println("on");
      }
      strip.show();
      isOn = false;
    } else {
      for (int i = 0; i < LED_COUNT; i++) {
        strip.setPixelColor(i, strip.Color(250, 0, 30));
        Serial.println("off");
      }
      strip.show();
      isOn = true;
    }
  }
}
