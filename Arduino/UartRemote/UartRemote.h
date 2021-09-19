#ifndef UARTREMOTE_H
#define UARTREMOTE_H

#include <Arduino.h>

#include <stdarg.h>
#include <string.h>
#include <cstdio>
#include <iostream>
#include <unordered_map>
// #include "packunpack.h"
#define RXD1 18
#define TXD1 19

#if defined ESP8266
  #define UART Serial
#elif defined ESP32
  #define UART Serial1
#endif

enum datatype { t_int, t_byte, t_float }; // t_char_array,


 struct variable {
    void* data;
    datatype data_type;
    int size; // size of the array, or 0 if this is not an array
}; 

 struct unpackresult {
    variable* vars;
    int size; 
    // variable& operator[](int i) {
    //     return vars[i];
    // }

    ~unpackresult() {
        delete[] (char*)vars[size].data;
        delete[] vars;
    }
};

struct packresult {
    char* data;
    int size; 
    ~packresult() {
        delete[] data;
    }
};

struct cmd_map{
  char* cmd;
  void (*function) (unpackresult&);
}  ;


class UartRemote
{

  public:

    UartRemote();

    void getvariables(unpackresult& args,  ...);

    void parseformat(const char* format, int& count, int& totalsize);
    unpackresult unpack(const char* format, const char* data);
    packresult vpack(const char* format, va_list list);
    packresult pack(const char* format, ...);
    int available();
    unsigned char readserial1();
    void flush();
    void command(char cmd[],unpackresult& rcvunpack);
    void send(const char* cmd, const char* format, ... );
    unpackresult receive(char* cmd);
    void add_command(char * cmd,  void (*func)(unpackresult&) );

  cmd_map cmds[20];
  int nr_cmds;

  private:
  char data_buf[400];
  char format[40];
};


#endif // UARTREMOTE_H
