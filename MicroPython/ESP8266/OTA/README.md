# Over The Air (OTA) flashing of firmware 

The micropython firmware of the ESP8266 baord, can be updated by using the EV3 hub, or by using an external FTDI board on a PC. Here we describe a method for upgrading the firmware using the Wifi connection.

## Using YAOTA8266 to flash the ESP8266
Once the bootloader is installed on the ESP8266, boot the ESP in OTA mode by pressing the BOOT button within 3 seconds from reset. The output of the ESP8266 can be followied on the SPIKE using the following code:

```python
from projects.uartremote import *
u=UartRemote("E")
while True:
    q=u.uart.read(100)
    if q:
	try:
	    print(q.decode('utf-8'),end='')
	except:
	    pass
```



The following command will upload the new firmware:

```
./ota_client.py -a <IP-address> live-ota  micropython_v1.15_uartremote-ota.bin
``` 
The ota versions of the firmware with and without precompiled uartremote library  can be found in the `yaota8266_bin` directory.

The micropython firmware needs to be build for OTA application, where the ROM segment is shifted by 0x3c000 to allow extra space for the first and second stage bootloader.

The OTA server contains a public RSA key to verify the signature of all data packets. The private key in the `OTA_client` directory is used to sign each data packet before sensding it to the OTA server. These two keys should form a pair. The keys as provided in this repository can be used adn there is no need to rebuild te OTA server. When you generate your own keypair, you need replace the `modulus` in the `config.h` file and recompile the OTA server.

### Enable webrepl

Paste the following program in a new project on the SPIKE and execute.

```python
from projects.uartremote import *
u=UartRemote("E")
u.repl_activate()
u.repl_run('from webrepl_setup import *')
u.repl_run('change_daemon(True)')
u.repl_run('pw="PASS = \'python\'\\n"')
u.repl_run('open("webrepl_cfg.py","w").write(pw)')
u.repl_run('import machine\nmachine.reset()')
```

## First time flashing OTA bootloader
Before you can use the OTA firmware update, you need to flash the bootloader.

Use the following `esptool.py` commands:

```
esptool.py --port your-port erase_flash
esptool.py --port your-port --baud 230400 write_flash -fm dio --flash_size=detect 0x0 <projects-dir>/yaota8266_bin/yaota8266.bin
esptool.py --port your-port --baud 230400 write_flash -fm dio --flash_size=detect 0x3c000 <projects-dir>/yaota8266_bin//micropython_v1.15-ota.bin
```

## YAOTA8266

[This github project](https://github.com/pfalcon/yaota8266) is a bootloader for the ESP8266 that allows Over The Air update of the firmware. It consists of a first stage bootloader `boot8266` that switches between the normal application (micropython in this case) and the OTA server. Normally this is triggered by a chaning GPIO port.

A number of patches are suggested [on this blog](https://schinckel.net/2018/05/26/ota-firmware-updates-with-micropython-esp8266/). In the source code in the `yaota8266_src` directory, these patches are already made.

[This github projects](https://github.com/nenadfilipovic/esp8266-micropython-ota) gives a good summary of how to use yaota8266.

## Extensions
To trigger the OTA loader, the bootloader checks for GPIO pins changing state during the initilisation phase. [An extension to this](https://github.com/ulno/yaota8266/commit/566a292bb2269747c3475b835d3a84ebc0c3061f), is that it checks the RTC register for a magic value ```"yaotaota"```. This value can be set from within the MicroPyhton environment:

```python
import machine

def start():
    machine.RTC().memory('yaotaota')
    machine.reset()
```

and can be triggered by:

```python
import ota
ota.start()
```

## Building yaota8266

[This github projects](https://github.com/nenadfilipovic/esp8266-micropython-ota) gives a good summary of how to build yaota8266.

## Compiling micropython for OTA


Make the following change in `micropython/ports/esp8266/boards/esp8266_ota.ld`:

```
//* GNU linker script for ESP8266 */

MEMORY
{
	dport0_0_seg : org = 0x3ff00000, len = 0x10
	dram0_0_seg :  org = 0x3ffe8000, len = 0x14000
	iram1_0_seg :  org = 0x40100000, len = 0x8000
	/* 0x3c000 is size of bootloader, 0x9000 is size of packed RAM segments */
	irom0_0_seg :  org = 0x40200000 + 0x3c000 + 0x9000, len = 1M - 100k
}

```

Now you can compile the OTA version of micropyhton by using (you can copy the `uartremote.py` library in the `/modules` directory before compiling to integrate the library as a frozen module):

```
make ota
```
