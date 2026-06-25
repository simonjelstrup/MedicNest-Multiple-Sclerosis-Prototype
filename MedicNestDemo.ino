#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// =====================================================
// OLED
// =====================================================

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// =====================================================
// PINS
// =====================================================

#define BUTTON_PIN 3

#define RED_PIN 5
#define GREEN_PIN 6
#define BLUE_PIN 7
#define BUZZER_PIN 8

// =====================================================
// STATES
// =====================================================

enum State {
  SAFE,
  WARNING_1,
  WARNING_2,
  EXPIRED
};

State currentState = SAFE;
State previousState = SAFE;

// =====================================================
// BUTTON DEBOUNCE
// =====================================================

bool lastButton = HIGH;
unsigned long lastPress = 0;
const unsigned long debounce = 250;

// =====================================================
// TIMERS
// =====================================================

unsigned long storageWarningStart = 0;

unsigned long lastDisplayUpdate = 0;
const unsigned long displayInterval = 1000; // update every second

unsigned long lastBeep = 0;

// =====================================================
// SETUP
// =====================================================

void setup() {

  Serial.begin(9600);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 failed");
    while (1);
  }

  display.clearDisplay();
  display.display();

  storageWarningStart = millis();

  setGreen();
  render();
}

// =====================================================
// LOOP
// =====================================================

void loop() {

  handleButton();

  // redraw immediately after state change
  if (currentState != previousState) {
    render();
    previousState = currentState;
  }

  // periodic refresh
  if (millis() - lastDisplayUpdate >= displayInterval) {
    lastDisplayUpdate = millis();
    render();
  }

  handleBuzzer();
}

// =====================================================
// BUTTON
// =====================================================

void handleButton() {

  bool b = digitalRead(BUTTON_PIN);

  if (b == LOW &&
      lastButton == HIGH &&
      millis() - lastPress > debounce) {

    currentState = (State)((currentState + 1) % 4);

    lastPress = millis();

    // optional feedback click
    tone(BUZZER_PIN, 2000, 50);
  }

  lastButton = b;
}

// =====================================================
// BUZZER
// =====================================================

void handleBuzzer() {

  unsigned long now = millis();

  switch (currentState) {

    case SAFE:
      noTone(BUZZER_PIN);
      break;

    case WARNING_1:

      // short beep every 5 seconds
      if (now - lastBeep > 5000) {
        tone(BUZZER_PIN, 1000, 200);
        lastBeep = now;
      }
      break;

    case WARNING_2:

      // short beep every 2 seconds
      if (now - lastBeep > 2000) {
        tone(BUZZER_PIN, 1200, 250);
        lastBeep = now;
      }
      break;

    case EXPIRED:
      noTone(BUZZER_PIN);
      break;
  }
}

// =====================================================
// DISPLAY
// =====================================================

void render() {

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  float fakeTemp = 6.2;

  if (currentState == SAFE) {

    setGreen();

    display.setCursor(0, 12);
    display.print("Temp: ");
    display.print(fakeTemp);
    display.println(" C");

    display.setCursor(0, 24);
    display.println("Opbevaring: KORREKT");

    display.setCursor(0, 40);
    display.println("Dosis om 12d");
  }

  else if (currentState == WARNING_1) {

    setYellow();

    display.setCursor(0, 12);
    display.print("Temp: ");
    display.print(fakeTemp);
    display.println(" C");

    display.setCursor(0, 24);
    display.println("Opbevaring: ADVARSEL");

    unsigned long elapsed =
      millis() - storageWarningStart;

    unsigned long limit =
      24UL * 60UL * 60UL * 1000UL;

    unsigned long remaining =
      (elapsed < limit) ? (limit - elapsed) : 0;

    unsigned long hours =
      remaining / 3600000UL;

    unsigned long minutes =
      (remaining % 3600000UL) / 60000UL;

    display.setCursor(0, 38);
    display.print(hours);
    display.print("t ");
    display.print(minutes);
    display.println("m tilbage");
  }

  else if (currentState == WARNING_2) {

    setYellow();

    display.setCursor(0, 12);
    display.print("Temp: ");
    display.print(fakeTemp);
    display.println(" C");

    display.setCursor(0, 30);
    display.println("Dosis risiko");

    display.setCursor(0, 42);
    display.println("1 dag forsinket");
  }

  else if (currentState == EXPIRED) {

    setRed();

    display.setTextSize(2);
    display.setCursor(0, 20);
    display.println("EXPIRED");
  }

  display.display();
}

// =====================================================
// LEDS
// =====================================================

void setGreen() {
  digitalWrite(RED_PIN, LOW);
  digitalWrite(GREEN_PIN, HIGH);
  digitalWrite(BLUE_PIN, LOW);
}

void setYellow() {
  digitalWrite(RED_PIN, HIGH);
  digitalWrite(GREEN_PIN, HIGH);
  digitalWrite(BLUE_PIN, LOW);
}

void setRed() {
  digitalWrite(RED_PIN, HIGH);
  digitalWrite(GREEN_PIN, LOW);
  digitalWrite(BLUE_PIN, LOW);
}