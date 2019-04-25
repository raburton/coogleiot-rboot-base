FLASH_DEF = 4M1M
BOARD = generic
UPLOAD_SPEED = 115200
ESP_ROOT = $(HOME)/esp8266

ESPTOOL = $(ESP_ROOT)/tools/esptool/esptool
BOOT_LOADER = ../rboot/firmware/rboot.bin

include makeEspArduino.mk
