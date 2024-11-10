#pragma once

#include "parser.h"


int* encode_literals(char** litterals,char** encoded_literals);

enum ins {

    I_EOF,
    // memory
    I_MOV,
    // arithmetic
    I_ADD,
    I_SUB,
    I_MUL,
    I_DIV,
    // logic
    I_AND,
    I_OR,
    I_XOR,
    I_NOT,
    I_CMP,
    I_SHL,
    I_SHR,
    // control
    I_JMP,
    I_JZ,
    // stack
    I_PUSH,
    I_POP,
    // call
    I_CALL,
    I_RET,
    // misc
    I_NOP,

};


// this represents data for a variable in an ASM file
// Here is how it would translate
/*
<name>:
    .data <size> <data>
*/
struct ASM_VAR {
    char* name;
    unsigned int size;
    byte* data;
};


struct ASM_ARG {
    enum {REG,VAR,LIT,FN} type ;

    union {
        int val;

        // named addr
        char* point;
    };
};

// this represents an operation in the asm code
// its pretty lossely derived from basic APU ops
// Here is how it should translate
/*
<op> <a1> <a2>
*/
struct ASM_OP {
    enum ins op;

    struct ASM_ARG a1;
    struct ASM_ARG a2;
};

// This represents a section of executable data in the asm file
// Here is how it translates
/*
<code-id>:
    op_1 arg_1_1 arg_1_2
    op_2 arg_2_1 arg_2_2
        ...
    op_n arg_n_1 arg_n_2
*/
struct ASM_CODE {

    struct ASM_OP* ops;
};


// This represents a function in the asm file
// Here is how it translates
/*
.name <name>
code_1:
    [...]

code_2:
    [...]


*/
struct ASM_FN {

    struct ASM_CODE* code;

    char* name;
};

// This represents an entire ASM file
// Here is how it translates
/*
.file <filename>

// data part

var_1_name:
    [...]

var_2_name:
    [...]

    ...

var_n_name:
    [...]

// function part

.name fn_1_name
    [...]

.name fn_2_name
    [...]

    ...

fn_n_name:
    [...]


*/
struct ASM_FILE {

    char* filename;

    struct ASM_VAR* vars;

    struct ASM_FN* fonctions;
};


struct ASM_FILE* assemblify(struct ASTblock* block,byte** litterals,char** ids);