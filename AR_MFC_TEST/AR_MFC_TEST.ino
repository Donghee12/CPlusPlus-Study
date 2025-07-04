// #include <Wire.h>
// #include <Adafruit_GFX.h>
// #include <Adafruit_SSD1306.h>

// // OLED 디스플레이 해상도 설정
// #define SCREEN_WIDTH 128
// #define SCREEN_HEIGHT 64



// // OLED 객체 생성 (리셋 핀은 사용하지 않음, -1)
// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// void setup() {
//   // 시리얼 통신 시작
//   Serial.begin(115200);

//   // OLED 초기화
//   if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
//     Serial.println(F("SSD1306 초기화 실패"));
//     while (true); // 무한루프로 멈춤
//   }

//   // 초기 메시지
//   display.clearDisplay();
//   display.setTextSize(1);             // 텍스트 크기 (6x8 픽셀)
//   display.setTextColor(SSD1306_WHITE);
//   display.setCursor(0, 0);
//   display.println(F("OLED Ready..."));
//   display.display();

//   delay(1000);
// }

// void loop() {
//   if (Serial.available()) {
//     String received = Serial.readStringUntil('\n'); // 줄바꿈까지 수신

//     // OLED에 출력
//     display.clearDisplay();
//     display.setCursor(0, 0);
//     display.setTextSize(1);
//     display.setTextColor(SSD1306_WHITE);
//     display.println("Received:");
//     display.println(received);
//     display.display();

//     // 시리얼로도 출력
//     Serial.print("Received: ");
//     Serial.println(received);
//   }
// }
#include <Wire.h>
#include <U8g2lib.h>

// I2C용 OLED 설정 (128x64, SSD1306)
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(
  U8G2_R0,    // 회전 없음
  U8X8_PIN_NONE // 리셋 핀 없음
);

void setup() {
  Serial.begin(115200);
  u8g2.begin();

  // 초기 메시지
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_unifont_t_korean1); // 한글 폰트 설정
  u8g2.drawStr(0, 20, "OLED 준비됨...");
  u8g2.sendBuffer();
}

void loop() {
  if (Serial.available()) {
    String received = Serial.readStringUntil('\n'); // 개행까지 읽기

    // 버퍼에 문자열 출력
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_unifont_t_korean1); // 한글 폰트
    u8g2.drawStr(0, 16, "수신됨:");
    u8g2.drawUTF8(0, 20, received.c_str());   // 한글도 출력 가능
    u8g2.sendBuffer();

    // 시리얼로도 출력
    Serial.print("Received: ");
    Serial.println(received);
  }
}
