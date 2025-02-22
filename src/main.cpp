#include <Arduino.h>
#include <Communication.h>
#include <Mapping.h>
#include <FastGPIO.h>

#define DEBUG 0

// Hardware Pins
const int ledPin = 13;
bool statusLedOn = false;

void blinkError();

Communication protocol;
Mapping mapping;


void setup() {
    FastGPIO::pinMode(ledPin, OUTPUT);
    FastGPIO::write(ledPin, LOW);  // LED off until connected

    // analogReadResolution(5);  // Set resolution to 5 bits
}

void loop() {
    // Listen for incoming commands from the client
    if (FastGPIO::available()) {
        byte data = FastGPIO::read();
        protocol.handleCommunication(data);
    }

    if (protocol.isConnected()) {
        if (!statusLedOn) {
            FastGPIO::write(ledPin, HIGH);
            statusLedOn = true;
        }
        mapping.checkButtons();
    } else {
        statusLedOn = false;
        blinkError();  // Blink if not connected
    }
}

void blinkError() {
    static unsigned long lastBlinkTime = 0;
    static bool ledState = false;

    if (millis() - lastBlinkTime > 500) {
        ledState = !ledState;
        FastGPIO::write(ledPin, ledState);
        lastBlinkTime = millis();
    }
}