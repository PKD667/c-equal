#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "stdbool.h"

#include "../include/structs.h"
#include "../include/main.h"
#include "../include/add.h"



unsigned int parser(char* path,struct scope** scope_arr,char** code)
{
    FILE *file = fopen(path, "r");
    size_t n = 0;
    (*code) = (char*)calloc(1024, sizeof(char)); 
    int c;

    if (file == NULL)
        return -1; //could not open file


    (*scope_arr) = (struct scope*)calloc(512, sizeof(struct scope));
    int c_scopes = 0;
    
    (*scope_arr)[0].parent = NULL;
    (*scope_arr)[0].c_childrens = 0;
    (*scope_arr)[0].c_lines = 0;
    (*scope_arr)[0].lines = (struct line*)calloc(128, sizeof(struct line));

    (*scope_arr)[c_scopes].lines[(*scope_arr)[c_scopes].c_lines].start = n;

    bool p_str = 0;
    int i_str = 0;
    char* buff_str = (char*)calloc(1024, sizeof(char));

    while ((c = fgetc(file)) != EOF)
    {
        if (c == '"')
        {
            //printf("detected string\n");
            if (p_str)
            {
                //printf("end of string\n");
                // store buff_str in the executable
                short unsigned int addr = add_data((unsigned char*)buff_str, i_str);
                (*code)[n++] = '/';
                (*code)[n++] = *((unsigned char*)(&(addr)) + 0);
                (*code)[n++] = *((unsigned char*)(&(addr)) + 1);
                // print the addr in the code
                printf("addr: %x %x\n",(*code)[n - 1],(*code)[n - 2]);


                p_str = false;
                i_str = 0;
            }
            else {
                //printf("start string\n");
                p_str = true;
                i_str = 0;
            }
            continue;
        }

        if (p_str)
        {
            // here we need to transform the string into a memory address
            // and then add it to the output
            buff_str[i_str] = c;
            i_str++;
            continue;
        }

        if (c == ';')
        {
            (*scope_arr)[c_scopes].lines[(*scope_arr)[c_scopes].c_lines].end = n;
            (*scope_arr)[c_scopes].c_lines++;
            (*scope_arr)[c_scopes].lines[(*scope_arr)[c_scopes].c_lines].start = n+1;
        }
        

        if (c != '\n' && c != ' ')
        {
            (*code)[n++] = c;
        }


        
    }

    fclose(file);


    return c_scopes;

}