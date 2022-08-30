#pragma once

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
    int argc;
    char** argv;
};
struct var {
    // the name is only used in the compiler, it wont be in the executable
    char* name;
    // the value is the address of the variable in the executable
    unsigned char* val;
    // the size of the variable
    unsigned int size;
};