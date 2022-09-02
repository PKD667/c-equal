#pragma once

#include "globals.h"
#include "structs.h"


void init();

unsigned int parser(char* path,struct scope** scope_arr,byte** code,int* code_size); 

int mod_var(char*name, unsigned char* value, unsigned int size);
int set_var(char*name, short unsigned int addr);
short unsigned int get_var(char*name);

int write_bin(char* path);

int parse_line(byte* line,int l_size);

int load_asset(char* name);

