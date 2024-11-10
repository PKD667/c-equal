#include <stdio.h>
#include <string.h>
#include "../include/cutils.h"
#include "../include/parser.h"
#include "../include/assembly.h"



struct ASM_FN asm_fn(struct ASTfn* fn,struct ASM_VAR* vars, char** ids,byte** litterals);


// This function convverts a whole AST into an ASM file struct
struct ASM_FILE* assemblify(struct ASTblock* block ,byte** litterals,char** ids) {

    struct ASM_FILE* file = calloc(1,sizeof(struct ASM_FILE));

    for (int i = 0; litterals[i]; i++) {

        printf("%d ? ",(int)*(litterals[i]));

        printf("'");
        for (int j = 0; litterals[i][j]; j++) {
            printf("%c",litterals[i][j]);
        }
        printf("'\n");
    }

    // init our vars
    unsigned int id_count = 0;
    for (int i = 0; ids[i]; i++) {
        id_count++;
    }

    file->vars = calloc(id_count,sizeof(struct ASM_VAR));

    for (int i = 0; i < block->size; i++) {
        struct AST* child = block->childs[i];
        
        if (child->type != AST_FN) {
            msg(ERROR, "Expected function declaration");
            return NULL;
        }

        struct ASM_FN asm_f = asm_fn(child->fn,file->vars,ids,litterals);



    }


    return file;

}

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




struct ASM_FN asm_fn(struct ASTfn* fn, struct ASM_VAR* vars, char**ids, byte** litterals) {

    struct ASM_FN asm_func;


    asm_func.name = fn->name;

    for (int i = 0; i < fn->body->size; i++) {
        struct AST* child = fn->body->childs[i];
        if (child->type == AST_STMT) {
            switch (child->stmt->tag) {
            }
        } else {
            msg(ERROR, "Expected statement");
            return (struct ASM_FN){0};
        }
    }

    return asm_func;

}

