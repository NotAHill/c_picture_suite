#!/bin/sh

make clean
make
./main
./main ./images/bmp/out.bmp
