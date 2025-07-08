#include <Wire.h>
#include <U8g2lib.h>
#include <Servo.h> // ← 서보모터 라이브러리 추가

const int ledPin = 13;
const int buttonPin = 2; // 버튼 핀
const int servoPin = 6;

unsigned long previousMills = 0;
const long interval = 1000;

int counter = 0;

bool ledState = false;
bool lastButtonState = HIGH;

int servoAngle = 180;

Servo myServo;

// I2C용 OLED 설정 (128x64, SSD1306)
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(
  U8G2_R0,    // 회전 없음
  U8X8_PIN_NONE // 리셋 핀 없음
);

void setup() {
  Serial.begin(115200);
  u8g2.begin();
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); // LED는 꺼진 상태로 시작

  pinMode(buttonPin, INPUT_PULLUP); // 내부 풀업 저항 사용

  myServo.attach(servoPin); // 서보 초기화
  myServo.write(servoAngle);

  // 초기 메시지
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_unifont_t_korean1); // 한글 폰트 설정
  u8g2.drawStr(0, 20, "OLED 준비됨...");
  u8g2.sendBuffer();
}

void loop() {
  
  unsigned long currentMills = millis();

  // 버튼 상태 읽기
  bool buttonState = digitalRead(buttonPin);

  // 버튼이 눌렸고(LOW), 이전 상태가 HIGH였을 때 → 토글
  if (buttonState == LOW && lastButtonState == HIGH) {
    ledState = !ledState; // 상태 반전
    digitalWrite(ledPin, ledState ? HIGH : LOW); // LED 제어

    // OLED에 표시
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_unifont_t_korean1);
    u8g2.drawStr(0, 16, "LED 상태:");
    u8g2.drawStr(0, 36, ledState ? "켜짐" : "꺼짐");
    u8g2.sendBuffer();
    // 디버깅 출력
    Serial.println(ledState ? "LED ON" : "LED OFF");
    
    delay(200); // 디바운스
    delay(200); // 디바운스
  }

  lastButtonState = buttonState;

  if (Serial.available()) {
    char buf[64] = {0};  // 수신 버퍼
    size_t len = Serial.readBytesUntil('\n', buf, sizeof(buf) - 1);  // 개행까지 읽음
    buf[len] = '\0';     // null-terminate

    // 앞뒤 공백 제거는 직접 처리해야 함
    char* trimmed = buf;
    while (*trimmed == ' ' || *trimmed == '\r') trimmed++;  // 앞 공백 제거

    int tlen = strlen(trimmed);
    while (tlen > 0 && (trimmed[tlen - 1] == ' ' || trimmed[tlen - 1] == '\r')) {
      trimmed[--tlen] = '\0';  // 뒤 공백 제거
    }

    // OLED 출력
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_unifont_t_korean1);
    u8g2.drawStr(0, 16, "수신됨:");
    u8g2.drawUTF8(0, 20, trimmed);
    u8g2.sendBuffer();

    Serial.print("Received: ");
    Serial.println(trimmed);

    // startsWith "SERVO "
    if (strncmp(trimmed, "SERVO ", 6) == 0) {
      int angle = atoi(trimmed + 6);  // "SERVO " 다음 문자부터 숫자 추출
      servoAngle = constrain(angle, 0, 180);  // 안전하게 제한
      myServo.write(servoAngle);  // 내부적으로 안전한 마이크로초로 변환해서 처리됨
    }
    else if(strncmp (trimmed, "SERVO RESET", 11) == 0)
    {
      servoAngle = 0;
      myServo.write(servoAngle);
    }
  }
}
