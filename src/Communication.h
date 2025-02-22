#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <Arduino.h>
#include <FastGPIO.h>

// Command Constants
const byte CMD_INIT = 0x01;
const byte CMD_ACK = 0x02;
const byte CMD_ERROR = 0x03;
const byte CMD_HEARTBEAT = 0x40;

const byte PROTOCOL_VERSION = 0x01;

const unsigned long HEARTBEAT_TIMEOUT = 5000;

class Communication {
   private:
    bool connected;
    unsigned long lastHeartbeat;
    void (*onCommandReceivedFunction)(byte);

    void checkConnectionHealth() {
        if (millis() - lastHeartbeat > HEARTBEAT_TIMEOUT) {
            connected = false;
        }
    }

   public:
    Communication() : connected(false), lastHeartbeat(0), onCommandReceivedFunction(nullptr) { FastGPIO::begin(115200); }

    void setEventHandler(void (*onCommandReceived)(byte)) { onCommandReceivedFunction = onCommandReceived; }
    inline bool isConnected() const { return connected; }
    void handleCommunication(byte data) {
        if (!isConnected() && data == CMD_INIT) {
            // Client requests handshake, send ACK with version
            FastGPIO::write(CMD_ACK);
            FastGPIO::write(PROTOCOL_VERSION);
            connected = true;
            lastHeartbeat = millis();
        } else if (data == CMD_HEARTBEAT) {
            lastHeartbeat = millis();  // Reset heartbeat timer
        } else if (connected) {
            checkConnectionHealth();
            if (onCommandReceivedFunction) {
                onCommandReceivedFunction(data);
            }
        } else {
            // If unexpected data is received before handshake, ignore it
            FastGPIO::write(CMD_ERROR);
        }
    }
};

#endif