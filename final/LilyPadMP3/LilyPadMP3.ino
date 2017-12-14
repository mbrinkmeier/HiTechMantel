/**
 * This sketch is the program for the LilyPad MP3 patch.
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
#include <SPI.h>            // To talk to the SD cars and MP3 schip
#include <SdFat.h>          // SD card file system
#include <SFEMP3Shield.h>   // MP3 decoder chip

#ifndef PSTR
  #define PSTR // Make Arduino Due happy
#endif

#define debugSerial Serial

#define EN_GPIO1 A2
#define SD_CS 9
#define NO_CMD 255

HiTechMantel mantel = HiTechMantel();

SFEMP3Shield player;
SdFat sd;

// up to 10 filenames can be stored with short (8,3) names, followed by a null character.

char filename[10][13];
int trackNo;
int noOfTracks;
bool playing;

bool debugging = true;

byte cmd;
byte dlen;
byte data[255];


void setup() {
  int index;
  byte result;
  SdFile file;
  char tempfilename[13];
  
  debugSerial.begin(9600);

  Wire.begin(ID_MP3);
  Wire.onReceive(handleMsg);

  delay(100);
  
  DBG_PRINT(F("LilyPad MP3 listening as ID "));
  DBG_PRINTLN(ID_MP3);

  // Read files from sd card
  DBG_PRINTLN(F("Accessing SD card ..."));

  result = sd.begin(SD_CS, SPI_HALF_SPEED);

  noOfTracks = -1;
  
  if (result != 1) {
    DBG_PRINTLN(F("Error accessing sd card"));
    noOfTracks = 0;
  } else {
    DBG_PRINTLN(F("SD card found"));
  }  

  DBG_PRINTLN(F("Starting MP3 player ..."));

  result = player.begin();

  if ((result != 0) && (result != 6)) {
    DBG_PRINT(F("error code "));
    DBG_PRINT(result);
    DBG_PRINT(F(", halting."));
  } else {
    DBG_PRINTLN(F("success!"));
  }

  if ( noOfTracks != 0 ) {
    DBG_PRINTLN(F("Reading up to 10 tracks ... "));
    if (debugging) debugSerial.flush();


    index = 0;
    sd.chdir("/tracks",true);
    while ( (file.openNext(sd.vwd(),O_READ)) && ( index < 10)) {
      file.getFilename(tempfilename);
      strcpy(filename[index],tempfilename);
      DBG_PRINT(F("Found track "));
      DBG_PRINTLN(filename[index]);      
      file.close();
      index++;
    }

    
    noOfTracks = index;

    DBG_PRINT(F("Number of tracks: "));
    DBG_PRINTLN(noOfTracks);
  }
  
  // Set initial volume
  player.setVolume(10,10);

  // Turn on amplifier
  digitalWrite(EN_GPIO1,HIGH);
  delay(2);

  DBG_PRINTLN("Listening on I2C");

  playing = false;
  cmd = NO_CMD;
}



void loop() {
  delay(10);
  if ( playing && !player.isPlaying() ) {
    trackNo = (trackNo+1) % noOfTracks;
    playTrack(filename[trackNo]);
  } 
  // else if (player.isPlaying() ) {
  // DBG_PRINTLN(player.currentPosition());
  // }

  // Execute command
  if (cmd != NO_CMD) {
     executeCmd();  
  }
}


/**
 * Handle incoming I2C Transmissions
 * 
 * Each transmission consists of
 * <cmd> <dlen> <data[0]> ... <data[dlen-1]>
 */
void handleMsg(int numBytes) {
  cmd = mantel.readFromWire();
  dlen = mantel.readFromWire();
  data[255];
  mantel.readData(dlen,data);
  mantel.emptyWire();
}


/**
 * Execute the last command
 */
void executeCmd() {      
  DBG_PRINT(F("Executing cmd: "));
  DBG_PRINT(cmd);
  DBG_PRINT(F(" and dlen: "));
  DBG_PRINTLN(dlen);
  if (debugging) mantel.debugData(data,dlen);
  DBG_PRINTLN();
  if (debugging) debugSerial.flush();

  
  switch (cmd) {
    case CMD_MP3_RESET:
      player.stopTrack();
      player.setVolume(255,255);
      trackNo = 0;
      delay(100);
      break;
    case CMD_MP3_PLAY:
    case CMD_MP3_PAUSE:
      // 0 -> 65, 255 -> 0
      data[0] = 65 - (65*data[0])/255;
      if ( data[0] >= 65 ) data[0] = 255;
      DBG_PRINT(F("Setting volume to "));
      DBG_PRINTLN(data[0]);
      player.setVolume(data[0],data[0]);

      if ( player.getState() == paused_playback ) {
        DBG_PRINTLN(F("Resume"));
        player.resumeMusic();
      } else if ( player.getState() == playback ) {
        DBG_PRINTLN(F("Pause"));
        player.pauseMusic();
      } else {
        playTrack(filename[trackNo]);
      }
      playing = true;
      break;
    case CMD_MP3_STOP:
      DBG_PRINTLN(F("Stop"));
      player.stopTrack();
      playing = false;
      break;
    case CMD_MP3_VOL_UP:
      break;
    case CMD_MP3_VOL_DOWN:
      break;
    case CMD_MP3_VOL_SET:
      // 0 -> 65, 255 -> 0
      data[0] = 65 - (65*data[0])/255;
      if ( data[0] >= 65 ) data[0] = 255;
      DBG_PRINT(F("Setting volume to "));
      DBG_PRINTLN(data[0]);
      player.setVolume(data[0],data[0]);
      break;
    case CMD_MP3_NEXT:
      trackNo = (trackNo+1) % noOfTracks;
      DBG_PRINT(F("Switching to track "));
      DBG_PRINTLN(trackNo);
      if ( player.isPlaying() ) {
        player.stopTrack();
        delay(100);
        playTrack(filename[trackNo]);
      }
      break;
    case CMD_MP3_PREV:
      DBG_PRINT(F("Switching to track "));
      DBG_PRINTLN(trackNo);
      trackNo = (trackNo-1) % noOfTracks;
      while ( trackNo <0 ) trackNo = trackNo + noOfTracks;
      if ( player.isPlaying() ) {
        player.stopTrack();
        delay(100);
        playTrack(filename[trackNo]);
      }
      break;
    case CMD_MP3_MUTE:
      player.setVolume(255,255);
      break;
   }
   if (debugging) debugSerial.flush();
   cmd= NO_CMD;
}


/**
 * Play a track
 */
void playTrack(char *name) {
  DBG_PRINT(F("Playing track "));
  DBG_PRINTLN(filename[trackNo]);  
  int res = player.playMP3(filename[trackNo]);
  if ( res != 0 ) {
    DBG_PRINT(F("Error "));
    DBG_PRINT(res);
    DBG_PRINTLN(F(" when trying to play track"));
  }
  delay(100);
}

