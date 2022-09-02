#pragma once

#include "types.h"

struct line {
    unsigned int start;
    unsigned int end;
};
struct scope {
    struct line* lines;
    unsigned int c_lines;
    struct scope* parent;
    struct scope** childrens;
    unsigned int c_childrens;
}; 

struct func {
    char* name;
    word addr;
};
struct var {
    // the name is only used in the compiler, it wont be in the executable
    char* name;
    //  address of the variable in the executable
    word addr;
};