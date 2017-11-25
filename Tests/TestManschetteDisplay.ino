#include "Nextion.h"

NexButton b0 = NexButton(0, 1, "b0");
NexButton b1 = NexButton(0, 2, "b1");

int pin = 7;

// alle Elemente der Seite in eine Liste einfügen
NexTouch *nex_listen_list[] =
{
  &b0,
  &b1,
  NULL
};

// definiert, was beim Drücken des ON-Buttons passiert
void b0PushCallback(void *ptr)
{
  //dbSerialPrintln("onButtonPushCallback");
  digitalWrite(pin, HIGH);    
}

// definiert, was beim Drücken des OFF-Buttons passiert
void b1PushCallback(void *ptr)
{
  //dbSerialPrintln("offButtonPushCallback");
  digitalWrite(pin, LOW);
}

void setup(void)
{
  /* Set the baudrate which is for debug and communicate with Nextion screen. */
  nexInit();
  //Serial.begin(115200);

  // Callback-Funktionen an die entsprechenden Elemente anhängen
  b0.attachPush(b0PushCallback);
  b1.attachPush(b1PushCallback);

  pinMode(pin, OUTPUT);
  
  //dbSerialPrintln("setup done");
}

void loop(void)
{
  nexLoop(nex_listen_list);
}
