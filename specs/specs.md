# Spezifikationen der Komponenten

## Manschette

## RGB Rücken

Der Flora steuert die Farbe der Rückenfläche.

* Register 0
  0 <Rot> <Grün> <Blau>
  Stelle die Fläche auf die durch die drei Byte beschrieben Farbe

* Typ 2
* ID: 20
* Programm: RGB mit ID 20
* Pins
  - 10 (gelb) -> ROT
  - 9  (grün) -> GRÜN
  - 6  (blau) -> BLAU
  - GND (rot) -> GND (schwarz)


## RGB Ärmel

Der Flora steuert die Farbe des Ärmelstreifens.

* Register 0
  0 <Rot> <Grün> <Blau>
  Stelle die Fläche auf die durch die drei Byte beschrieben Farbe

  * Typ 2
* ID: 21
* Programm: RGB mit ID 21
* Pins
  - 10 (gelb) -> ROT
  - 9  (grün) -> GRÜN
  - 6  (blau) -> BLAU
  - GND (rot) -> GND (schwarz)


## RGB Gürtel

Der Flora steuert die Farbe des Gürtels.

* Register 0
  0 <Rot> <Grün> <Blau>
  Stelle die Fläche auf die durch die drei Byte beschrieben Farbe

* Typ 2
* ID: 22
* Programm: RGB mit ID 22
* Pins
  - 10 (gelb) -> ROT
  - 9  (grün) -> GRÜN
  - 6  (blau) -> BLAU
  - GND (rot) -> GND (schwarz)


## Herzmatrix

Funktionen:
- Matrix löschen
  Register 0
  
- Laufschrift anzeigen
  Register 1 <Text>
  
- Herzschlag anzeigen
  Register 2 <Frequenz>
  
- Farbe
  Register 3 <Rot> <Grün> <Blau>
  
- Muster ???

* Typ 1
* ID: 30
* Programm: Heart
* Pins
  - 10 (gelb) -> ROT
