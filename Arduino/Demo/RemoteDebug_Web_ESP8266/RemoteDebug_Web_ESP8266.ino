
#include <ext/pb_ds/assoc_container.hpp>

#define HOST_NAME "remotedebug"

// Board especific libraries

#define USE_MDNS true
#define USE_ARDUINO_OTA true
#define WEB_SERVER_ENABLED true

#include <ESP8266WiFi.h>

#ifdef USE_MDNS
#include <DNSServer.h>
#include <ESP8266mDNS.h>
#endif

#include <ESP8266WebServer.h>

#ifdef USE_ARDUINO_OTA
#include <ArduinoOTA.h>
#endif

ESP8266WebServer HTTPServer(80);

#include "RemoteDebug.h"        //https://github.com/JoaoLopesF/RemoteDebug
RemoteDebug Debug;


// WiFi credentials
// Note: if commented, is used the smartConfig
// That allow to it in mobile app
// See more details in http://www.iotsharing.com/2017/05/how-to-use-smartconfig-on-esp32.html

//#define WIFI_SSID "...."  // your network SSID (name)
//#define WIFI_PASS "...."  // your network key


#include "UartRemote.h"
char cmd[20]; // global temporary storage for command names
UartRemote uartremote;


void tst(unpackresult& args) {
   float a,b;
   uartremote.getvariables(args,&a,&b);
   
   float c=a+b;
   //Serial.println("sum = "+String(c));
    debugV("* This is a message of debug level VERBOSE");
      debugD("* This is a message of debug level DEBUG");
   debugV("Sum of %f and %f = %f",a,b,c);
    
   uartremote.send("tstack","3f",a,b,c);
}

void measure(unpackresult& args) {
  int n;
  uartremote.getvariables(args,&n);

  //packresult rcvpack;
  unsigned char b[50];
  for (int i=0; i<50; i++) b[i]=i;
  debugV("* Before pack/unpack");
  for (int i=0; i<n; i++) {
    packresult rcvpack=uartremote.pack("a50B",b);
    uartremote.unpack("a50B",rcvpack.data);
  }
  debugV("* after pack/unpack");
  //Serial.println("Ready");
  uartremote.send("measureack","b",0);
}

void tstarr(unpackresult& args) {
   unsigned char r[64];
   uartremote.getvariables(args,r);
   debugV("* tstarr function");
   unsigned char q[64];
   for (int i=0; i<64; i++) {q[i]=i;}

   uartremote.send("tstack","a64B",q);
}


void led(unpackresult& args) {
   int a,b,c; 
   uartremote.getvariables(args,&a,&b,&c);

   debugV("led RGB %d %d %d\n",a,b,c);
   uartremote.send("ledack","b",0);
}
/////// Variables

// Time

uint32_t mTimeToSec = 0;
uint32_t mTimeSeconds = 0;

////// Setup

void setup() {

	// Initialize the Serial (use only in setup codes)

	Serial.begin(230400);
  // debug via TXD1 (there is no RXD1) on Pin GPIO2
  Serial1.begin(115200); 
  
	connectWiFi();

	// Host name of WiFi
  WiFi.hostname(HOST_NAME);
  initializeOTA();

 if (MDNS.begin(HOST_NAME)) {
		Serial1.print("* MDNS responder started. Hostname -> ");
		Serial1.println(HOST_NAME);
	}

	// Register the services

	MDNS.addService("http", "tcp", 80);   // Web server
	MDNS.addService("telnet", "tcp", 23); // Telnet server of RemoteDebug, register as telnet

	// HTTP web server

	HTTPServer.on("/", handleRoot);
	HTTPServer.onNotFound(handleNotFound);
  HTTPServer.begin();
  Serial1.println("* HTTP server started");

	Debug.begin(HOST_NAME); // Initialize the WiFi server

	//Debug.setPassword("r3m0t0."); // Password for WiFi client connection (telnet or webapp)  ?

	Debug.setResetCmdEnabled(true); // Enable the reset command
	Debug.showProfiler(true); // Profiler (Good to measure times, to optimize codes)
	Debug.showColors(true); // Colors
	// Debug.setSerialEnabled(true); // if you wants serial echo - only recommended if ESP is plugged in USB

	// Project commands

	String helpCmd = "bench1 - Benchmark 1\n";
	helpCmd.concat("bench2 - Benchmark 2");

	Debug.setHelpProjectsCmds(helpCmd);
	Debug.setCallBackProjectCmds(&processCmdRemoteDebug);

	// End of setup - show IP


  // define UartRemote commands

  uartremote.add_command("led", &led);
  uartremote.add_command("tst", &tst);
  uartremote.add_command("tstarr",&tstarr);
  uartremote.add_command("measure", &measure);

} // end setup()

