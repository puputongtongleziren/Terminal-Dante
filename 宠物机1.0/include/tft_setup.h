#pragma once

#define USER_SETUP_LOADED
#define ST7789_DRIVER
#define TFT_WIDTH 240
#define TFT_HEIGHT 320
#define TFT_RGB_ORDER TFT_BGR

#define TFT_MISO -1
#define TFT_MOSI 11
#define TFT_SCLK 12
#define TFT_CS 10
#define TFT_DC 13
#define TFT_RST 14

#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_GFXFF

#define SPI_FREQUENCY 27000000
#define SPI_READ_FREQUENCY 16000000
