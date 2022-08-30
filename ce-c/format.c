#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "globals.h"

unsigned char* OUTPUT;

unsigned char* DATA;
unsigned int C_DATA;

unsigned char* CODE;
unsigned int C_CODE;

struct var* VAR_TABLE;
unsigned int VAR_COUNT;

struct func* FUNC_TABLE;
unsigned int FUNC_COUNT;

void init()
{
    OUTPUT = (unsigned char *)calloc(SIZE, sizeof(unsigned char));

    OUTPUT[0] = 0xb4;
    OUTPUT[1] = 0xe0;
    OUTPUT[SIZE - 2] = 0x55;
    OUTPUT[SIZE - 1] = 0xAA;
    
    DATA = (unsigned char *)calloc(DATA_SIZE, sizeof(unsigned char));
    C_DATA = 0;
    

    VAR_TABLE = (struct var *)calloc(MAX_FUNC, sizeof(struct var));
    VAR_COUNT = 0;

    FUNC_TABLE = (struct func *)calloc(MAX_VAR, sizeof(struct func));
    FUNC_COUNT = 0;

}
void store_data()
{
    if (OUTPUT[DATA_POS] != 0) {
        printf("error , instruction overflow");
        exit(1);
    }

    strcpy((char*)&OUTPUT[DATA_POS], (char*)DATA);

}

short unsigned int add_data(unsigned char* bytes, int size)
{
    short unsigned int* ptr_addr = calloc(1,sizeof(short unsigned int));
    short unsigned int addr = *ptr_addr; 
    addr = C_DATA;
    
    for (int i = 0; i < size; i++) {
        DATA[C_DATA] = bytes[i];
        C_DATA++;
    }
    DATA[C_DATA] = '\0';
    C_DATA++;
    
    return addr;
}
