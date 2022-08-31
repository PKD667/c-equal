#include "stdio.h"
#include "stdlib.h"
#include <string.h>

#include "../include/globals.h"
#include "../include/main.h"
#include "../include/structs.h"
#include "../include/cutils.h"
#include "../include/add.h"



int main(int argc, char *argv[])
{
    printf("initiating stuff\n");
    init();
    
    printf("parsing scopes\n");
    struct scope* scopes;
    char* code;
    unsigned int scope_count = parser(argv[1],&scopes,&code);

    //print code
    printf("code:\n");
    for (int i = 0; i < strlen(code); i++)
    {
        printf("%c ",code[i]);
    }
    printf("\n");
    // print lines values
    printf("lines:\n");
    for (int i = 0; i < scope_count+1; i++)
    {
        for (int j = 0; j < scopes[i].c_lines; j++)
        {
            printf("%d %d",scopes[i].lines[j].start,scopes[i].lines[j].end);
        }
        printf("\n");
    }

    for (int i = 0; i < scope_count+1; i++)
    {
        printf("looping over scope %d\n", i);
        printf("scope has %d lines \n",scopes[i].c_lines);
        for (int j = 0; j < scopes[i].c_lines; j++)
        {
            // parse line
            if (scopes[i].lines[j].start == scopes[i].lines[j].end) {
                printf("empty line\n");
                continue;
            }

            char *line = substr(code, scopes[i].lines[j].start, scopes[i].lines[j].end);
            printf("Line is %s \n",line);

            if (line[0] == '$')
            {
                printf("detected variable setting \n");
                // name = line before '='
                // value = line after '='
                char* name = substr(line, 1, strchr(line, '=')-line);
                char* value = substr(line, strchr(line, '=')-line+1, strlen(line));
                printf("name: %s\n", name);
                printf("value: %s\n", value);
                unsigned short int address;
                if (value[0] == '/')
                {
                    // detected prepocessed value
                    printf("detected preprocessed value\n");
                    
                    unsigned char bytes_val[2];
                    bytes_val[0] = value[1];
                    bytes_val[1] = value[2]; 
                    address = (unsigned short int)(bytes_val[0]+bytes_val[1]);
                    printf("address: %x\n", address);

                }
                else
                {
                    int int_value = atoi(value);
                    unsigned char bytes_val[4];
                    bytes_val[0] = (int_value >> 24) & 0xFF;
                    bytes_val[1] = (int_value >> 16) & 0xFF;
                    bytes_val[2] = (int_value >> 8) & 0xFF;
                    bytes_val[3] = int_value & 0xFF;
                    address = add_data(bytes_val, strlen(value));
                }
                printf("address: %x\n", address);
                set_var(name, address);

            }
            else {
                printf("Unknown stuff\n");
            }
        }
    }
    printf("Writing to file\n");
    write_bin(argv[2]);

    return 0;
}
