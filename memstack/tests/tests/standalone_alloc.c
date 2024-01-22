#include <memstack.h>
#include <stdio.h>

int main(){

    printf("Creating memstack!\n");
    memstack* ms = msnew();

    int* ptr = (int*)msalloc(ms, sizeof(int));
    printf("ptr: %d\n", *ptr);
    printf("Setting ptr to 60\n");
    *ptr = 60;
    printf("ptr: %d\n", *ptr);

    msfree(ms);

    printf("Done!");

    return 0;
}