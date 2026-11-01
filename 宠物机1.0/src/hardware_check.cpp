#include <TFT_eSPI.h>

namespace {
constexpr int kButtonA = 4;
constexpr int kButtonB = 5;
constexpr int kBuzzer = 21;
constexpr bool kTestBuzzer = false;

TFT_eSPI tft;
}

void setup() {
  pinMode(kButtonA, INPUT_PULLUP);
  pinMode(kButtonB, INPUT_PULLUP);
  pinMode(kBuzzer, OUTPUT);
  digitalWrite(kBuzzer, LOW);

  Serial.begin(115200);
  delay(500);
  Serial.println("[BOOT] Hardware check started");
  Serial.flush();

  Serial.println("[TFT] Initializing ST7789...");
  Serial.flush();
  tft.init();
  Serial.println("[TFT] init() returned");
  Serial.flush();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  tft.fillRect(0, 0, 240, 64, TFT_RED);
  tft.fillRect(0, 64, 240, 64, TFT_GREEN);
  tft.fillRect(0, 128, 240, 64, TFT_BLUE);
  tft.fillRect(0, 192, 240, 64, TFT_WHITE);
  tft.fillRect(0, 256, 240, 64, TFT_BLACK);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.drawString("ESP32-S3 / ST7789", 8, 270, 2);
  Serial.println("[TFT] Test pattern drawn");
  Serial.flush();

  if (kTestBuzzer) {
    digitalWrite(kBuzzer, HIGH);
    delay(80);
    digitalWrite(kBuzzer, LOW);
  }

  Serial.println("Hardware check ready. Buttons use INPUT_PULLUP; pressed = LOW.");
}

void loop() {
  static int previousA = HIGH;
  static int previousB = HIGH;
  const int currentA = digitalRead(kButtonA);
  const int currentB = digitalRead(kButtonB);

  if (currentA != previousA || currentB != previousB) {
    Serial.printf("A=%s B=%s\n",
                  currentA == LOW ? "pressed" : "released",
                  currentB == LOW ? "pressed" : "released");
    previousA = currentA;
    previousB = currentB;
  }
  delay(10);
}
