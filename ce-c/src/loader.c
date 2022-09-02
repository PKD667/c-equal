#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#include "../include/add.h"

int load_asset(char* name)
{
    char* path = calloc(strlen(name) + 20, sizeof(char));
    sprintf(path, "assets/%s.bin",name);
    printf("Loading asset: %s\n", path);

    FILE *file = fopen(path, "r");

    if (file == NULL) {
        printf("Could not open asset: %s\n", path);
        return -1; //could not open file
    }
    size_t n = 0;
    unsigned char* bytes = (unsigned char*)calloc(1024, sizeof(char)); 
    int c;

    while ((c = fgetc(file)) != EOF)
    {
        bytes[n++] = (unsigned char)c; //cast here
    }
    unsigned int c_bytes = n;

    fclose(file);
    
    // PRINT ASSET
    printf("Asset: %s\n", name);
    for (int i = 0; i < c_bytes; i++)
    {
        printf("%x ",bytes[i]);
    }
    printf("\n");

    add_func(name, bytes, c_bytes);

    return 0;

}

