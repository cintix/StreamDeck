#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <U8glib.h>
#include <Wire.h>

U8GLIB_SSD1306_128X32 u8g(U8G_I2C_OPT_NONE);

class Display {
   private:
    const char *statusLine;
    const char *infoLine;
    int textSize;
    unsigned long lastUpdateTime;  
    unsigned long resetTime;       

    void draw() {
        u8g.firstPage();
        do {
            u8g.setColorIndex(1);
            if (textSize == 0) {
                u8g.setFont(u8g_font_7x14B);
                if (statusLine) u8g.drawStr(0, 10, statusLine);
                u8g.setFont(u8g_font_9x15B);
                if (infoLine) u8g.drawStr(0, 30, infoLine);
            }
        } while (u8g.nextPage());
    }

   public:
    Display() : statusLine(nullptr), infoLine(nullptr), textSize(0), lastUpdateTime(0), resetTime(3000) {}

    void showStatus(const char *text) {
        statusLine = text;
        draw();
    }

    void showText(const char *text) {
        infoLine = text;
        lastUpdateTime = millis();
        draw();
    }

    void update() {
        if (millis() - lastUpdateTime > resetTime) {
            if (infoLine != nullptr) {
                infoLine = nullptr;
                draw();
            }
        }
    }
};

#endif
