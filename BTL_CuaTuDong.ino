#include <DistanceSRF04.h>
#include <Servo.h>

DistanceSRF04 Dist;
Servo myservo;

#define SERVO_PIN       11
#define SRF04_ECHO_PIN  3
#define SRF04_TRIG_PIN  2

#define SERVO_CLOSED_ANGLE 180
#define SERVO_OPEN_ANGLE   90

#define IDLE_STATE  0
#define OPEN_STATE  1
#define CLOSE_STATE 2

int distance;
unsigned long previousMillis = 0;
unsigned long autoMillis = 0;
int modeRun = IDLE_STATE;
unsigned long timeMillis = 0;
unsigned int timeOpen = 2;

void setup() {
  Serial.begin(115200);
  Dist.begin(SRF04_ECHO_PIN, SRF04_TRIG_PIN);
  myservo.attach(SERVO_PIN);
  myservo.write(SERVO_CLOSED_ANGLE);
  pinMode(13, OUTPUT);
  pinMode(A7, INPUT_PULLUP);
}

void loop() {
  Serial.println(modeRun);
  switch (modeRun) {
    case IDLE_STATE:
      if (readSRF04() == 1) {
        modeRun = OPEN_STATE;
        myservo.write(SERVO_OPEN_ANGLE);
        delay(50);
        digitalWrite(13, HIGH);
        timeMillis = millis();
      }
      break;
    case OPEN_STATE:
      if (millis() - timeMillis > timeOpen * 1000) {
        timeMillis = millis();
        modeRun = CLOSE_STATE;
      }
      if (readSRF04() == 1) {
        timeMillis = millis();
      }
      break;
    case CLOSE_STATE:
      myservo.write(SERVO_CLOSED_ANGLE);
      digitalWrite(13, LOW);
      delay(500);
      modeRun = IDLE_STATE;
      break;
  }
}

int readSRF04() {
  if (millis() - previousMillis >= 10) {
    previousMillis = millis();
    distance = Dist.getDistanceCentimeter();
    if (distance > 1 && distance < 10) {
      autoMillis = millis();
      return 1;
    }
  }
  return 0;
}
