/************************ Adafruit IO Config *******************************/

// visit io.adafruit.com if you need to create an account,
// or if you need your Adafruit IO key.
#define IO_USERNAME  "luispedromc"
#define IO_KEY       "aio_uuNL16FF1PvUJCOY5LZqK5aB1snE"

/******************************* WIFI **************************************/
#define WIFI_SSID "esp32"
#define WIFI_PASS "contra123"
#include "AdafruitIO_WiFi.h"
AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);


