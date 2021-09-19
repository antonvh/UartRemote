#include "UartRemote.h"


UartRemote::UartRemote() {}



void UartRemote::add_command(char* cmd, void (*func)(unpackresult&)){
  cmds[nr_cmds].cmd=cmd;
  cmds[nr_cmds].function=func;
  nr_cmds++;
}

void UartRemote::getvariables(unpackresult& args,  ...) {
    va_list list;
    va_start(list,args.size);
    for(int i=0;i<args.size;++i) {
        auto& v = args.vars[i];
        if(v.size>=1) {
            switch (args.vars[i].data_type) {
                case t_byte:
                    memcpy(va_arg(list, char*),v.data, v.size);
                    break;
                case t_int:
                    memcpy(va_arg(list, int*),v.data, v.size*4);
                    break;
                case t_float:
                    memcpy(va_arg(list, float*),v.data, v.size*4);
                    break;
                default:
                break;
            }
        } else {
            switch (v.data_type) {
                case t_byte:
                    *va_arg(list, char*) = *(char*)v.data;
                    break;
                case t_int:
                    *va_arg(list, int*) = *(int*)v.data;
                    break;
                case t_float:
                    *va_arg(list, float*) = *(float*)v.data;
                    break;
                default:
                break;
            }
        }
    }
}


void UartRemote::parseformat(const char* format, int& count, int& totalsize) {
    for(int j=0;format[j]!=0;j++) {
        bool isarray = false;
        if(format[j]== 'a') {
            ++j;
            isarray = true;
        }
        int amount = 0;
        while(format[j]<='9' and format[j]>='0') {
            amount = amount*10+(format[j++]-'0');
        }
        if(amount==0) amount=1;
        if(isarray) {
            count++;
        } 
        while(amount--) {
            switch (format[j]) {
                case 'b':
                    totalsize++;
                    break;
                case 'B':
                    totalsize++;
                    break;
                case 'i':
                    totalsize+=4;
                    break;
                case 'I':
                    totalsize+=4;
                    break;
                case 'f':
                    totalsize+=4;
                    break;
                default:
                    break;
            }
            count+=!isarray;
        }
    }
    // printf("parseformat count=%d totalsize=%d\n",count,totalsize);
}



unpackresult UartRemote::unpack(const char* format, const char* data) {
    int totalsize=0,count=0;
    parseformat(format,count,totalsize);
    variable* variables = new variable[count+1];

    char* cpydata = new char[totalsize];
    memcpy(cpydata,data,totalsize);

    int variableid=0;
    totalsize=0;
    for(int j=0;format[j]!=0;j++) {
        bool isarray = false;
        if(format[j]== 'a') {
            ++j;
            isarray = true;
        }
        int amount = 0;
        while(format[j]<='9' and format[j]>='0') {
            amount = amount*10+(format[j++]-'0');
        }
        if(amount==0) amount=1;
       
        if(isarray) {
            int cursize=-1;
            switch (format[j]) {
                case 'b':
                case 'B':
                    cursize= amount;
                    break;
                case 'i':
                case 'I':
                case 'f':
                    cursize=4*amount;
                    break;
                default:
                    break;
            }
            auto& v = variables[variableid];
            v.data = (void*)(data+totalsize);
            v.size = amount;
            switch(format[j]) {
                case 'b':
                case 'B':
                    v.data_type = t_byte;
                    break;
                case 'i':
                case 'I':
                    v.data_type = t_int;
                    break;
                case 'f':
                    v.data_type = t_float;
                    break;
                default:
                    break;
            }

            totalsize+=cursize;
            variableid++;
        } else while(amount--) {
            auto& v = variables[variableid];
            v.data = (void*)(cpydata+totalsize);
            v.size=0;
            switch (format[j]) {
                case 'b':
                case 'B':
                    v.data_type = t_byte;
                    totalsize++;
                    break;
                case 'i':
                case 'I':
                    v.data_type = t_int;
                    totalsize+=4;
                    break;
                case 'f':
                    v.data_type = t_float;
                    totalsize+=4;
                    break;
                default:
                    break;
            }
            variableid++;
        }
    }
    variables[count].data = (void*)cpydata;
    return {variables,count};
    
}

