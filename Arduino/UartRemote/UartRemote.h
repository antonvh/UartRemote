#ifndef UARTREMOTE_H
#define UARTREMOTE_H

#include <Arduino.h>

#include <stdarg.h>
#include <string.h>
#include <cstdio>
#include <iostream>
#include <unordered_map>

extern "C" {
#include "struct.h"
}

// https://github.com/svperbeast/struct

#define RXD1 18
#define TXD1 19

#if defined ESP8266
  #define UART Serial
#elif defined ESP32
  #define UART Serial1
#endif


struct Arguments {
    void* buf;
    const char* fmt;
    int error;
    template<typename... Args> friend void unpack(const Arguments& a, Args... args) {
        struct_unpack(a.buf,a.fmt, args...);
    }
};

struct cmd_map{
  const char* cmd;
  void (*function) (Arguments);
}  ;


class UartRemote
{

  public:

    UartRemote();

    int available();
    unsigned char readserial1();
    void flush();
    Arguments pack( unsigned char* buf, const char* format, ...);
    // template<typename... Args> Arguments pack( const char* format, Args... args);
    void command(const char* cmd,Arguments rcvunpack);
    void send_command(const char* cmd, const char* format, ...);
    void testsend(const char* cmd,unsigned char* buf, const char* format, ...);
    

    Arguments receive_command(char* cmd);
    int receive_execute();
    Arguments testreceive(char* cmd, unsigned char* buf);
    Arguments call(const char* cmd, const char* format, ...);
    void add_command(const char * cmd,  void (*func)(Arguments) );

  cmd_map cmds[20];
  int nr_cmds;

  private:
   char data_buf[256];
  char format[40];
};


#endif // UARTREMOTE_H
