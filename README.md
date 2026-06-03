# MedicNest-Multiple-Sclerosis-Prototype
Medication storage prototype for people with Multiple Sclerosis (MS). Built on an Arduino Nano, the system monitors temperature and humidity, provides medication reminders using a DS3231 RTC, and delivers visual and auditory feedback through an OLED display, RGB LEDs, and a buzzer.
# MS Medication Storage Prototype

A smart medication storage prototype designed to support people with Multiple Sclerosis (MS). The system monitors storage conditions and provides medication reminders using visual and auditory feedback.

## Features

* Temperature monitoring
* Humidity monitoring
* Medication reminders
* OLED status display
* RGB LED indicators
* Buzzer notifications
* Real-time clock (RTC)
* Reminder acknowledgement button

## Hardware

The prototype is built using:

* Arduino Nano
* DHT11 Temperature & Humidity Sensor
* DS3231 RTC Module
* 128x64 I2C OLED Display
* RGB LED
* Piezo Buzzer
* Push Button
* 9V Battery

## Wiring

### Arduino Nano Pinout

| Pin | Component        |
| --- | ---------------- |
| D2  | DHT11 Data       |
| D3  | Push Button      |
| D5  | Red LED          |
| D6  | Green LED        |
| D7  | Blue LED         |
| D8  | Buzzer           |
| A4  | SDA (OLED + RTC) |
| A5  | SCL (OLED + RTC) |

## How It Works

The DHT11 sensor continuously measures temperature and humidity inside the medication container.

The DS3231 RTC keeps track of time and controls the medication reminder schedule.

When it is time to take medication, the system activates the buzzer and displays a reminder on the OLED screen. The user can acknowledge the reminder by pressing the button.

The RGB LED provides a quick visual indication of the system status.

## Required Libraries

Install the following libraries through the Arduino IDE Library Manager:

* Adafruit GFX
* Adafruit SSD1306
* RTClib
* DHT Sensor Library
* Wire

## Installation

1. Clone the repository.
2. Open the Arduino project in Arduino IDE.
3. Install the required libraries.
4. Connect the hardware according to the schematic.
5. Upload the code to the Arduino Nano.

## Schematic
<img width="606" height="635" alt="Skærmbillede 2026-06-03 kl  11 31 22" src="https://github.com/user-attachments/assets/54979623-7b22-45f2-9842-607cf510459e" />


## Project Background

This prototype was developed as part of a university thesis project exploring how technology can support medication management for people living with Multiple Sclerosis.

## License

This project is intended for academic and research purposes only.
