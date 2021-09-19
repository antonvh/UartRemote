#include "UartRemote.h"

#include <stdarg.h>
#include <string.h>
#include <cstdio>



#define RXD1 18
#define TXD1 19


//#define UART Serial1

char cmd[20]; // global temporary storage for command names

UartRemote uartremote;



void tst(unpackresult& args) {
   float a,b;
   uartremote.getvariables(args,&a,&b);
   
   float c=a+b;
   Serial.println("sum = "+String(c));

   uartremote.send("tstack","f",c);
}

void tstarr(unpackresult& args) {
   float a,b; unsigned char r[64];
   uartremote.getvariables(args,r);
   
   unsigned char q[64];
   for (int i=0; i<64; i++) {q[i]=i;}

   uartremote.send("tstack","a64B",q);
}


void led(unpackresult& args) {
   int a,b,c;  float d,e,f,g;
   uartremote.getvariables(args,&a,&b,&c,&d,&e,&f,&g);

   printf("%d %d %d %f\n",a,b,c,d);
   uartremote.send("ledack","b",0);
}


  
void setup() {
  // initialize serial port
  Serial1.begin(230400, SERIAL_8N1, RXD1, TXD1);
  // debug 
  Serial.begin(115200);
  // put your setup code here, to run once:

  uartremote.add_command("led", &led);
  uartremote.add_command("tst", &tst);
  uartremote.add_command("tstarr",&tstarr);
}



void loop() {
  // put your main code here, to run repeatedly:
  unpackresult rcvunpack = uartremote.receive(cmd);
  Serial.println("Command received: "+String(cmd));
  uartremote.command(cmd,rcvunpack);
 
}



/*
# on micropython platform, use the follwoing code to test with this Arduino example


from uartremote import *
u=UartRemote()

for i in range(10):                                                                       
   q=u.send_receive('tst','2f',i*2,12.5)  


 
 */