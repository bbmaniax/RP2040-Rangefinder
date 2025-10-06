// RP2040-Rangefinder

#include <Arduino.h>
#include <Adafruit_SSD1306.h>

#if defined(DEBUG)
#define DEBUG_SERIAL_BEGIN(x) Serial.begin(x)
#define DEBUG_SERIAL_WAIT_FOR() \
  while (!Serial && millis() < 5000) { delay(100); }
#define DEBUG_SERIAL_PRINT(x) Serial.print(x)
#define DEBUG_SERIAL_PRINTLN(x) Serial.println(x)
#define DEBUG_SERIAL_PRINTHEX(x) Serial.print(x, HEX)
#define DEBUG_SERIAL_PRINTHEXLN(x) Serial.println(x, HEX)
#else
#define DEBUG_SERIAL_BEGIN(x)
#define DEBUG_SERIAL_WAIT_FOR()
#define DEBUG_SERIAL_PRINT(x)
#define DEBUG_SERIAL_PRINTLN(x)
#define DEBUG_SERIAL_PRINTHEX(x)
#define DEBUG_SERIAL_PRINTHEXLN(x)
#endif

#define HCSR04_TRIGGER_PIN 27
#define HCSR04_ECHO_PIN 6
#define HCSR04_MIN_DISTANCE_MM 20
#define HCSR04_MAX_DISTANCE_MM 4000
#define SENSOR_READ_INTERVAL_MS 100
#define DISPLAY_I2C_ADDRESS 0x3C
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 32

Adafruit_SSD1306 display(DISPLAY_WIDTH, DISPLAY_HEIGHT);

void setup() {
  DEBUG_SERIAL_BEGIN(9600);
  DEBUG_SERIAL_WAIT_FOR();
  DEBUG_SERIAL_PRINTLN();
  DEBUG_SERIAL_PRINTLN("--");

  pinMode(HCSR04_TRIGGER_PIN, OUTPUT);
  pinMode(HCSR04_ECHO_PIN, INPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, DISPLAY_I2C_ADDRESS)) { DEBUG_SERIAL_PRINTLN("Failed to initialize display"); }
  display.display();

  DEBUG_SERIAL_PRINTLN("Rangefinder");
  delay(1000);
}

void loop() {
  static unsigned long lastMillis = 0;
  unsigned long currentMillis = millis();

  if (currentMillis - lastMillis >= SENSOR_READ_INTERVAL_MS) {
    // DEBUG_SERIAL_PRINTLN("Time to read sensors");
    float distance = acquire();
    DEBUG_SERIAL_PRINTLN("D:" + CString(distance, 2));

    String value = String("NaN");
    if (!isnan(distance)) { value = String(distance, 2); }

    int16_t x, y;
    uint16_t w, h;
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextWrap(false);
    display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    display.getTextBounds(value, 0, 0, &x, &y, &w, &h);
    display.setCursor(DISPLAY_WIDTH - w, (DISPLAY_HEIGHT - h) / 2);
    display.print(value);
    display.display();

    lastMillis = currentMillis;
  }

  delay(10);
}

float acquire() {
  digitalWrite(HCSR04_TRIGGER_PIN, LOW);
  delayMicroseconds(5);
  digitalWrite(HCSR04_TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(HCSR04_TRIGGER_PIN, LOW);

  long duration = pulseIn(HCSR04_ECHO_PIN, HIGH);

  float distance = (duration / 2) / 2.91f;
  if (distance < HCSR04_MIN_DISTANCE_MM || distance > HCSR04_MAX_DISTANCE_MM) {
    distance = NAN;
  }

  return distance;
}
