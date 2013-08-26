#include "cpuminer-config.h"
#include "miner.h"

//#define QUARK_AES 1
//#define QUARK_VPERM 1
//#define QUARK_ITR 1

#include <string.h>
#include <stdint.h>

#include "blake.c"
#include "bmw.c"
#include "keccak.c"
#include "skein.c"
#include "jh_sse2_opt64.h"
//#include "groestl.c"

#if 1
#include "grso.c"
#ifndef PROFILERUN
#include "grso-asm.c"
#endif
#endif

/*define data alignment for different C compilers*/
#if defined(__GNUC__)
      #define DATA_ALIGN16(x) x __attribute__ ((aligned(16)))
#else
      #define DATA_ALIGN16(x) __declspec(align(16)) x
#endif

inline void quarkhash(void *state, const void *input)
{
    sph_keccak512_context    ctx_keccak;
    //sph_groestl512_context ctx_grs;
    grsoState sts_grs;

    //DECL_GRS;
    //DECL_KEC;

    int i;

    DATA_ALIGN16(uint32_t hash[32]);
	
    for(i=0; i<9; i++)
  {
    /* blake is split between 64bit hashes and the 80bit initial block */
    DECL_BLK;
    switch (i+(16*((hash[0] & (uint32_t)(8)) == (uint32_t)(0))))
    {
        case 0:
        case 16: 
            BLK_I;
            BLK_W;
            break;
        case 5 :
            BLK_I;
            BLK_U;
            break;
        case 1:
        case 17:
        case 21: do { 
            DECL_BMW;
            BMW_I;
            BMW_U;
/* bmw compress uses some defines */
#define M(x)    sph_dec64le_aligned(data + 8 * (x))
#define H(x)    (h[x])
#define dH(x)   (dh[x])
            BMW_C;
#undef M
#undef H
#undef dH
            } while(0); continue;;
        case 2:
#ifdef CHEAT
                 /* blake and bmw are almost free
                  * if luck means that groestl will now be run twice
                  * give up and try anoter nonce */
                 return;
#endif
        case 19:
        case 3: do {
            //GRSAI;
            //GRSAU;
            //GRSAC;
            //sph_groestl512_init(&ctx_grs);
            //sph_groestl512(&ctx_grs, hash, 64);
            //sph_groestl512_close(&ctx_grs, hash);
            GRS_I;
            GRS_U;
            GRS_C;
            //grsoInit(&sts_grs);
            //grsoUpdateq(&sts_grs, (char*)hash);
            //grsoFinal(&sts_grs, (char*)hash);
            } while(0); continue;
        case 4:
        case 20:
        case 24: do {
            DECL_JH;
            JH_H;
            } while(0); continue;
        case 6:
        case 22:
        case 8: do {
            DECL_KEC;
            KEC_I;
            KEC_U;
            KEC_C;
            //sph_keccak512_init(&ctx_keccak);
            //sph_keccak512q(&ctx_keccak,hash); 
            //sph_keccak512_close(&ctx_keccak, hash);
            } while(0); continue;
        case 18:
        case 7:
        case 23: do {
            DECL_SKN;
            SKN_I;
            SKN_U;
            SKN_C;
            } while(0); continue;
        default:
            abort();
    }
    /* only blake shouuld get here without continue */
    /* blake finishs from top split */
    BLK_C;
 }
    memcpy(state, hash, 32);
}

int scanhash_quark(int thr_id, uint32_t *pdata, const uint32_t *ptarget,
	uint32_t max_nonce, unsigned long *hashes_done)
{
	uint32_t n = pdata[19] - 1;
	const uint32_t first_nonce = pdata[19];
	const uint32_t Htarg = ptarget[7];

	uint32_t hash64[8] __attribute__((aligned(32)));
	uint32_t endiandata[32];
	
	//we need bigendian data...
	//lessons learned: do NOT endianchange directly in pdata, this will all proof-of-works be considered as stale from minerd.... 
	int kk=0;
	for (; kk < 32; kk++)
	{
		be32enc(&endiandata[kk], ((uint32_t*)pdata)[kk]);
	};

//	if (opt_debug) 
//	{
//		applog(LOG_DEBUG, "Thr: %02d, firstN: %08x, maxN: %08x, ToDo: %d", thr_id, first_nonce, max_nonce, max_nonce-first_nonce);
//	}
	
	
	
	
	do {
	
		pdata[19] = ++n;
		be32enc(&endiandata[19], n); 
		quarkhash(hash64, &endiandata);
        if (((hash64[7]&0xFFFFFF00)==0) && 
				fulltest(hash64, ptarget)) {
            *hashes_done = n - first_nonce + 1;
			return true;
		}
	} while (n < max_nonce && !work_restart[thr_id].restart);
	
	*hashes_done = n - first_nonce + 1;
	pdata[19] = n;
	return 0;
}
