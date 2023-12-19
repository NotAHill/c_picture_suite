#!/bin/sh

input="./images/bmp/test.bmp"
output="./images/bmp/out.bmp"

make clean
make
./main $input $output
