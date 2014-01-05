#include "cpuminer-config.h"

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "miner.h"

struct work_restart *work_restart;

struct work_restart workr;

void sha1coinhash(void *state, const void *input);

bool fulltest(const uint32_t *hash, const uint32_t *target)
{
    return false;
}

int main()
{
    int i;
    int in[1024];
    int out[1024];

    work_restart = &workr;

    for (i = 0; i < 1024; i++) {
        in[i] = i;
    }

    sha1coinhash(out, in);
    sha1coinhash(out, in);

    for (i = 0; i < 32; i++) {
        printf("%x", out[i]);
    }
    printf("\n");

    for (i = 0; i < 160000; i++) {
        sha1coinhash(out,in);
        sha1coinhash(in,out);
    }
    for (i = 0; i < 32; i++) {
        printf("%x", out[i]);
    }
    printf("\n");

    return 0;
}
