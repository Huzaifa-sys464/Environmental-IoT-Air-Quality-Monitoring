#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int mqPin = 34;       // MQ2 AO -> GPIO34
const int relayPin = 26;    // Relay IN -> GPIO26
int thresholdPercent = 70;  // adjust threshold as needed
const unsigned long readInterval = 800; // ms

void setup() {
  Serial.begin(115200);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 init failed");
    // continue even if display fails so serial still works
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("Pollution Monitor");
  display.display();
  delay(500);
}

int readMqPercent() {
  int raw = analogRead(mqPin);            // 0..4095 on ESP32
  int percent = map(raw, 0, 4095, 0, 100);
  percent = constrain(percent, 0, 100);
  return percent;
}

void updateDisplay(int percent, bool active) {
  // if display wasn't initialized, these calls are safe (they'll be no-op)
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(1);
  display.println("Pollution Monitor");
  display.println();
  display.setTextSize(2);
  display.print("Poll: ");
  display.print(percent);
  display.println("%");
  display.setTextSize(1);
  display.println();
  display.print("Thresh: ");
  display.print(thresholdPercent);
  display.println("%");
  display.println();
  display.print("System: ");
  display.println(active ? "ACTIVE" : "SAFE");
  display.display();
}

void loop() {
  int percent = readMqPercent();
  bool alarm = (percent >= thresholdPercent);

  // drive the relay
  digitalWrite(relayPin, alarm ? HIGH : LOW);

  // update OLED (if present)
  updateDisplay(percent, alarm);

  // debug to serial
  Serial.print("Raw ADC: ");
  Serial.print(analogRead(mqPin));
  Serial.print(" -> ");
  Serial.print(percent);
  Serial.print("%  Relay: ");
  Serial.println(alarm ? "ON" : "OFF");

  delay(readInterval);
}
