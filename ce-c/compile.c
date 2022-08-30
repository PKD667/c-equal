
#include "cutils.h"
#include "string.h"
#include <stdlib.h>
#include <stdio.h>



unsigned int compile_ins(unsigned char** bytes,char* ins)
{
    (*bytes) = calloc(16, sizeof(unsigned char));
    unsigned int bytes_count = 0;
    if (ins[0] == '@') {
        // this is an instruction
        char* instruction = substr(ins, 1, strlen(ins) - 1);

        // check if instruction is valid
        if (strcmp(instruction, "setal") == 0)
        {
            // the argument must be a 16 bit address
            // add instruction to bytes
            (*bytes)[bytes_count++] = 0xA0;

        }
        else if ((strcmp(instruction, "inter") == 0))
        {
            (*bytes)[bytes_count++] = 0xCD;
        }
        else {
            printf("invalid instruction\n");
            exit(1);
        }
        

    }
    return bytes_count;
}