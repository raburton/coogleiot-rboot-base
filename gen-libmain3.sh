#!/bin/bash
echo "mkdir -p $2/libmain"
mkdir -p $2/libmain
echo "cd $2/libmain"
cd $2/libmain
echo "ar x $1/tools/sdk/lib/libmain.a"
ar x $1/tools/sdk/lib/libmain.a
echo "$1/tools/xtensa-lx106-elf/bin/xtensa-lx106-elf-objcopy --weaken-symbol Cache_Read_Enable_New $2/libmain/app_main.o $2/libmain/app_main2.o"
$1/tools/xtensa-lx106-elf/bin/xtensa-lx106-elf-objcopy --weaken-symbol Cache_Read_Enable_New $2/libmain/app_main.o $2/libmain/app_main2.o
echo "rm $2/libmain/app_main.o"
rm $2/libmain/app_main.o
echo "mv $2/rboot-bigflash.c.o $2/libmain/"
mv $2/rboot-bigflash.c.o $2/libmain/
echo "cd $2"
cd $2
echo "ar cr $1/tools/sdk/lib/libmain3.a $2/libmain/*.o"
ar cr $2/libmain3.a $2/libmain/*.o
echo "rm -r $2/libmain"
rm -r $2/libmain
