#include "DHT.h"
#include <Wire.h>
#include <LiquidCrystal.h>
#include "BluetoothSerial.h"

#define SRF04_ECHO_PIN 22
#define SRF04_TRIG_PIN 21
#define DHTPIN 2   // Chân dht11
#define LDRPIN 14  // Chân A0 của cảm biến ánh sáng
#define DHTTYPE DHT11
#define den 5
#define quat 19
#define denNgu 18
#define dieuHoa 25
DHT dht(DHTPIN, DHTTYPE);
BluetoothSerial SerialBT;
String message = "";
int distance;
unsigned long previousMillis = 0;
unsigned long autoMillis = 0;

void setup() {
  Serial.begin(115200);
  pinMode(den, OUTPUT);
  pinMode(quat, OUTPUT);
  pinMode(denNgu, OUTPUT);
  pinMode(dieuHoa, OUTPUT);
  pinMode(LDRPIN, INPUT);
  pinMode(SRF04_TRIG_PIN, OUTPUT);
  pinMode(SRF04_ECHO_PIN, INPUT);
  SerialBT.begin("ESP32_NhaThongMinh");  // Tên thiết bị Bluetooth
  Serial.println("Thiết bị đã khởi động, đợi kết nối qua Bluetooth!");
  dht.begin();
}

void DoDoAm_NhietDo() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Lỗi đọc dữ liệu");
  } else {
    SerialBT.print("-Nhiệt độ " + String(t) + "°C ~ Độ ẩm " + String(h) + "%");
  }
}

void LightControl() {
  // Đọc giá trị ánh sáng từ cảm biến
  int lightValue = analogRead(LDRPIN);
  Serial.println(lightValue);
  // Ngưỡng ánh sáng để bật đèn
  int lightThreshold = 4000;
  // càng sáng thì giá trị analogRead càng thấp
  // Kiểm tra ánh sáng và bật đèn nếu cần
  if (lightValue > lightThreshold) {
    digitalWrite(denNgu, HIGH);  // Bật đèn ngủ
  }
  // else {
  //   digitalWrite(den, LOW);   // Tắt đèn
  //   SerialBT.println("Đèn tắt");
  // }
}
int readSRF04() {
  unsigned long currentMillis = millis();
  // Kiểm tra xem đã đến lúc đọc cảm biến chưa (mỗi 10 mili giây)
  if (currentMillis - previousMillis >= 10) {
    previousMillis = currentMillis;
    // Kích hoạt cảm biến siêu âm để đo khoảng cách
    digitalWrite(SRF04_TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(SRF04_TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(SRF04_TRIG_PIN, LOW);
    // Đọc pin echo để lấy thời gian trong micro giây
    unsigned long duration = pulseIn(SRF04_ECHO_PIN, HIGH);
    // Tính toán khoảng cách theo đơn vị centimet
    distance = duration * 0.034 / 2;
    Serial.println(String(distance)+" cm");
    // Kiểm tra xem khoảng cách có nằm trong khoảng mong muốn không (từ 1 đến 10 cm)
    if (distance < 10) {
      autoMillis = currentMillis;
      return 1; // Trả về 1 nếu điều kiện được đáp ứng
    }
  }
  return 0; // Trả về 0 nếu điều kiện không được đáp ứng
}



void loop() {
  // Kiểm tra kết nối Bluetooth
  // if (SerialBT.connected()) {
  //   Serial.println("Đã kết nối qua Bluetooth!");
  // }
  // Xử lý dữ liệu từ kết nối Bluetooth
  if (SerialBT.available()) {
    char incomingChar = SerialBT.read();
    if (incomingChar != '\n') {
      message += String(incomingChar);
    } else {
      message = "";
    }
    Serial.write(incomingChar);
  }
  processBluetoothCommand(message);  // Xử lý lệnh Bluetooth
  if(readSRF04()==1){
    digitalWrite(den, HIGH);  // Bật đèn
  }
  // Thực hiện các chức năng khác
  DoDoAm_NhietDo();
  LightControl();
}

void processBluetoothCommand(String command) {
  if (command.equals("den_on")) {
    digitalWrite(den, HIGH);
    SerialBT.println("Đèn bật");
    message = "";
  } else if (command.equals("den_off")) {
    digitalWrite(den, LOW);
    SerialBT.println("Đèn tắt");
    message = "";
  } else if (command.equals("quat_on")) {
    digitalWrite(quat, HIGH);
    SerialBT.println("Quạt bật");
    message = "";
  } else if (command.equals("quat_off")) {
    digitalWrite(quat, LOW);
    SerialBT.println("Quạt tắt");
    message = "";
  } else if (command.equals("den_ngu_on")) {
    digitalWrite(denNgu, HIGH);
    SerialBT.println("Đèn ngủ bật");
    message = "";
  } else if (command.equals("den_ngu_off")) {
    digitalWrite(denNgu, LOW);
    SerialBT.println("Đèn ngủ tắt");
    message = "";
  } else if (command.equals("dieu_hoa_on")) {
    digitalWrite(dieuHoa, HIGH);
    SerialBT.println("Điều hòa bật");
    message = "";
  } else if (command.equals("dieu_hoa_off")) {
    digitalWrite(dieuHoa, LOW);
    SerialBT.println("Điều hòa tắt");
    message = "";
  }
}