////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Example for using UartRemote commands on Arduino  to receive. This example waits for receving one of the two commands: `led` or `add`.
// Below is the code that can run on the MicropPython sending side. 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*

# Python counter part

from uartremote import *
from utime import sleep_ms
ur=UartRemote()

ur.flush()

while True:
  ack,s=ur.call('add','2i',1,2)
  print("sum = ",s)
  sleep_ms(500)
  ur.call('led','4i',1,2,3,4)
  sleep_ms(500)
*/

#include <Arduino.h>
#include <struct.h>
#include <stdarg.h>
#include <string.h>
#include <cstdio>

#include "UartRemote.h"

char cmd[32]; // global temporary storage for command names

UartRemote uartremote;

void led(Arguments args) {
    int r,g,b,n;
    unpack(args,&r,&g,&b,&n);
    Serial.printf("LED on: %d, %d, %d, %d\n", r, g, b, n);
    uartremote.send_command("ledack","B",0);
}

void add(Arguments args) {
    int a,b;
    unpack(args,&a,&b);
    Serial.printf("sum on: %d, %d\n", a, b);
    int c=a+b;
    uartremote.send_command("imuack","i",c);
}
  
void setup() {
  // debug 
  Serial.begin(9600);

  uartremote.add_command("led", &led);
  uartremote.add_command("add", &add);
  

  Serial.println("\n\nWaiting for commands\n");
  uartremote.flush(); 
  // put your setup code here, to run once:
}


void loop() {
  int error = uartremote.receive_execute();
    if (error==1) {
      printf("error in receiving command\n");
    }
}

