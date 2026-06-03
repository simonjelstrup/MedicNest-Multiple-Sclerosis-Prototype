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
// BUTTON + OUTPUT
// =====================================================

#define BUTTON_PIN 3

#define RED_PIN 5
#define GREEN_PIN 6
#define BLUE_PIN 7
#define BUZZER_PIN 8

void playSound(int frequency, int duration = 0);

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

// button debounce
bool lastButton = HIGH;
unsigned long lastPress = 0;
const unsigned long debounce = 300;

// =====================================================
// TIMERS (fake but realistic)
// =====================================================

unsigned long storageWarningStart = 0;

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

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();

  storageWarningStart = millis();

  setGreen();
}

// =====================================================
// LOOP
// =====================================================

void loop() {

  handleButton();
  render();

  delay(3000);
}

// =====================================================
// BUTTON (cycle states)
// =====================================================

void handleButton() {

  bool b = digitalRead(BUTTON_PIN);

  if (b == LOW && lastButton == HIGH &&
      millis() - lastPress > debounce) {

    currentState = (State)((currentState + 1) % 4);

    lastPress = millis();
  }

  lastButton = b;
}

// =====================================================
// UI
// =====================================================

void render() {

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  float fakeTemp = 6.2; // stable “good fridge temp”

  // =====================================================
  // SAFE
  // =====================================================

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

    noTone(BUZZER_PIN);
  }

  // =====================================================
  // WARNING 1 (storage risk)
  // =====================================================

  else if (currentState == WARNING_1) {

    setYellow();

    display.setCursor(0, 12);
    display.print("Temp: ");
    display.print(fakeTemp);
    display.println(" C");

    display.setCursor(0, 24);
    display.println("Opbevaring: ADVARSEL");

    unsigned long elapsed = millis() - storageWarningStart;
    unsigned long remaining = (24UL * 60UL * 60UL * 1000UL > elapsed)
      ? (24UL * 60UL * 60UL * 1000UL - elapsed)
      : 0;

    unsigned long hours = remaining / 3600000UL;
    unsigned long minutes = (remaining % 3600000UL) / 60000UL;

    display.setCursor(0, 36);
    display.print(hours);
    display.print("t ");
    display.print(minutes);
    display.println("m tilbage");

    playSound(1000, 200);
  }

  // =====================================================
  // WARNING 2 (dose compliance risk)
  // =====================================================

  else if (currentState == WARNING_2) {

    setYellow();

    display.setCursor(0, 12);
    display.print("Temp: ");
    display.print(fakeTemp);
    display.println(" C");
    display.setCursor(0, 40);
    display.println("Dosis 1 dag forsinket");

    playSound(1200, 250);
  }

  // =====================================================
  // EXPIRED
  // =====================================================

  else if (currentState == EXPIRED) {

    setRed();

    display.setTextSize(2);
    display.setCursor(0, 20);
    display.println("EXPIRED");

    noTone(BUZZER_PIN);
  }

  display.display();
}

// =====================================================
// SOUND
// =====================================================

void playSound(int frequency, int duration = 0) {
  if (duration > 0) tone(BUZZER_PIN, frequency, duration);
  else tone(BUZZER_PIN, frequency);
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