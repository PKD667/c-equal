#pragma once

#include "structs.h"



#define OFFSET 0x7c00

#define DATA_POS 0x40
#define DATA_SIZE 0x100

#define FUNC_POS 0x140
#define FUNC_SIZE 0x80

#define MAX_FUNC 64
#define MAX_VAR 64

#define SIZE 0x200

extern unsigned char* OUTPUT;

extern unsigned int CODE_PTR;
extern unsigned int DATA_PTR;
extern unsigned int FUNC_PTR;

extern struct var** VAR_TABLE;
extern unsigned int VAR_COUNT;

extern struct func** FUNC_TABLE;
extern unsigned int FUNC_COUNT;
