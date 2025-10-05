#include "sd_card.h"

SDCard::SDCard() : cs_pin(0), mosi_pin(0), miso_pin(0), sck_pin(0) {}

bool SDCard::begin(uint8_t cs_pin, uint8_t mosi_pin, uint8_t miso_pin,
                   uint8_t sck_pin) {
  this->cs_pin = cs_pin;
  this->mosi_pin = mosi_pin;
  this->miso_pin = miso_pin;
  this->sck_pin = sck_pin;

  Serial.println("[SD] Initializing SD card...");
  Serial.printf("[SD] Pins - CS:%d, MOSI:%d, MISO:%d, SCK:%d\n", cs_pin,
                mosi_pin, miso_pin, sck_pin);

  SPI.begin(sck_pin, miso_pin, mosi_pin, cs_pin);
  delay(10);

  int retries = 3;
  bool success = false;

  for (int i = 0; i < retries && !success; i++) {
    if (i > 0) {
      Serial.printf("[SD] Card init attempt %d/%d...\n", i + 1, retries);
      delay(100);
    }

    success = SD.begin(cs_pin, SPI, 4000000);

    if (!success) {
      Serial.printf("[SD] Attempt %d failed\n", i + 1);
    }
  }

  if (!success) {
    Serial.println("[SD] ✗ SD Card initialization failed!");
    Serial.println("[SD] Troubleshooting tips:");
    Serial.println("[SD]   - Check SD card is inserted correctly");
    Serial.println("[SD]   - Verify wiring connections");
    Serial.println("[SD]   - Try a different SD card (Class 10 recommended)");
    Serial.println("[SD]   - Format card as FAT32");
    return false;
  }

  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("[SD] ✗ No SD card attached!");
    return false;
  }

  printCardInfo();
  Serial.println("[SD] ✓ SD Card initialized successfully!");
  return true;
}

void SDCard::printCardInfo() {
  uint8_t cardType = SD.cardType();
  Serial.print("[SD] ✓ SD Card Type: ");
  switch (cardType) {
  case CARD_MMC:
    Serial.println("MMC");
    break;
  case CARD_SD:
    Serial.println("SDSC");
    break;
  case CARD_SDHC:
    Serial.println("SDHC");
    break;
  default:
    Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("[SD] ✓ SD Card Size: %lluMB\n", cardSize);
  Serial.printf("[SD] ✓ Total space: %lluMB\n",
                SD.totalBytes() / (1024 * 1024));
  Serial.printf("[SD] ✓ Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
}

File SDCard::openFile(const char *filename) {
  File file = SD.open(filename);
  if (!file) {
    Serial.printf("[SD] ✗ Failed to open file: %s\n", filename);
    return File();
  }

  Serial.printf("[SD] ✓ Opened file: %s (size: %u bytes)\n", filename,
                file.size());
  return file;
}

void SDCard::listFiles(const char *dirname) {
  Serial.printf("[SD] Listing directory: %s\n", dirname);
  File root = SD.open(dirname);
  if (!root) {
    Serial.println("[SD] ✗ Failed to open directory");
    return;
  }

  if (!root.isDirectory()) {
    Serial.println("[SD] ✗ Not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.printf("[SD]   DIR: %s\n", file.name());
    } else {
      Serial.printf("[SD]   FILE: %s (%u bytes)\n", file.name(), file.size());
    }
    file = root.openNextFile();
  }
  root.close();
}
