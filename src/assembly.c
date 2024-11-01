#include <stdio.h>
#include <string.h>
#include "../include/cutils.h"
#include "../include/parser.h"
#include "../include/assembly.h"



struct ASM_FN asm_fn(struct ASTfn* fn, char** ids, byte** litterals);

struct ASM_FILE* assemblify(struct ASTblock* block ,byte** litterals,char** ids) {

    struct ASM_FILE* file = calloc(1,sizeof(struct ASM_FILE));
    file->litterals = litterals;

    for (int i = 0; litterals[i]; i++) {

        printf("%d ? ",(int)*(litterals[i]));

        printf("'");
        for (int j = 0; litterals[i][j]; j++) {
            printf("%c",litterals[i][j]);
        }
        printf("'\n");
    }

    for (int i = 0; i < block->size; i++) {
        struct AST* child = block->childs[i];
        
        if (child->type != AST_FN) {
            msg(ERROR, "Expected function declaration");
            return NULL;
        }

        struct ASM_FN asm_f = asm_fn(child->fn,ids,litterals);



    }


    return file;

}

#define MAX_OPS 1024 // temporary
#define MAX_VARS_META 1024 // temporary

unsigned int get_var_size(struct ASTspec spec) {
    
    if (spec.tag == AST_8) {
        return 1;
    } else if (spec.tag  == AST_16) {
        return 2;
    } else if (spec.tag == AST_32) {
        return 4;
    } else {
        msg(ERROR,"Spec lookin' weird");
        exit(1);
    }
}

char* get_var_name(struct ASTvalue id,char** ids) {
    return ids[id.v - 1];
}

// dont take care of allocation for now
int add_var(byte* vars, unsigned int vars_size, byte* n_var, unsigned int n_var_size) {

    unsigned int new_size = vars_size + n_var_size;

    memcpy(vars + vars_size, n_var, n_var_size);

    return new_size;
}

// get position of var (iterative search)
// if >= 0 : position
// if < 0 : not found (error)
int get_var_pos(struct META* vars_meta, int var_count, char* var_name) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(vars_meta[i].name, var_name) == 0) {
            return vars_meta[i].pos;
        }
    }
    return -1;

}



struct ASM_FN asm_fn(struct ASTfn* fn, char** ids, byte** litterals) {

    struct ASM_FN asm_func;

    asm_func.code.ops = calloc(MAX_OPS,sizeof(struct ASM_OP));
    int ops_count = 0;

    int var_alloc = 1024;
    asm_func.code.vars = calloc(var_alloc, sizeof(byte));
    int var_used = 0;

    asm_func.code.vars_meta = calloc(MAX_VARS_META, sizeof(struct META));
    int var_count = 0;

    asm_func.name = fn->name;




    for (int i = 0; i < fn->body->size; i++) {
        struct AST* child = fn->body->childs[i];
        if (child->type == AST_STMT) {
            switch (child->stmt->tag) {
                case AST_VDECLARE: { // declare a variable

                    // variable are stored in a global register for the entire program
                    // might be unsafe
                    // We should ensure its zeroed after scope exit
                    // but the logic seems complicated, flemme

                    struct META meta = {0};

                    struct AST* type_spec = child->stmt->args[0];
                    if (type_spec->type != AST_SPEC) {
                        msg(ERROR,"Error here, var doesnt have a type specifier");
                        exit(1);
                    }

                    meta.size = get_var_size(*type_spec->spec);

                    struct AST* var_id = child->stmt->args[1];

                    if (var_id->type != AST_VALUE) {
                        msg(ERROR,"Error here, var doesnt have an ID");
                        exit(1);
                    }

                    // add the name to the meta header.
                    meta.name = get_var_name(*var_id->value, ids);

                    // set the position to the index where we will add the new variable
                    meta.pos = asm_func.code.vars_size;

                    // check for the arg[2] (init value)
                    if (child->stmt->argc > 2) {
                        // we have an init value
                        struct AST* init_val = child->stmt->args[2];
                        // check if it's a literal or a variable
                        if (init_val->type == AST_VALUE) {
                            if (init_val->value->tag == AST_VAR) {
                                // variable
                                // get the variable name
                                char* var_name = get_var_name(*init_val->value, ids);
                                // get the variable position
                                int var_pos = 0;
                                for (int i = 0; i < var_count; i++) {
                                    if (strcmp(asm_func.code.vars_meta[i].name, var_name) == 0) {
                                        var_pos = asm_func.code.vars_meta[i].pos;
                                        break;
                                    }
                                }
                                // add the variable to the new variable
                                asm_func.code.vars_size = add_var(
                                    asm_func.code.vars, asm_func.code.vars_size,
                                    asm_func.code.vars + var_pos, meta.size);
                            } else {
                                // literal
                                // add the literal to the new variable
                                asm_func.code.vars_size = add_var(
                                    asm_func.code.vars, asm_func.code.vars_size,
                                    litterals[init_val->value->v], meta.size);
                            }
                        } else {
                            msg(ERROR, "Expected literal or variable in init value");
                            exit(1);
                        }
                    }

                    asm_func.code.vars_size = add_var(
                        asm_func.code.vars, asm_func.code.vars_size,
                        , meta.size);


                    asm_func.code.vars_meta[var_count++] = meta;

                    break;
                }
                case AST_CALL: {
                    // call a function

                    struct ASM_OP call = {0};
                    call.op = I_CALL;

                    struct ASM_ARG arg  = {0};
                    arg.type = FN;

                    struct AST* fn_id = child->stmt->args[0];

                    if (fn_id->type != AST_VALUE) {
                        msg(ERROR,"Error here, var doesnt have an ID");
                        exit(1);
                    }

                    arg.fn = get_var_name(*fn_id->value, ids);

                    call.a1 = arg;

                    asm_func.code.ops[ops_count++] = call;

                    break;
                }
                case AST_RETURN: {
                    
                    struct ASM_OP ret = {0};
                    ret.op = I_RET;

                    struct ASM_ARG arg  = {0};

                    struct AST* ret_val;

                    if (child->stmt->args[0]->type != AST_VALUE) {
                        msg(ERROR,"Error here, var doesnt have an ID");
                        exit(1);
                    }

                    if (child->stmt->args[0]->value->tag == AST_VAR) {
                        arg.type = VAR;
                        arg.val = get_var_pos(asm_func.code.vars_meta, var_count, get_var_name(*child->stmt->args[0]->value, ids));
                    } else {
                        arg.type = LIT;
                        arg.val = child->stmt->args[0]->value->v;
                    }

                    ret.a1 = arg;

                    asm_func.code.ops[ops_count++] = ret;

                    break;
                }
                
                case AST_BREAK: {
                    // break the loop

                    struct ASM_OP brk = {0};

                    // check if its in a loop

                    break;
                }
                case AST_CONTINUE:
                    // continue to the next iteration
                    break;
                default:
                    msg(ERROR, "Unknown statement");
                    return NULL;
            }
        } else {
            msg(ERROR, "Expected statement");
            return NULL;
        }
    }

    return (int*)code;

}
