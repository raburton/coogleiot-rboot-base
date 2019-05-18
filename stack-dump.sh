#!/bin/sh
java -jar EspStackTraceDecoder.jar ~/esp8266/tools/xtensa-lx106-elf/bin/xtensa-lx106-elf-addr2line ./build/generic/app.elf $1
