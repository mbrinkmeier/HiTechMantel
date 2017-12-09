#ifndef HITECHMANTEL_H
#define HITECHMANTEL_H

#include <Adafruit_GFX.h>
#include <Adafruit_NeoPixel.h>

#define DBG_PRINT(x) if (debugging) debugSerial.print(x);
#define DBG_PRINTLN(x) if (debugging) debugSerial.println(x);

#define CMD_PING 42
#define CMD_SELFTEST 43
#define START_BYTE 42

// RGB
#define ID_BACK 1
#define ID_ARM 2
#define ID_BELT 3

#define CMD_RGB_RESET 0
#define CMD_RGB_SET 1
#define CMD_RGB_RED 2
#define CMD_RGB_GREEN 3
#define CMD_RGB_BLUE 4

// NeoPixel Strip
#define ID_STRIP 4

#define CMD_STRIP_RESET 0
#define CMD_STRIP_COLOR 1
#define CMD_STRIP_RAINBOW 2
#define CMD_STRIP_RUNNING 3
#define CMD_STRIP_SPEED 10

// NeoPixel Matrix
#define ID_MATRIX 5

#define CMD_MATRIX_RESET 0
#define CMD_MATRIX_COLOR 1
#define CMD_MATRIX_RAINBOW 2
#define CMD_MATRIX_RUNNING 3
#define CMD_MATRIX_TEXT 4
#define CMD_MATRIX_SPEED 10
#define CMD_MATRIX_RED 5
#define CMD_MATRIX_GREEN 6
#define CMD_MATRIX_BLUE 7

// Motor
#define ID_MOTOR 6

#define CMD_MOTOR_STOP 0
#define CMD_MOTOR_UP 1
#define CMD_MOTOR_DOWN 2

// PIR Movement detector
#define ID_PIR_FRONT 7
#define ID_PIR_BACK 8

#define CMD_PIR_RESET 0
#define CMD_PIR_SET 2

// LilyPad MP3
#define ID_MP3 9

#define CMD_MP3_RESET  0
#define CMD_MP3_PLAY  1
#define CMD_MP3_PAUSE 2
#define CMD_MP3_STOP 3
#define CMD_MP3_VOL_UP 4
#define CMD_MP3_VOL_DOWN 5
#define CMD_MP3_VOL_SET 6
#define CMD_MP3_NEXT 7
#define CMD_MP3_PREV 8
#define CMD_MP3_MUTE 9

// PULSE
#define ID_PULSE 30

#define CMD_PULSE_STOP 0
#define CMD_PULSE_START 1

#define HEART_ID 30
#define PIR_FRONT_ID 50;
#define PIR_BACK_ID 51;

#define RESET_REG    0
#define SELFTEST_REG 42

#define RGB_SET_REG  1

#define PIR_SET_REG  1

#define HRT_COL_REG 1
#define HRT_TXT_REG 2
#define HRT_HRT_REG 3


class HiTechMantel {
  public:
    Adafruit_NeoPixel pixel;
    HiTechMantel();
    byte readFromWire();
    void readData(int len, byte buf[]);
    void emptyWire();
    void emptyWire(int count);

    // byte rainbowRed(int pos, int intervalLength);
    // byte rainbowGreen(int pos, int intervalLength);
    // byte rainbowBlue(int pos, int intervalLength);

    void debugData(byte buf[], int len);
};


#endif
