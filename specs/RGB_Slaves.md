# RGB Slaves

- Typ: Flora Patch II
- ID: 1 (Rücken), 2 (Arm), 3 (Gürtel)
- Sketch: RGB mit der jeweiligen ID
- Pins (Flora -> Bus)
    - 10 (gelb) -> ROT
    - 9  (grün) -> GRÜN
    - 6  (blau) -> BLAU
    - GND (rot) -> GND (schwarz)

## Kommandos

### Reset

- CMD_ID: 0 (CMD_RGB_RESET)
- DLEN: 0

Setzt die angezeigte Farbe auf (0,0,0)

### Set Color

- CMD_ID: 1 (CMD_RGB_SET)
- DLEN: 3
- DATA_0: Rot-Anteil (0..255)
- DATA_1: Grün-Anteil (0..255)
- DATA_2: Blau-Anteil (0..255)