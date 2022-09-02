#include "stdio.h"
#include "stdlib.h"
#include <string.h>

#include "../include/globals.h"
#include "../include/main.h"
#include "../include/structs.h"
#include "../include/array.h"
#include "../include/add.h"



int main(int argc, char *argv[])
{
    printf("initiating stuff\n");
    init();
    
    printf("parsing scopes\n");
    struct scope* scopes;
    byte* code;
    int code_size;
    unsigned int scope_count = parser(argv[1],&scopes,&code,&code_size);

    //print code
    printf("code:\n");
    for (int i = 0; i < code_size; i++)
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
            printf("%d %d\n",scopes[i].lines[j].start,scopes[i].lines[j].end);
        }
        printf("\n");
    }
    
    // load assets here
    printf("loading assets\n");
    load_asset("print");

    // print all functions
    printf("functions:\n");
    for (int i = 0; i < FUNC_COUNT; i++)
    {
        printf("%d - %s\n",i,FUNC_TABLE[i]->name);
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

            byte *line = subarr(code, scopes[i].lines[j].start, scopes[i].lines[j].end);
            printf("Line is %s \n",line);

            parse_line(line, scopes[i].lines[j].end - scopes[i].lines[j].start);
        }
    }
    printf("Writing to file\n");
    write_bin(argv[2]);

    return 0;
}
