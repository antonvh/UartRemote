// This is user-specific config file for yaota8266 OTA bootloader.
// For compilation to succeed, this file should be copied to "config.h"
// and any settings related to RSA keys replaced with your values. Do
// not use values in this example as is - it is a security issue. You
// won't be able to perform any OTA update (because you don't have a
// private key for the public key specified here), but I will own
// your system (because I have it).

// Offset of the main application (one which will undergo OTA update)
// Default start of OTA region == size of boot8266 + ota-server, aligned
// (size of yaota8266.bin as produced by the top-level Makefile).
#define MAIN_APP_OFFSET 0x3c000

// Baud rate for serial output. Set to 0 to not set baud rate explicitly,
// which them will be the default 74880.
#define BAUD_RATE 115200

// Modulus of RSA public key used to verify OTA signature
// (size is 512 bits, exponent is hardcoded at 3).
#define MODULUS "\xfb\x6b\xdd\x1d\xc8\x3f\xf9\x21\xa2\x10\x60\xe4\x0e\xe4\x31\x58\x52\x31\x37\x6e\x82\xea\xd0\xee\x9d\x47\x9f\x7d\x4f\x46\x97\x84\x82\x1e\x8e\x69\xbe\x7c\xf2\xa7\x67\xa0\xd9\x4b\x33\xfe\x88\x92\x77\x06\x32\x31\x19\x1d\x74\x2d\x7e\xf1\x60\x15\xa9\xed\xdd\xf3"



// How long to wait for next expected packet before assuming that
// transfer is broken and restart reception from beginning.
#define PKT_WAIT_MS 10000

// If first OTA packet isn't received during this time, reboot. This
// protects against being stuck in OTA mode if entered by accident.
#define IDLE_REBOOT_MS 60000

// Parameters below can be configured in flash too

// GPIO mask. Default value filters out UART0 and QSPI for FlashROM.
// If value is 0, then OTA is entered unconditionally. By adjusting
// IDLE_REBOOT_MS, you can make a device spend minimal time in this
// unconditional OTA mode.
#define GPIO_MASK 0xf035

// How long to wait for GPIO change to go into OTA mode.
#define GPIO_WAIT_MS 3000
