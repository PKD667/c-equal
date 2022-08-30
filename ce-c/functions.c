#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "structs.h"
#include "globals.h"
#include "cutils.h"
#include "main.h"
#include <stdbool.h>

unsigned int translate_func();

int def_function(char* name,struct scope f_scope,char** argv,unsigned int argc,unsigned char**argaddr,char* code)
{
    struct func new;
    new.name = name;
    new.argc = argc;
    new.argv = argv;
    FUNC_TABLE[FUNC_COUNT] = new;
    FUNC_COUNT++;

    // now we need to create the function in the code
    unsigned char* f_bytes;
    unsigned int f_size = translate_func(&f_bytes,f_scope,argc,argv,argaddr,code);
    
    for (int i = 0; i < f_size; i++)
    {
        CODE[C_CODE] = f_bytes[i];
        C_CODE++;
    }
    // add a null byte to the end of the function
    CODE[C_CODE] = 0;
    C_CODE++;

    return 0;
}
unsigned int translate_func(unsigned char** f_bytes,struct scope f_scope,unsigned int argc,char** argv,unsigned char** argaddr,const char* code)
{
    unsigned int rep_size = 0;
    for (int i = 0; i < f_scope.c_lines; i++)
    {
        struct line line = f_scope.lines[i];
        // get the line
        char* line_code = substr(code,line.start,line.end);
        // replace the variables by their adresses
        char* buffer = (char*)calloc(128,sizeof(char));
        for (int j,b = 0; j < strlen(line_code); j++)
        {
            if (line_code[j] == ' '){
                bool found = false;
                for (int k = 0; k < argc; k++)
                {
                    if (strcmp(argv[k],buffer) == 0)
                    {
                        // replace the variable by its address
                        char* addr = (char*)argaddr[k];
                        for (int l = 0; l < strlen(addr); l++)
                        {
                            (*f_bytes)[rep_size] = addr[l];
                            rep_size++;
                        }
                        found = true;
                        break;
                    }
                }
                if (!found)
                {
                    // its not a variable
                    unsigned char* bytes;
                    unsigned int bytes_size  = compile_ins(&bytes, buffer);
                    for (int l = 0; l < bytes_size; l++)
                    {
                        (*f_bytes)[rep_size] = bytes[l];
                        rep_size++;
                    }
                }
                // reset the buffer
                free(buffer);
                buffer = (char*)calloc(128,sizeof(char));
                b = 0;

            }

            buffer[b] = line_code[j];
            (*f_bytes)[j] = line_code[j];
            b++;
        }
    }

    return rep_size;
    
}