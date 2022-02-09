////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Example for using UartRemote on Arduino to send commands. This example sends every second alternating a `led` and a `add` command.
// Below is the code that can run on the MicropPython receiving side. 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*

# Python counter part

from uartremote import *
ur=UartRemote()

def led(n,r,g,b):
    print('led',n,r,g,b)
    
def add(a,b):
    print('adding',a,b)
    return(a+b)

ur.add_command(led)
ur.add_command(add,'i')

ur.loop()

*/


#include <Arduino.h>
#include <struct.h>
#include <stdarg.h>
#include <string.h>
#include <cstdio>

#include "UartRemote.h"

char cmd[32]; // global temporary storage for command names

UartRemote uartremote;
  
void setup() {
  // debug 
  Serial.begin(9600);

  Serial.println("\n\nSending commands\n");
  uartremote.flush(); // flush any characters from UART receive buffer
}

int i=0;
int s=0;
Arguments args;

void loop() {
  i+=1;
  i%=100; // max 100

  args=uartremote.call("led","4i",i+1,i+2,i+3,i+4);
  if (args.error==0) { // no error
    printf("received ack\n");
  } else { printf("error from call led\n");}

  delay(1000);
  
  args=uartremote.call("add","2i",i+1,i+2);
  if (args.error==0) { //no error
    unpack(args,&s);
    printf("Received sum=%d\n",s);
  } else { printf("error from call sum\n");}

  delay(1000);
}

