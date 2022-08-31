#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#include "../include/add.h"

int load_asset(char* name)
{
    char* path = calloc(strlen(name) + 20, sizeof(char));
    sprintf(path, "../assets/%s.bin",name);
    printf("Loading asset: %s\n", path);

    FILE *file = fopen(path, "r");
    size_t n = 0;
    unsigned char* bytes = (unsigned char*)calloc(1024, sizeof(char)); 
    int c;

    if (file == NULL)
        return -1; //could not open file




    while ((c = fgetc(file)) != EOF)
    {
        bytes[n++] = (unsigned char)c; //cast here
    }
    unsigned int c_bytes = n;

    fclose(file);

    add_func(name, bytes, c_bytes);

    return 0;

}