void loop() {

	// Time of begin of this loop
 uint32_t timeBeginLoop = millis();

 if (uartremote.available()>0) {  
    debugV("* Uart avaialble!");
    unpackresult rcvunpack = uartremote.receive(cmd);
    debugV("* Command received %s",cmd);
    Serial1.println("Command received: "+String(cmd));
    uartremote.command(cmd,rcvunpack);
  }

	// Each second

	if (millis() >= mTimeToSec) {
  	mTimeToSec = millis() + 1000;
  	mTimeSeconds++;
  	if (mTimeSeconds % 5 == 0) { // Each 5 seconds
	}
	}
	////// Services on Wifi


	ArduinoOTA.handle();
	HTTPServer.handleClient();
	Debug.handle();

	// Give a time for ESP
	yield();

	uint32_t time = (millis() - timeBeginLoop);
	if (time > 100) {
		debugI("* Time elapsed for the loop: %u ms.", time);
	} else if (time > 200) {
		debugW("* Time elapsed for the loop: %u ms.", time);
	}

}


// Process commands from RemoteDebug

void processCmdRemoteDebug() {

	String lastCmd = Debug.getLastCommand();
	if (lastCmd == "bench1") {
		// Benchmark 1 - Printf
		debugA("* Benchmark 1 - one Printf");
		uint32_t timeBegin = millis();
    unsigned char b[50];
    for (int i=0; i<50; i++) b[i]=i;
    debugV("* Start pack/unpack benchmark");
    for (int i=0; i<100000; i++) {
      packresult rcvpack=uartremote.pack("a50B",b);
      uartremote.unpack("a50B",rcvpack.data);
    }
    debugV("* Stop pack/unpack benachmark");
		debugA("* Time elapsed for %u printf: %ld ms.\n", 100000,
					(millis() - timeBegin));

	} else if (lastCmd == "bench2") {

		// Benchmark 2 - Print/println
    uartremote.send("float","3f",millis(),2.2,3.3);
		debugA("* Send UartRemote3 floats ");

		
	}
}


////// WiFi

void connectWiFi() {

	////// Connect WiFi

#ifdef EM_DEPURACAO
	Serial1.println("*** connectWiFi: begin conection ...");
#endif

	// Connect with SSID and password stored

#ifndef WIFI_SSID
	WiFi.begin();
#else
	WiFi.begin(WIFI_SSID, WIFI_PASS);
#endif

	// Wait connection

	uint32_t timeout = millis() + 20000; // Time out
	while (WiFi.status() != WL_CONNECTED && millis() < timeout) {
		delay(250);
		Serial1.print(".");
	}

	// Not connected yet?

	if (WiFi.status() != WL_CONNECTED) {

#ifndef WIFI_SSID
		// SmartConfig

		WiFi.beginSmartConfig();

		// Wait for SmartConfig packet from mobile

		Serial1.println("connectWiFi: Waiting for SmartConfig.");

		while (!WiFi.smartConfigDone()) {
			delay(500);
			Serial1.print(".");
		}

		Serial1.println("");
		Serial1.println("connectWiFi: SmartConfig received.");

		// Wait for WiFi to connect to AP

		Serial1.println("connectWiFi: Waiting for WiFi");

		while (WiFi.status() != WL_CONNECTED) {
			delay(500);
			Serial1.print(".");
		}
#else
		Serial1.println("Not possible connect to WiFi, rebooting");
		ESP.restart();
#endif
	}

	// End

	Serial1.println("");
	Serial1.print("connectWiFi: connect a ");
	Serial1.println(WiFi.SSID());
	Serial1.print("IP: ");
	Serial1.println(WiFi.localIP().toString());

}

void initializeOTA() {

	ArduinoOTA.onStart([]() {
		Serial1.println("* OTA: Start");
	});
	ArduinoOTA.onEnd([]() {
		Serial1.println("\n*OTA: End");
	});
	ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
		Serial1.printf("*OTA: Progress: %u%%\r", (progress / (total / 100)));
	});
	ArduinoOTA.onError([](ota_error_t error) {
		Serial1.printf("*OTA: Error[%u]: ", error);
		if (error == OTA_AUTH_ERROR) Serial1.println("Auth Failed");
		else if (error == OTA_BEGIN_ERROR) Serial1.println("Begin Failed");
		else if (error == OTA_CONNECT_ERROR) Serial1.println("Connect Failed");
		else if (error == OTA_RECEIVE_ERROR) Serial1.println("Receive Failed");
		else if (error == OTA_END_ERROR) Serial1.println("End Failed");
	});

	ArduinoOTA.begin();
}

/////////// Handles

 void handleRoot() {

     // Root web page

     HTTPServer.send(200, "text/plain", "hello from esp - RemoteDebug Sample!");
 }

 void handleNotFound(){

     // Page not Found

     String message = "File Not Found\n\n";
     message.concat("URI: ");
     message.concat(HTTPServer.uri());
     message.concat("\nMethod: ");
     message.concat((HTTPServer.method() == HTTP_GET)?"GET":"POST");
     message.concat("\nArguments: ");
     message.concat(HTTPServer.args());
     message.concat("\n");
     for (uint8_t i=0; i<HTTPServer.args(); i++){
         message.concat(" " + HTTPServer.argName(i) + ": " + HTTPServer.arg(i) + "\n");
     }
     HTTPServer.send(404, "text/plain", message);
 }
