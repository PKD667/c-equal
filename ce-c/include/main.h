#pragma once

#include "structs.h"

void init();

unsigned int parser(char* path,struct scope** scope_arr,char** code);

int mod_var(char*name, unsigned char* value, unsigned int size);
int set_var(char*name, short unsigned int addr);

int write_bin(char* path);

