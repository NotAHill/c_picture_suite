#!/bin/sh

input="./images/bmp/img.bmp"
output="./images/bmp/out.bmp"

make clean
make
./main $input $output
