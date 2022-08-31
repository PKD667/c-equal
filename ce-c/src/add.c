#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#include "../include/globals.h"

int add_func(char* name, unsigned char* code, unsigned int c_code)
{
    struct func new_func;
    new_func.name = "print";
    new_func.addr = FUNC_COUNT;
    for (int i = 0; i < c_code; i++)
    {
        if (FUNC_COUNT >= MAX_FUNC)
        {
            printf("Too many functions\n");
            return -1;
        }

        if (OUTPUT[FUNC_PTR] != '\0')
        {
            printf("Function storage space already occupied : %x\n", FUNC_PTR);
            return -1;
        }
        
        OUTPUT[FUNC_PTR++] = code[i];
    }
    FUNC_TABLE[FUNC_COUNT++] = &new_func;
    return 0;
}

short unsigned int add_data(unsigned char* data, unsigned int c_data)
{
    short unsigned int addr = DATA_PTR;
    for (int i = 0; i < c_data; i++)
    {
        OUTPUT[DATA_PTR++] = data[i];
    }
    OUTPUT[DATA_PTR++] = 0x00;

    return addr;
}

int add_var(char* name, unsigned char* data, unsigned int c_data)
{
    struct var new_var;
    new_var.name = name;
    new_var.addr = add_data(data, c_data);
    VAR_TABLE[VAR_COUNT++] = &new_var;
    return 0;
}

// for now we only alllow one arg
int add_call(char* func_name,short unsigned int arg16)
{
    // find func in funcs
    short unsigned int func_addr = 0;
    for (int i = 0; i < FUNC_COUNT; i++)
    {
        if (strcmp(FUNC_TABLE[i]->name, func_name) == 0)
        {
            func_addr = FUNC_TABLE[i]->addr;
            break;
        }
    }
    if (func_addr == 0)
    {
        printf("Function not found: %s \n", func_name);
        return -1;
    }

    short int rel16 = func_addr  - CODE_PTR - 5;

    // set bx
    OUTPUT[CODE_PTR++] = 0xBB;
    
    OUTPUT[CODE_PTR++] = *((unsigned char*)(&(arg16)) + 0);
    OUTPUT[CODE_PTR++] = *((unsigned char*)(&(arg16)) + 1);

    OUTPUT[CODE_PTR++] = 0xE8;
    OUTPUT[CODE_PTR++] = *((unsigned char*)(&(rel16)) + 0);
    OUTPUT[CODE_PTR++] = *((unsigned char*)(&(rel16)) + 1);

    return 0;
}