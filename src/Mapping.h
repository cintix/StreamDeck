#ifndef MAPPING_H
#define MAPPING_H

#include <Arduino.h>
#include <FastGPIO.h>

const int buttonPins[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
const int numButtons = sizeof(buttonPins) / sizeof(buttonPins[0]);

const byte CMD_BTN_PRESS = 0x10;
const byte CMD_BTN_RELEASE = 0x20;
const byte CMD_VOL_CHANGE = 0x30;

// Button debounce
bool buttonStates[10] = {false};
unsigned long lastDebounceTime[10] = {0};
const unsigned long debounceDelay = 50;  // 50ms debounce

const int potPin = A0;  // Potentiometer pin

// Volume control
unsigned long lastVolumeSent = 0;
const unsigned long volumeInterval = 200;  // 200ms interval


class Mapping {
   private:
    /* data */
   public:
    Mapping() {
        for (int i = 0; i < numButtons; i++) {
            FastGPIO::pinMode(buttonPins[i], INPUT_PULLUP);
        }
    }
    void checkButtons() {
        for (int i = 0; i < numButtons; i++) {
            int reading = FastGPIO::read(buttonPins[i]);

            if (reading == LOW && !buttonStates[i]) {
                if (millis() - lastDebounceTime[i] > debounceDelay) {
                    buttonStates[i] = true;
                    FastGPIO::write(CMD_BTN_PRESS + i);
                    lastDebounceTime[i] = millis();
                }
            } else if (reading == HIGH && buttonStates[i]) {
                if (millis() - lastDebounceTime[i] > debounceDelay) {
                    buttonStates[i] = false;
                    FastGPIO::write(CMD_BTN_RELEASE + i);
                    lastDebounceTime[i] = millis();
                }
            }
        }
    }
    void readPotentiometer() {
        static int lastVolume = -1;
        int potValue = analogRead(potPin);
        int volume = map(potValue, 0, 1023, 0, 100);

        if (volume != lastVolume && millis() - lastVolumeSent > volumeInterval) {
            FastGPIO::write(CMD_VOL_CHANGE);
            FastGPIO::write(volume);
            lastVolume = volume;
            lastVolumeSent = millis();
        }
    }
};

#endif