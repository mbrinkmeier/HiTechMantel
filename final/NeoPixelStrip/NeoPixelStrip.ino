/**
 * This sketch is the programm for the NeoPixel running light STRIP.strip->
 *   
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
#include <Wire.h>
#include <HiTechMantel.h>
#include <HiTechStrip.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoPixel.h>


HiTechStrip* strip = new HiTechStrip();


void setup() {
  strip->setup();
}


void loop() {
  strip->loop();
}

void initAniOwn(HiTechStrip *strip) {
  strip->frameAni = ANI_OWN;
  strip->frameNumber = LEN;
  strip->frameCount = 0;
  strip->frameDelay = 50;
  strip->colRed = 255;
  strip->colGreen = 0;
  strip->colBlue = 0;  
}

void doAniOwn(HiTechStrip *strip, int frame) {
  for ( int i = 0; i < LEN; i++ ) {
    strip->strip->setPixelColor(i,0,0,0);    
  }
  strip->strip->setPixelColor(frame,strip->mantel->rainbowRed(frame,21),strip->mantel->rainbowGreen(frame,21),strip->mantel->rainbowBlue(frame,21));
}



