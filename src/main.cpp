#include <Arduino.h>
#include <Communication.h>
#include <FastGPIO.h>
#include <Mapping.h>
#include <Display.h>

#define DEBUG 0

// Hardware Pins
const int ledPin = 12;
bool statusLedOn = false;

void blinkError();

Communication protocol;
Mapping mapping;
Display display;

void setup() {
    FastGPIO::begin(115200);
    FastGPIO::pinMode(ledPin, OUTPUT);
    FastGPIO::write(ledPin, LOW);  // LED off until connected
}

void loop() {
    // Listen for incoming commands from the client
    if (FastGPIO::available()) {
        byte data = FastGPIO::read();
        protocol.handleCommunication(data);
    }

    if (protocol.isConnected()) {
        display.showText("Connected...");
        protocol.checkConnectionHealth();
        if (!statusLedOn) {
            FastGPIO::write(ledPin, HIGH);
            statusLedOn = true;
        }
        mapping.checkButtons();
    } else {
        display.showText("Not connected...");
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