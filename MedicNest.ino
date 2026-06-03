#include <DHT.h>
#include <Wire.h>
#include <RTClib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

void playSound(int frequency, int duration = 0);

// =====================================================
// OLED DISPLAY
// =====================================================

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  OLED_RESET
);

// =====================================================
// RTC CLOCK
// =====================================================

RTC_DS3231 rtc;

// =====================================================
// DHT11 SENSOR
// =====================================================

#define DHTPIN 2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

// =====================================================
// BUTTON
// =====================================================

#define BUTTON_PIN 3

// =====================================================
// RGB LED
// =====================================================

#define RED_PIN 5
#define GREEN_PIN 6
#define BLUE_PIN 7

// =====================================================
// BUZZER
// =====================================================

#define BUZZER_PIN 8

// =====================================================
// SETTINGS
// =====================================================

bool muted = true;
const int temperturecalibration = 0;

const float MIN_TEMP = 2.0;
const float MAX_TEMP = 8.0;

const unsigned long STORAGE_LIMIT =
24UL * 60UL * 60UL * 1000UL;

const unsigned long MEDICATION_INTERVAL =
30UL * 24UL * 60UL * 60UL * 1000UL;

const unsigned long MEDICATION_CRITICAL =
35UL * 24UL * 60UL * 60UL * 1000UL;

const unsigned long BUTTON_RELEASE_TIME = 60000UL;
const unsigned long REMINDER_INTERVAL = 3600000UL;

// =====================================================
// VARIABLES
// =====================================================

float tempC;

bool storageWarning = false;
unsigned long storageWarningStart = 0;

unsigned long lastMedicationTime = 0;

bool buttonReleased = false;
unsigned long releaseStart = 0;

unsigned long lastReminder = 0;

// =====================================================
// SETUP
// =====================================================

void setup() {

  Serial.begin(9600);

  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  dht.begin();

  if (!rtc.begin()) {
    Serial.println("RTC NOT FOUND");
    while (1);
  }

  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED FAILED");
    while(1);
  }

  display.clearDisplay();
  display.display();

  lastMedicationTime = millis();

  setGreen();

  Serial.println("MedicNest Started");
}

// =====================================================
// MAIN LOOP
// =====================================================

void loop() {

  DateTime now = rtc.now();

  tempC = dht.readTemperature();
  tempC = tempC+temperturecalibration;

  bool buttonState = digitalRead(BUTTON_PIN);
  Serial.print("Button state: ");
  Serial.println(buttonState);

  if(buttonState == HIGH) {

    if(!buttonReleased) {
      buttonReleased = true;
      releaseStart = millis();
    }

    if(millis() - releaseStart > BUTTON_RELEASE_TIME) {
      lastMedicationTime = millis();
      Serial.println("Medication registered");
      buttonReleased = false;
    }
  } else {
    buttonReleased = false;
  }

  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  //display.setCursor(0,0);

  //if(now.hour() < 10) display.print("0");
  //display.print(now.hour());
  //display.print(":");
  //if(now.minute() < 10) display.print("0");
  //display.println(now.minute());

  display.setCursor(0,12);
  display.print("Temp: ");
  display.print(tempC);
  display.println(" C");

  if(tempC >= MIN_TEMP && tempC <= MAX_TEMP) {

    setGreen();
    display.setCursor(0,24);
    display.println("Opbevaring: KORREKT");
    storageWarning = false;
    noTone(BUZZER_PIN);

  } else {

    setYellow();
    display.setCursor(0,24);
    display.println("Opbevaring: ADVARSEL");

    if(!storageWarning) {
      storageWarning = true;
      storageWarningStart = millis();
    }

    unsigned long elapsed = millis() - storageWarningStart;
    unsigned long remaining = STORAGE_LIMIT - elapsed;

    unsigned long hours = remaining / 3600000UL;
    unsigned long minutes = (remaining % 3600000UL) / 60000UL;

    display.setCursor(0,36);
    display.print(hours);
    display.print("t ");
    display.print(minutes);
    display.println("m tilbage");

    playSound(1000, 200);

    if(elapsed >= STORAGE_LIMIT) {

      setRed();

      display.clearDisplay();
      display.setTextSize(2);
      display.setCursor(0,20);
      display.println("UDLØBET");

      playSound(2500);

      Serial.println("Medicin udløbet");
    }
  }

  unsigned long medElapsed = millis() - lastMedicationTime;

  if(medElapsed < MEDICATION_INTERVAL) {

    unsigned long daysLeft =
      (MEDICATION_INTERVAL - medElapsed)
      / (24UL * 60UL * 60UL * 1000UL);

    display.setCursor(0,52);
    display.print("Dosis om ");
    display.print(daysLeft);
    display.print("d");

  } else if(medElapsed < MEDICATION_CRITICAL) {

    display.setCursor(0,52);
    display.println("Dosis Due");

    if(millis() - lastReminder > REMINDER_INTERVAL) {
      playSound(1200, 300);
      lastReminder = millis();
    }

  } else {

    display.setCursor(0,52);
    display.println("GLEMT DOSIS");
    playSound(2500);
  }

  display.display();

  delay(3000);
}

// =====================================================
// SOUND FUNCTION
// =====================================================

void playSound(int frequency, int duration) {

  if(!muted) {

    if(duration > 0) {
      tone(BUZZER_PIN, frequency, duration);
    } else {
      tone(BUZZER_PIN, frequency);
    }
  }
}

// =====================================================
// LED FUNCTIONS
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