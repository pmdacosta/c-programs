#include <stdio.h>

#define MAX_PERSONS 1024

int main() {
#ifdef DEBUG
    printf("WE ARE IN DEBUG MODE: %s:%d\n", __FILE__, __LINE__);
#endif
    return 0;
}
