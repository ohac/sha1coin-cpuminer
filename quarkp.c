#include "cpuminer-config.h"
#include "miner.h"

#include <string.h>
#include <stdint.h>
#include "miner.h"

//#include "grsi-asm.h"


struct work_restart *work_restart;

struct work_restart workr;

bool fulltest(const uint32_t *hash, const uint32_t *target)
{
    return;
}

int main ()
{
    int i;
    int in[1024];
    int out[1024];

//    grsiSET_CONSTANTS();

    /* not really used */
    work_restart = &workr;

    /* not so random numer */
    for (i=0;i<1024;i++)
    {
        in[i] = i;
    }

    /* check if typto trashing constants that are not constant */
    quarkhash(out,in);
    quarkhash(out,in);

    /* show simple test */
    for (i=0;i<32;i++)
    {
        printf("%x", out[i]);
    }
    printf("\n");

    /* burn though many round of quarkhash */
    for (i=0;i<40000;i++)
    {
        quarkhash(out,in);
        quarkhash(in,out);
    }

    /* not really a usefull answer but but should never change */
    for (i=0;i<32;i++){
        printf("%x", out[i]);
    }
    printf("\n");

    return 0;
}
