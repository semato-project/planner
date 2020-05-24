#include "pvm3.h"
#include <stdlib.h>


int main()
{
        int mytid;
    int tids[4];/*tablica z identyfikatorami procesów slave*/
    int nproc;
        nproc=pvm_spawn("slave", NULL, PvmTaskDefault, \
                                "", 8, tids);
        pvm_exit();
}