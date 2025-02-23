#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <U8glib.h>
#include <Wire.h>

U8GLIB_SSD1306_128X32 u8g(U8G_I2C_OPT_NONE);

class Display {
   private:
    const char *lineOne;
    const char *lineTwo;
    int textSize;
    void draw() {
        u8g.firstPage();
        do {
            u8g.setColorIndex(1);
            u8g.setFont(u8g_font_7x14B);
            if (textSize == 0) {
                if (lineOne) u8g.drawStr(0, 10, lineOne);
                if (lineTwo) u8g.drawStr(0, 24, lineTwo);
            }

        } while (u8g.nextPage());
    }

   public:
    Display() : lineOne(nullptr), lineTwo(nullptr), textSize(0) {}
    void showText(const char *text) {
        lineOne = text;
        lineTwo = nullptr;
        draw();
    }
    void showTexts(const char *text, const char *text2) {
        lineOne = text;
        lineTwo = text2;
        draw();
    }
};

#endif