packresult UartRemote::vpack(const char* format, va_list list) {
    // printf("format string: %s \n",format);
    int count=0, totalsize=0;
    parseformat(format,count,totalsize);
    // printf("count = %d, totalsize = %d", count,totalsize);
    char* ans = new char[totalsize];

    totalsize=0;
    for(int j=0;format[j]!=0;j++) {
        bool isarray = false;
        if(format[j]== 'a') {
            ++j;
            isarray = true;
        }
        int amount = 0;
        while(format[j]<='9' and format[j]>='0') {
            amount = amount*10+(format[j++]-'0');
        }
        if(amount==0) amount=1;
       
        if(isarray) {
            int cursize=-1;
            switch (format[j]) {
                case 'b':
                case 'B':
                    cursize= amount;
                    break;
                case 'i':
                case 'I':
                case 'f':
                    cursize=4*amount;
                    break;
                default:
                    break;
            }
            memcpy(ans+totalsize, va_arg(list,void*), cursize);
            totalsize+=cursize;
        } else while(amount--) {
            switch (format[j]) {
                case 'b':
                    ans[totalsize] = (char)va_arg(list, int);
                    totalsize++;
                    break;
                case 'B':
                    ans[totalsize] = (unsigned char)va_arg(list, int);
                    totalsize++;
                    break;
                case 'i':
                    *(int*)(ans+totalsize) = va_arg(list, int);
                    totalsize+=4;
                    break;
                case 'I':
                    *(unsigned int*)(ans+totalsize) = va_arg(list, unsigned int);
                    totalsize+=4;
                    break;
                case 'f':
                    *(float*)(ans+totalsize) = (float)va_arg(list, double);
                    totalsize+=4;
                    break;
                default:
                    break;
            }
        }
    }
//     for (int i; i<totalsize; i++) { printf("%02x ",ans[i]);}
//     printf("\n");
    packresult result = {ans,totalsize};
    return result;

}

packresult UartRemote::pack(const char* format, ...) {
  va_list list;
  va_start(list,format);
  auto result = vpack(format,list);
  va_end(list);
  return result;
}

void UartRemote::command(char cmd[],unpackresult& rcvunpack) {
  for (int i=0; i<nr_cmds; i++) {
     //printf("checking cmd %s\n",cmds[i].cmd);
     if (strcmp(cmd,cmds[i].cmd)==0) {
         (*cmds[i].function)(rcvunpack);
     }
  }
 // printf("return from UartRemote::command\n");
}

unsigned char UartRemote::readserial1() {
  while (UART.available()==0) {
  }
  return UART.read();
}

int UartRemote::available()
{
   if (UART.available()>0) {
     return 1;
   }
   else {
     return 0;
   }
}

void UartRemote::flush() {
    delay(100);
    while (available()) {
        UART.read();
    }
}

void UartRemote::send(const char* cmd, const char* format, ... ) {

  va_list list;
  va_start(list, format);
  auto p=vpack(format, list);
  va_end(list);    
    
  unsigned char lc = strlen(cmd);
  unsigned char lf = strlen(format);
  unsigned char l=lc+lf+p.size+2; // +2 for the length fields of cmd and format
  int i;
  UART.write('<');
  UART.write(l);
  UART.write(lc);
  for (i=0; i<lc; i++) UART.write(cmd[i]);
  UART.write(lf);
  for (i=0; i<lf; i++) UART.write(format[i]);
  //printf("p.size= %d\n",p.size);
  for (i=0; i<p.size; i++) UART.write(p.data[i]);
  UART.write('>');
}

unpackresult UartRemote::receive(char* cmd) {
  char delim=readserial1();
  if (delim!='<') {
    strcpy(cmd,"error");
    flush();
    send("error","b",'!');
    return unpack("b","!");
  }
  //printf("left delim %d   %d!=60: %d\n",delim,delim,delim!=60);
  unsigned char l =readserial1();
  unsigned char lc = readserial1();

  for (int i=0; i<lc; i++) {
    cmd[i]=readserial1();
  }
  cmd[lc]=0;
  unsigned char lf = readserial1();
  for (int i=0; i<lf; i++) {
    format[i]=readserial1();
  }
  format[lf]=0;
  int l_data=l-2-lc-lf;
  for (int i=0; i<l_data; i++) {
    data_buf[i]=readserial1();
  }
  delim=readserial1();
  //printf("right delim %c\n",delim);
  if (delim!='>') {
      strcpy(cmd,"error");
      flush();
      send("error","b",'!');
      return unpack("b","!"); 
  }
  return unpack(format,data_buf);
}


 
