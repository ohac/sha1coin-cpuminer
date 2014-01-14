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

uint32_t decodeb64chunk(const char* str)
{
  unsigned int dec = 0;
  const char *pbase64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  for (int i = 0; i < 4; i++)
  {
    char* pos = strchr(pbase64, str[i]);
    int v = (int)(pos - pbase64);
    dec = (dec << 6) | v;
  }
  dec <<= 8;
  uint32_t result = 0;
  be32enc(&result, dec); 
  return result;
}

inline void encodeb64chunk(const unsigned char* pch, char* buff)
{
  const char *pbase64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  int mode = 0, left = 0;
  const int len = 3;
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
}

static unsigned short b64tbl1[0x10000];
static unsigned short b64tbl2[0x10000];
uint32_t searchchunks[128];
int nsearchchunks = 0;
char *findtrips[128];

void genb64tbl()
{
  unsigned char in[4] = "";
  unsigned char out[8] = "";
  for (int i = 0; i < 0x10000; i++) {
    in[0] = i & 0xff;
    in[1] = i >> 8;
    in[2] = 0;
    encodeb64chunk(in, out);
    b64tbl1[i] = out[0] | (out[1] << 8);
    in[0] = 0;
    in[1] = i & 0xff;
    in[2] = i >> 8;
    encodeb64chunk(in, out);
    b64tbl2[i] = out[2] | (out[3] << 8);
  }
  char *strbase = malloc(strlen(opt_findtrip) + 1);
  char *str = strbase;
  char *str2;
  char *saveptr = NULL;
  strcpy(str, opt_findtrip);
  for (int i = 0; str2 = strtok_r((char *)str, ",", &saveptr); i++) {
    findtrips[i] = str2;
    searchchunks[i] = decodeb64chunk(str2) & 0x00ffffff;
    nsearchchunks = i + 1;
    str = NULL;
  }
  // don't free strbase.
}

void encodeb64wide(const unsigned char* pch, unsigned short* buff)
{
  unsigned short sv;
  for (int i = 0; i < 7; i++) {
    sv = pch[0] | (pch[1] << 8);
    *buff++ = b64tbl1[sv];
    sv = pch[1] | (pch[2] << 8);
    *buff++ = b64tbl2[sv];
    pch += 3;
  }
}

void tbltest()
{
  unsigned char in[21] = "12345678901234567890";
  char out[30] = "";
  genb64tbl();
  encodeb64(in, out);
  printf("expected: %s\n", out);
  encodeb64wide(in, (unsigned short *)out);
  printf("actual  : %s\n", out);
}

uint32_t sha1coinhash(void *state, const void *input)
{
  char str[38] __attribute__((aligned(32))); // 26 + 11 + 1
  char trip[28] __attribute__((aligned(32))); // 26 + 1 + padding
  char tripkey[13] = "";
  uint32_t prehash[5] __attribute__((aligned(32)));
  uint32_t hash[5] __attribute__((aligned(32))) = { 0 };
  uint32_t prehash0;
  uint32_t hash4 = 0;
  SHA1(input, 20 * 4, (void *)prehash);
#if 0
  encodeb64((const unsigned char *)prehash, (unsigned char *)str);
#else
  encodeb64wide((const unsigned char *)prehash, (unsigned short *)str);
#endif
  memcpy(&str[26], str, 11);
//str[37] = 0;
  for (int i = 0; i < 26; i++) {
    SHA1((const unsigned char*)&str[i], 12, (unsigned char *)prehash);
#define TRIP
#if defined(TRIP)
    prehash0 = prehash[0] & 0x00ffffff;
    for (int j = 0; j < nsearchchunks; j++) {
      if (prehash0 == searchchunks[j]) {
        encodeb64wide((const unsigned char *)prehash, (unsigned short *)trip);
        memcpy(tripkey, &str[i], 12);
        trip[12] = 0;
        int triplen = strlen(findtrips[j]);
        int result = !memcmp(trip, findtrips[j], triplen);
        if (result) {
          applog(LOG_INFO, "tripkey: #%s, trip: %s %s", tripkey, trip, "(yay!!!)");
        }
      }
    }
#endif
#define CHEAT
#if !defined(CHEAT)
    hash[0] ^= prehash[0];
    hash[1] ^= prehash[1];
    hash[2] ^= prehash[2];
    hash[3] ^= prehash[3];
#endif
    //hash[4] ^= prehash[4];
    hash4 ^= prehash[4];
  }
#if !defined(CHEAT)
  memcpy(state, hash, 20);
#else
  //memcpy((char *)state + 16, &hash[4], 4);
  //*(uint32_t *)state = hash4;
#endif
  return hash4;
}

int scanhash_sha1coin(int thr_id, uint32_t *pdata, const uint32_t *ptarget,
    uint32_t max_nonce, unsigned long *hashes_done)
{
  const uint32_t first_nonce = pdata[19];
  uint32_t n = first_nonce - 1;
  uint32_t endiandata[32];
  uint32_t hash[8] __attribute__((aligned(32)));
#if defined(CHEAT)
  uint32_t hash7;
#endif
  hash[0] = 0;
  hash[1] = 0;
  hash[2] = 0;
  for (int kk = 0; kk < 32; kk++) {
    be32enc(&endiandata[kk], ((uint32_t*)pdata)[kk]);
  }
  do {
    pdata[19] = ++n;
    be32enc(&endiandata[19], n); 
#if defined(CHEAT)
    hash7 = sha1coinhash(NULL, endiandata);
    if (!(hash7 & 0xfffffc00)) {
      hash[7] = hash7;
      if (fulltest(hash, ptarget)) {
        *hashes_done = n - first_nonce + 1;
        return 1;
      }
    }
#else
    sha1coinhash(&hash[3], endiandata);
    if (!(hash[7] & 0xfffffc00) && fulltest(hash, ptarget)) {
      *hashes_done = n - first_nonce + 1;
      return 1;
    }
#endif
  } while (n < max_nonce && !work_restart[thr_id].restart);
  *hashes_done = n - first_nonce + 1;
  pdata[19] = n;
  return 0;
}
