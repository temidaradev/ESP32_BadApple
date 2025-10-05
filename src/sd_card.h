#ifndef SD_CARD_H
#define SD_CARD_H

#include "SD.h"
#include "SPI.h"
#include <Arduino.h>

class SDCard {
public:
  SDCard();
  bool begin(uint8_t cs_pin, uint8_t mosi_pin, uint8_t miso_pin,
             uint8_t sck_pin);
  File openFile(const char *filename);
  void printCardInfo();
  void listFiles(const char *dirname = "/");

private:
  uint8_t cs_pin;
  uint8_t mosi_pin;
  uint8_t miso_pin;
  uint8_t sck_pin;
};

#endif
