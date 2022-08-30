#pragma once

#include "structs.h"

#define SIZE 0x200

#define OFFSET 0x7c00

#define DATA_POS 0x40
#define DATA_SIZE 0x100

#define CODE_POS 0x2
#define CODE_SIZE 0x38

#define HEAP_POS 0x140
#define HEAP_SIZE 0x40

#define MAX_FUNC 64
#define MAX_VAR 64

extern unsigned char* OUTPUT;


extern unsigned char* DATA;
extern unsigned int C_DATA;

extern unsigned char* CODE;
extern unsigned int C_CODE;

extern struct var* VAR_TABLE;
extern unsigned int VAR_COUNT;

extern struct func* FUNC_TABLE;
extern unsigned int FUNC_COUNT;
