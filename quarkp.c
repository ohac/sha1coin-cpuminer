#include "cpuminer-config.h"
#include "miner.h"

#include <string.h>
#include <stdint.h>
#include "miner.h"

struct work_restart *work_restart;

struct work_restart workr;

bool fulltest(const uint32_t *hash, const uint32_t *target)
{
    return true;
}

int main ()
{
    int i;
    int in[1024];
    int out[1024];

    work_restart = &workr;

    for (i=0;i<1024;i++)
    {
        in[i] = i;
    }

    quarkhash(out,in);

    for (i=0;i<32;i++)
    {
        printf("%x", out[i]);
    }
    printf("\n");

    for (i=0;i<40000;i++)
    {
        quarkhash(out,in);
        quarkhash(in,out);
    }

    for (i=0;i<32;i++){
        printf("%x", out[i]);
    }
    printf("\n");
    return 0;
}
