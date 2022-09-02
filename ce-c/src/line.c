#include "stdio.h"
#include "stdlib.h"
#include <string.h>

#include "../include/globals.h"
#include "../include/main.h"
#include "../include/structs.h"
#include "../include/array.h"
#include "../include/add.h"


int parse_line(byte* line,int l_size)
{
    printf("parsing line %s\n",line);
    if (line[0] == '$')
    {
        printf("detected variable setting \n");
        // name = line before '='
        // value = line after '='
        char* name = (char*)subarr(line, 1, arrchr(line, '=',l_size));
        byte* value = subarr(line, arrchr(line, '=',l_size), l_size);
        printf("name: %s\n", name);
        printf("value: %s\n", value);
        unsigned short int address;
        if (value[0] == '/')
        {
            // detected prepocessed value
            printf("detected preprocessed value\n");
            
            byte bytes_val[2];
            bytes_val[0] = value[1];
            bytes_val[1] = value[2]; 
            address = (unsigned short int)(bytes_val[0]+bytes_val[1]);
            printf("address: %x\n", address);

        }
        else
        {   
            // detected normal value
            printf("detected normal value\n");
            // on the line below we are converting the value to a char and then to an int 
            // its bad but it works
            int int_value = atoi((const char*)value);
            printf("int value: %d\n", int_value);
            unsigned char bytes_val[4];
            printf("bytes: %x\n",(unsigned char)int_value);
            address = add_data((unsigned char*)&int_value, sizeof(int));
        }
        printf("address: %x\n", address);
        set_var(name, address);

    }
    else {
        //other stuff
        // function call
        printf("detected function call\n");
        // function name  = line before '('
        // argument = line between '(' and ')'
        // there can be only one argument
        char* name = (char*)subarr(line, 0, arrchr(line, '(',l_size));
        printf("name: %s\n", name);
        byte* argument = (byte*) subarr(line, arrchr(line, '(',l_size)+1, arrchr(line, ')',l_size));
        
        printf("argument: %s\n", argument);

        // check if its a preprocessed value
        unsigned short int address;
        if (argument[0] == '/')
        {
            // detected prepocessed value
            printf("detected preprocessed value\n");
            printf("argument: %s\n", argument);
            byte bytes_val[2];
            bytes_val[0] = argument[1];
            bytes_val[1] = argument[2];
            address = (unsigned short int)(bytes_val[0]+bytes_val[1]);

        }
        else if (get_var((char*)argument) != 0)
        {
            printf("argument is a variable\n");
            word address = get_var((char*)argument);
            printf("address: %x\n", address);
        }
        else
        {
            // argument is a normal value
            printf("argument is a normal value\n");
            int int_value = atoi((char*)argument);
            printf("int value: %d\n", int_value);
            byte bytes_val[4];
            printf("bytes: %x\n",(unsigned char)int_value);
            word address = add_data((unsigned char*)&int_value, sizeof(int));
        }
        
        // add call
        add_call(name, address);
    }

    return 0;

}

