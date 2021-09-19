# UartRemote library for Arduino

This library is fully compatible with the MicroPython UartRemote library. It can be used in any Arduino project by adding this whole directory to the Arduino ibrary directory. After importing the `UartRemote` library, an example can be selected form the examples in the Arduino IDE.

## Differences compared to MicroPython implementation

Because C++ lacks the possibility to generate a function call with a variable number of parameters, a conversion function `getvariables` was introduced. 

A typical definition of a user defined function that will be called upon receiving a command with its accompanying parameters is hsown below:

```python
void led(unpackresult& args) {
   int a,b,c;  float d,e,f,g;
   uartremote.getvariables(args,&a,&b,&c,&d,&e,&f,&g);

   printf("%d %d %d %f\n",a,b,c,d);
   uartremote.send("ledack","b",0);
}
```

Here, the user function takes always the parameters as `unpackresult&`. The function `getvariables` assigns the parsed values to the respective variables. In the remainder of the fucntion definition, these assigned variables can be used.

The user defined function must always return an acknowledgement. In this case a summy variable of type `byte` is returned.

The following example shows how to return one or more values back.

```python
void tst(unpackresult& args) {
   float a,b;
   uartremote.getvariables(args,&a,&b);
   
   float c=a+b;
 
   uartremote.send("tstack","f",c);
}
```

## Speed

Because this library is written in pure C++, it is faster by a factor of approximately 100 compared to the MicroPython implementation.

