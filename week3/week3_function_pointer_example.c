#include <stdio.h>
#include <stdlib.h>


typedef void (*cb_t)(int);

void long_library_function(cb_t progress)
{
    for(int i = 0; i <= 100; i += 10)
    {
        // do something very complex ....
        // and then afterwards update user
        progress(i);
    }
}

void progress_callback(int percentage)
{
    printf("Callback: %d percent complete...\n", percentage);
}

int main()
{
    cb_t progress = progress_callback;
    long_library_function(progress);

    return 0;
}