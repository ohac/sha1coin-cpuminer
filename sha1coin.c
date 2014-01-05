#include "cpuminer-config.h"
#include "miner.h"

#include <string.h>
#include <stdint.h>
#include <openssl/sha.h>

inline void encodeb64(const unsigned char* pch, char* buff)
{
  const char *pbase64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  int mode = 0, left = 0;
  const int len = 20;
  const unsigned char *pchEnd = pch + len;
  while (pch < pchEnd) {
    int enc = *(pch++);
    if (mode == 0) {
      *buff++ = pbase64[enc >> 2];
      left = (enc & 3) << 4;
      mode = 1;
    }
    else if (mode == 1) {
      *buff++ = pbase64[left | (enc >> 4)];
      left = (enc & 15) << 2;
      mode = 2;
    }
    else {
      *buff++ = pbase64[left | (enc >> 6)];
      *buff++ = pbase64[enc & 63];
      mode = 0;
    }
  }
  *buff = pbase64[left];
//*(buff + 1) = 0;
}

#ifndef PROFILERUN
inline
#endif
void sha1coinhash(void *state, const void *input)
{
  char str[38]; // 26 + 11 + 1
  uint32_t prehash[5] __attribute__((aligned(32)));
  uint32_t hash[5] __attribute__((aligned(32))) = { 0 };
  SHA1(input, 20 * 4, prehash);
  encodeb64(prehash, str);
  memcpy(&str[26], str, 11);
//str[37] = 0;
  for (int i = 0; i < 26; i++) {
    SHA1((unsigned char*)&str[i], 12, prehash);
#define CHEAT
#if !defined(CHEAT)
    hash[0] ^= prehash[0];
    hash[1] ^= prehash[1];
    hash[2] ^= prehash[2];
    hash[3] ^= prehash[3];
#endif
    hash[4] ^= prehash[4];
  }
#if !defined(CHEAT)
  memcpy(state, hash, 20);
#else
  memcpy((char *)state + 16, &hash[4], 4);
#endif
}

int scanhash_sha1coin(int thr_id, uint32_t *pdata, const uint32_t *ptarget,
	uint32_t max_nonce, unsigned long *hashes_done)
{
	const uint32_t first_nonce = pdata[19];
	uint32_t n = first_nonce - 1;
	uint32_t endiandata[32];
  uint32_t hash[8] __attribute__((aligned(32)));
  hash[0] = 0;
  hash[1] = 0;
  hash[2] = 0;
  for (int kk = 0; kk < 32; kk++) {
    be32enc(&endiandata[kk], ((uint32_t*)pdata)[kk]);
  }
  do {
    pdata[19] = ++n;
		be32enc(&endiandata[19], n); 
    sha1coinhash(&hash[3], endiandata);
    if (!(hash[7] & 0xfffffc00) && fulltest(hash, ptarget)) {
      *hashes_done = n - first_nonce + 1;
      return 1;
    }
  } while (n < max_nonce && !work_restart[thr_id].restart);
	*hashes_done = n - first_nonce + 1;
	pdata[19] = n;
	return 0;
}
