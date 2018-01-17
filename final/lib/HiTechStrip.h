/**
 *  Copyright (c) 2017 Michael Brinkmeier (michael.brinkmeier@uni-osnabrueck.de)
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#ifndef HITECHSTRIP_H
#define HITECHSTRIP_H

#include <Adafruit_GFX.h>
#include <Adafruit_NeoPixel.h>
#include "HiTechMantel.h"

#define PIN 9
#define LEN 127
#define SHOW_DELAY 10
#define DIMMER 3

#define ANI_COLOR 0
#define ANI_RAINBOW 1
#define ANI_PULSE 2
#define ANI_OWN 3
#define ANI_RUNNING 4

class HiTechStrip {
  public:
    Adafruit_NeoPixel* strip;
    HiTechMantel *mantel;

    volatile int frameAni;     // The id of the animation
    volatile int frameDelay;   // The time between two frames
    volatile int frameNumber;  // The number of frames for the current animation
    volatile int frameCount;   // Counts the current frame
    volatile int colRed;
    volatile int colGreen;
    volatile int colBlue;
    volatile unsigned long lastFrame;
    volatile bool colChanged = false;
    volatile long colors[30];
    volatile byte data[255];

    HiTechStrip();

    void setup();
    void loop();
    void selftest();
    void setSpeed(byte speed);

    void initAniColor(int red, int green, int blue);
    void doAniColor(int frame);
    void initAniRainbow();
    void doAniRainbow(int frame);
    void initAniPulse();
    void doAniPulse(int frame);
    void initAniRunning();
    void doAniRunning(int frame);

    void clearStrip();
    void setColor(int led, long color);
    void showColors(int start, int len, boolean repeat, boolean reverse);
    long colorToLong(int red, int green, int blue);
    long mixColors(long col1, long col2);

    long runningPixel(int pos, int frame, int start, long color, boolean reverse);
    long runningPixels(int pos, int frame, int start, int len,  long color[], boolean reverse);
    long runningRainbow(int pos, int frame, int start, int len, boolean reverse);
};

void handleStripMsg(int numBytes);

#endif
