#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#define OFFSET 0x7c00

#define FUNC_POS 0x100
#define DATA_POS 0x60

struct func {
    char* name;
    short int addr;
};
struct var {
    char* name;
    short int addr;
};

unsigned char* output;

struct func funcs[10];
unsigned int c_funcs = 0;
struct var vars[10];
unsigned int c_vars = 0;

unsigned int code_ptr = 0;
unsigned int func_ptr = FUNC_POS;
unsigned int data_ptr = DATA_POS;


int add_func(char* name, unsigned char* code, unsigned int c_code);
int add_var(char* name, unsigned char* data, unsigned int c_data);
int add_call(char* func_name,short unsigned int arg16);

int load_asset(char* name);

int write_bin(char* filename);

int main(int argc, char **argv)
{

    output = calloc(512,sizeof(unsigned char));

    output[510] = 0x55;
    output[511] = 0xAA;


    load_asset("print");

    // Creating variable

    char* data = "Hello World!";
    add_var("var", (unsigned char*)data, strlen(data)+1);
    

    // adding actuall calling code
    struct var hello = vars[0];
    unsigned short int rl_addr = hello.addr + OFFSET;
    add_call("print", rl_addr);

    


    // Writin to file here
    
    write_bin(argv[2]);
}

int load_asset(char* name)
{
    char* path = calloc(strlen(name) + 20, sizeof(char));
    sprintf(path, "../assets/%s.bin",name);
    printf("Loading asset: %s\n", path);

    FILE *file = fopen(path, "r");
    size_t n = 0;
    unsigned char* bytes = (unsigned char*)calloc(1024, sizeof(char)); 
    int c;

    if (file == NULL)
        return -1; //could not open file




    while ((c = fgetc(file)) != EOF)
    {
        bytes[n++] = (unsigned char)c; //cast here
    }
    unsigned int c_bytes = n;

    fclose(file);

    add_func(name, bytes, c_bytes);

    return 0;

}

int add_func(char* name, unsigned char* code, unsigned int c_code)
{
    struct func new_func;
    new_func.name = "print";
    new_func.addr = func_ptr;
    for (int i = 0; i < c_code; i++)
    {
        output[func_ptr++] = code[i];
    }
    funcs[c_funcs++] = new_func;
    //free(code);
    return 0;
}

int add_var(char* name, unsigned char* data, unsigned int c_data)
{
    struct var new_var;
    new_var.name = name;
    new_var.addr = data_ptr;
    for (int i = 0; i < c_data; i++)
    {
        output[data_ptr++] = data[i];
    }
    vars[c_vars++] = new_var;
    //free(data);
    return 0;
}
// for now we only alllow one arg
int add_call(char* func_name,short unsigned int arg16)
{
    // find func in funcs
    short unsigned int func_addr = 0;
    for (int i = 0; i < c_funcs; i++)
    {
        if (strcmp(funcs[i].name, func_name) == 0)
        {
            func_addr = funcs[i].addr;
            break;
        }
    }
    if (func_addr == 0)
    {
        printf("Function not found: %s \n", func_name);
        return -1;
    }

    short int rel16 = func_addr  - code_ptr - 5;

    // set bx
    output[code_ptr++] = 0xBB;
    
    output[code_ptr++] = *((unsigned char*)(&(arg16)) + 0);
    output[code_ptr++] = *((unsigned char*)(&(arg16)) + 1);

    output[code_ptr++] = 0xE8;
    output[code_ptr++] = *((unsigned char*)(&(rel16)) + 0);
    output[code_ptr++] = *((unsigned char*)(&(rel16)) + 1);

    return 0;
}

int write_bin(char* filename)
{
    FILE *out = fopen(filename, "wb");
    fwrite(output, 1, 512, out);
    fclose(out);

    return 0;
}