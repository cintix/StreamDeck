#ifndef FASTGPIO_H
#define FASTGPIO_H

#include <Arduino.h>

// Detect if we are compiling for ESP32 or AVR
#ifdef ESP32
#include "driver/gpio.h"  // ESP32 specific GPIO library
#elif defined(__AVR__)
#include <avr/io.h>  // AVR specific I/O library
#endif

class FastGPIO {
   public:
    static int available(void) { return Serial.available(); }
    static int read(void) { return Serial.read(); }
    // Fast digital write
    static void begin(unsigned long speed) { Serial.begin(speed); }
    static void write(int data) { Serial.write(data); }
    static void write(byte data) { Serial.write(data); }
    static void write(long data) { Serial.write(data); }
    static void write(unsigned long data) { Serial.write(data); }
    static void write(unsigned int data) { Serial.write(data); }
    static void write(char data) { Serial.write(data); }
    static void write(const char *data) { Serial.write(data); }

    static void write(int pin, bool value) {
#ifdef ESP32
        // ESP32 GPIO write
        if (value) {
            gpio_set_level(static_cast<gpio_num_t>(pin), 1);  // Set HIGH
        } else {
            gpio_set_level(static_cast<gpio_num_t>(pin), 0);  // Set LOW
        }
#elif defined(__AVR__)
        // AVR GPIO write
        uint8_t port = digitalPinToPort(pin);
        if (!port) return;
        volatile uint8_t *out = portOutputRegister(port);
        uint8_t bit = digitalPinToBitMask(pin);
        if (value) {
            *out |= bit;  // Set HIGH
        } else {
            *out &= ~bit;  // Set LOW
        }
#endif
    }

    // Fast digital read
    static bool read(int pin) {
#ifdef ESP32
        // ESP32 GPIO read
        return gpio_get_level(static_cast<gpio_num_t>(pin)) != 0;
#elif defined(__AVR__)
        // AVR GPIO read
        uint8_t port = digitalPinToPort(pin);
        if (!port) return false;
        volatile uint8_t *in = portInputRegister(port);
        uint8_t bit = digitalPinToBitMask(pin);
        return (*in & bit) != 0;
#endif
    }

    // Fast pinMode
    static void pinMode(int pin, uint8_t mode) {
#ifdef ESP32
        // ESP32 pinMode
        gpio_pad_select_gpio(static_cast<gpio_num_t>(pin));  // Select the GPIO pad
        gpio_set_direction(static_cast<gpio_num_t>(pin), static_cast<gpio_mode_t>(mode));
#elif defined(__AVR__)
        // AVR pinMode
        uint8_t port = digitalPinToPort(pin);
        if (!port) return;
        volatile uint8_t *ddr = portModeRegister(port);
        uint8_t bit = digitalPinToBitMask(pin);
        if (mode == OUTPUT) {
            *ddr |= bit;  // Set as OUTPUT
        } else {
            *ddr &= ~bit;  // Set as INPUT
            if (mode == INPUT_PULLUP) {
                volatile uint8_t *out = portOutputRegister(port);
                *out |= bit;  // Enable pull-up
            }
        }
#endif
    }

    // Attach interrupt
    static void attachInterrupt(int pin, void (*ISR)(void), int mode) {
#ifdef ESP32
        // ESP32 attach interrupt
        ::attachInterrupt(digitalPinToInterrupt(pin), ISR, mode);
#elif defined(__AVR__)
        // AVR attach interrupt
        uint8_t interruptNum = digitalPinToInterrupt(pin);
        if (interruptNum != NOT_AN_INTERRUPT) {
            ::attachInterrupt(interruptNum, ISR, mode);
        }
#endif
    }

    // Detach interrupt
    static void detachInterrupt(int pin) {
#ifdef ESP32
        // ESP32 detach interrupt
        ::detachInterrupt(digitalPinToInterrupt(pin));
#elif defined(__AVR__)
        // AVR detach interrupt
        uint8_t interruptNum = digitalPinToInterrupt(pin);
        if (interruptNum != NOT_AN_INTERRUPT) {
            ::detachInterrupt(interruptNum);
        }
#endif
    }
};

#endif
