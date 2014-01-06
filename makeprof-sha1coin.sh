#!/bin/sh 
rm -f sha1coinp sha1coinp.o sha1coin.o 
echo build 
CC="gcc -std=gnu99"
CFLAGS="-g -p -pg -O3 -msse2 -msse4 "
LDFLAGS="-p -pg"
INCLUDES="-I./compat/jansson"
$CC $CFLAGS $INCLUDES -c -o sha1coinp.o sha1coinp.c
$CC $CFLAGS $INCLUDES -DSPEEDRUN -DPROFILERUN -c -o sha1coin.o sha1coin.c
$CC $LDFLAGS -g -o sha1coinp sha1coinp.o sha1coin.o -lcrypto

echo expect
echo 2c1ddd57401f64506372dfd3713589894529c62e000000000000000000000000000
echo 2878637d3e3393e8e9505a5c95cfb25228455a68000000000000000000000000000

echo actual
time ./sha1coinp 2> b.txt

echo speed
cat b.txt

echo profile
gprof -l sha1coinp > a.txt
cat /proc/cpuinfo >> a.txt
