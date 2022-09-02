#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "../include/globals.h"
#include "../include/add.h"

byte* OUTPUT;

unsigned int CODE_PTR;
unsigned int DATA_PTR;
unsigned int FUNC_PTR;

struct var** VAR_TABLE;
unsigned int VAR_COUNT;

struct func** FUNC_TABLE;
unsigned int FUNC_COUNT;

void init()
{
    OUTPUT = (unsigned char *)calloc(SIZE, sizeof(unsigned char));

    OUTPUT[SIZE - 2] = 0x55;
    OUTPUT[SIZE - 1] = 0xAA;
    
    CODE_PTR = 0x00;
    DATA_PTR = DATA_POS;
    FUNC_PTR = FUNC_POS;
    

    VAR_TABLE = (struct var **)calloc(MAX_FUNC, sizeof(struct var*));
    VAR_COUNT = 0;

    FUNC_TABLE = (struct func **)calloc(MAX_VAR, sizeof(struct func*));
    FUNC_COUNT = 0;

}

int mod_var(char*name, short unsigned int addr)
{
    struct var new;
    new.name = name;
    new.addr = addr;

    for (int i = 0; i < VAR_COUNT; i++) {
        if (strcmp(VAR_TABLE[i]->name, name) == 0) {
            VAR_TABLE[i] = &new;
            return 0;
        }
    }
    return -1;
}
int set_var(char*name, short unsigned int addr)
{
    struct var new;
    new.name = name;
    new.addr = addr;

    VAR_TABLE[VAR_COUNT++] = &new;
    return 0;
}

short unsigned int get_var(char*name)
{
    for (int i = 0; i < VAR_COUNT; i++) {
        if (strcmp(VAR_TABLE[i]->name, name) == 0) {
            return VAR_TABLE[i]->addr;
        }
    }
    return 0;
}

int write_bin(char* path)
{
    FILE *file = fopen(path, "wb");
    if (file == NULL)
        return -1; //could not open file
    fwrite(OUTPUT, sizeof(unsigned char), SIZE, file);
    fclose(file);
    return 0;
}
