must be compiled with 

CFLAGS="-msse2"

recomended is 

CFLAGS="-msse2 -O3"

jh_sse2_opt64.h is for 64bit only, sse2 enabled proccessors.
groestl is optimized assembly for Opteron or newer AMD chips. No test have
been done on Intel.
