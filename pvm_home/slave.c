#include "pvm3.h"
#include <stdio.h>
#include <unistd.h>

int main()
{
    sleep(5);
    printf("Hello world!\n");
    pvm_exit();
}