#pragma once

#include "structs.h"

void init();
void store_data();
unsigned int add_data(unsigned char* bytes, int size);

unsigned int parser(char* path,struct scope** scope_arr,char** code);

int set_var(char*name, unsigned char* value, unsigned int size);
int get_var(char*name, unsigned char** value);
int mod_var(char*name, unsigned char* value, unsigned int size);

unsigned int compile_ins(unsigned char** bytes,char* ins);

int def_function(char* name,struct scope f_scope,char** argv,unsigned int argc,unsigned char**argaddr,char* code);
unsigned int translate_func(unsigned char** f_bytes,struct scope f_scope,unsigned int argc,char** argv,unsigned char** argaddr,const char* code);