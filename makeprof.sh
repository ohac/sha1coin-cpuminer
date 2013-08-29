#!/bin/sh 
rm quarkp quarkp.o quark.o 
echo build 
CC=gcc
CFLAGS="-g -O1 -msse2 -msse4 "
$CC $CFLAGS -c -o quarkp.o quarkp.c
#$CC $CFLAGS -DSPEEDRUN -DPROFILERUN -c -o quark.o quark.c
$CC $CFLAGS -DPROFILERUN -c -o quark.o quark.c
#$CC $CFLAGS -c -o grostl_vperm.o grostl_vperm.c
#$CC $CFLAGS -c -o grox.o grox.c
#$CC $CFLAGS -c -o vperm.o vperm.c
#$CC $CFLAGS -c -o quarkp quarkp.o quark.o grostl_vperm.o vperm.o
#$CC -O2 -msse2 -c -o grso.o grso.c
$CC -O1 -msse4 -c -o grso-asm2.o grso-asm2.c
#$CC -O2 -msse4 -c -o grso-asm.o grso-asm2.c
$CC -O1 -msse2 -c -o grso.o grso.c
#$CC -O2 -msse4 -c -o grss.o grss.c
#$CC -O2 -msse4 -c -o bitsliceaes.o bitsliceaes.c
#$CC -O2 -msse2 -c -o grsv.o grsv.c
#$CC -O2 -msse2 -c -o grsn.o grsn.c
#$CC $CFLAGS -c -o grsi.o grsi.c
#$CC $CFLAGS -c -o blake.o blake.c
#$CC $CFLAGS -c -o bmw.o bmw.c
#$CC $CFLAGS -c -o groestl.o groestl.c
#$CC $CFLAGS -c -o jh.o jh.c
#$CC $CFLAGS -c -o keccak.o keccak.c
#$CC $CFLAGS -c -o skein.o skein.c
#$CC $CFLAGS -c -o skein_mm.o skein_mm.c
#$CC $LDFLAGS -g -pg -o quarkp quarkp.o quark.o blake.o bmw.o jh.o keccak.o skein.o groestl.o
$CC $LDFLAGS -g -o quarkp quarkp.o quark.o grso-asm2.o 
#$CC $LDFLAGS -g -pg -o quarkp quarkp.o quark.o  grsv.o
#$CC $LDFLAGS -g -pg -o quarkp quarkp.o quark.o grsn.o
#$CC $LDFLAGS -g -pg -o quarkp quarkp.o quark.o 
#$CC $LDFLAGS -g -pg -o quarkp quarkp.o quark.o grss.o bitsliceaes.o
#$CC $LDFLAGS -o quarkp quarkp.o quark.o skein_mm.o grox.o
#$CC $LDFLAGS -o quarkp quarkp.o quark.o grox.o
echo run
echo good
echo a3c382a5327044ee96bfd104881e0e84a775dfe67f3ef63073bc96b14ee7aed2000000000000000000000000
echo cheat
echo 25e189764ac6fdd0cc851a85bb39ed2797629d3434f7f7d3f2fe08a69d6ab392000000000000000000000000
echo test
time ./quarkp

echo speed
echo 28df2b1c3a209c9cb55d559f7ca621fa2d778bec91bedf35b080f9bab5fdf7000000000000000000000000
echo cheat
echo d66205dd833b29b02dc7230e5fa52ee5a2c195f5c3916ef03c31fdd5e0a1c66c000000000000000000000000
echo profile
gprof quarkp > a.txt

