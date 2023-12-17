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

int distance; //Lưu trữ giá trị khoảng cách đo được từ cảm biến siêu âm.
unsigned long previousMillis = 0; //Lưu trữ giá trị khoảng cách đo được từ cảm biến siêu âm.
unsigned long autoMillis = 0; //Theo dõi thời gian (đơn vị là milliseconds) khi có sự kiện tự động xảy ra.
int modeRun = IDLE_STATE; //Xác định trạng thái hoạt động của chương trình (IDLE_STATE, OPEN_STATE, CLOSE_STATE).
unsigned long timeMillis = 0;//Theo dõi thời gian (đơn vị là milliseconds) cho một sự kiện cụ thể.
unsigned int timeOpen = 2; //Đặt thời gian (đơn vị là giây) cần để mở cửa servo.

void setup() {
  Serial.begin(115200);//Khởi tạo giao tiếp serial với tốc độ baud là 115200. 
  Dist.begin(SRF04_ECHO_PIN, SRF04_TRIG_PIN);//Khởi tạo đối tượng DistanceSRF04 (Dist) 
  myservo.attach(SERVO_PIN);//Kết nối động cơ Servo (myservo) với chân đã được chỉ định (SERVO_PIN). 
  myservo.write(SERVO_CLOSED_ANGLE);//Góc ban đầu của servo được đặt thành SERVO_CLOSED_ANGLE (180 độ).
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
