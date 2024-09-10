#include "../include/parser.h"
#include "../include/cutils.h"

#include <stdlib.h>
#include <stdlib.h>
#include <stdio.h>
#include "limits.h"




struct AST* parse_op(int* tokens, int* index) {

    dbg(2, "Parsing operation from %d", tokens[*index]);

    struct AST *ast = malloc(sizeof(struct AST));
    ast->type = AST_OP;
    ast->op = malloc(sizeof(struct ASTop));

    // Syntaxic definition
    /*
        op -> arg op arg
            - <arg> <op> <arg>
    */

    // Index is set at the beginning of an operation, or series of operations
    // The arguments are not yet parsed, and we need to recursively parse them

    // Parse the left argument
    // get the len of the left argument
    int len = 0;
    while (!t_isop(tokens[*index + len])) {

        // skip any

        len++;
    }

    if (len == 0) {
        // operation is one sided
        ast->op->left = NULL;
    } else {
        ast->op->left = parse(tokens, index, len);
    }

    // Parse the operator
    switch (tokens[*index]) {
        case T_PLUS:
            ast->op->tag = AST_ADD;
            break;
        case T_MINUS:
            ast->op->tag = AST_SUB;
            break;
        case T_STAR:
            ast->op->tag = AST_MUL;
            break;
        case T_SLASH:
            ast->op->tag = AST_DIV;
            break;
        case T_PERCENT:
            ast->op->tag = AST_MOD;
            break;
        case T_EQ:
            ast->op->tag = AST_CMP;
            break;
        case T_LSHIFT:
            ast->op->tag = AST_SHL;
            break;
        case T_RSHIFT:
            ast->op->tag = AST_SHR;
            break;
        case T_AND:
            ast->op->tag = AST_AND;
            break;
        case T_OR:
            ast->op->tag = AST_OR;
            break;
        case T_NOT:
            ast->op->tag = AST_NOT;
            break;
        default:
            printf("Unknown operator: %d\n", tokens[*index]);
            exit(1);
            break;
    }
    (*index)++;

    ast->op->right = parse(tokens, index,0);

    return ast;
}

struct AST* parse_arg(int t) {

    dbg(2, "Parsing argument %d", t);

    struct AST* ast = malloc(sizeof(struct AST));
    if (t < 0 ) {
        // variable
        ast->type = AST_VALUE;
        ast->value = malloc(sizeof(struct ASTvalue));
        ast->value->tag = AST_VAR;
        ast->value->v = -t;
    }
    else if (t == T_NUMBER || t == T_STRING) {
        // literal
        ast->type = AST_VALUE;
        ast->value = malloc(sizeof(struct ASTvalue));
        ast->value->tag = AST_LIT;
        ast->value->v = get_laddr();
    }
    else {
        msg(ERROR, "Unknown argument type\n");
        exit(1);
    }
    return ast;
}

struct AST* parse_specifier(int t) {

        dbg(2, "Parsing specifier %d", t);

        struct AST* ast = malloc(sizeof(struct AST));
        ast->type = AST_SPEC;
        ast->spec = malloc(sizeof(struct ASTspec));

        switch (t) {
            case T_T8:
                ast->spec->tag = AST_8;
                break;
            case T_T16:
                ast->spec->tag = AST_16;
                break;
            case T_T32:
                ast->spec->tag = AST_32;
                break;
            default:
                printf("Unknown specifier: %d\n", t);
                exit(1);
                break;
        }

        return ast;
}

struct AST* parse_loop(int* tokens, int* index) {

    dbg(2, "Parsing loop from %d", tokens[*index]);

    // Syntaxic definition
    /*
        loop -> while ( condition ) block
            - T_WHILE T_LPAREN <condition> T_RPAREN block
        loop -> for ( declare ; condition ; assign ) block
            - T_FOR T_LPAREN <declare> T_SEMICOLON <condition> T_SEMICOLON <assign> T_RPAREN <block>
    */

    struct AST* ast = malloc(sizeof(struct AST));
    ast->type = AST_LOOP;
    ast->loop = malloc(sizeof(struct ASTloop));

    switch (tokens[*index]) {
        case T_WHILE:
            ast->loop->tag = AST_WHILE;
            break;
        case T_FOR:
            ast->loop->tag = AST_FOR;
            break;
        default:
            printf("Unknown loop: %d\n", tokens[*index]);
            exit(1);
            break;
    }

    (*index)++;

    if (tokens[*index] != T_LPAREN) {
        msg(ERROR, "Expected '(' in loop\n");
        exit(1);
    }

    (*index)++;

    if (ast->loop->tag == AST_WHILE) {
        ast->loop->condition = parse(tokens, index,0);
    }
    else  {
        struct AST* condblock = malloc(sizeof(struct AST));
        condblock->type = AST_BLOCK;
        condblock->block = malloc(sizeof(struct ASTblock));
        condblock->block->size = 0;
        condblock->block->childs = malloc(3 * sizeof(struct AST));

        for (int i = 0; i < 3; i++) {
            condblock->block->childs[i] = parse(tokens, index,0);
        }
        condblock->block->size = 3;
        ast->loop->condition = condblock;
    }

    if (tokens[*index] != T_RPAREN) {
        msg(ERROR, "Expected ')' in loop\n");
        exit(1);
    }

    (*index)++;

    ast->loop->body = parse(tokens, index,0);

    return ast;
}

struct AST* parse_call(int* tokens, int* index) {

    dbg(2, "Parsing call from %d", tokens[*index]);

    // Syntaxic definition
    /*
        call -> call ( args )
            - <name> T_LPAREN <args> T_RPAREN
        args -> arg , args | arg
            - <arg> T_COMMA <args> | <arg>
    */
    struct AST* ast = malloc(sizeof(struct AST));
    ast->stmt = malloc(sizeof(struct ASTstmt));
    ast->stmt->tag = AST_CALL;

    if (!t_isvar(tokens[*index])) {
        msg(ERROR, "Expected variable in call\n");
        exit(1);
    }

    (*index)++;

    if (tokens[*index] != T_LPAREN) {
        msg(ERROR, "Expected '(' in call\n");
        exit(1);
    }
    (*index)++;

    ast->stmt->argc = 0;
    int capacity = 10;
    ast->stmt->args = malloc(capacity * sizeof(struct AST*));

    int pi = *index; // parenthesis index
    if (skip_to(tokens, &pi, T_RPAREN,0) != 0) {
        msg(ERROR, "Expected ')' in call\n");
        exit(1);
    }
    int mlen = pi - *index;

    while (tokens[*index] != T_RPAREN) {

        int ci = *index; // comma index
        if (skip_to(tokens, &ci, T_COMMA, mlen) == 0) {
            mlen = ci - *index;
        }

        if (ast->stmt->argc >= capacity) {
            capacity *= 2;
            ast->stmt->args = realloc(ast->stmt->args, capacity * sizeof(struct AST*));
        }
        ast->stmt->args[ast->stmt->argc] = parse(tokens, index, mlen);
        ast->stmt->argc++;
        (*index)++;
        if (tokens[*index] == T_COMMA) {
            (*index)++;
        }
    }

    return ast;
}


struct AST* parse_stmt(int* tokens, int* index) {

    dbg(2, "Parsing statement from %d", tokens[*index]);

    // Syntaxic definition
    /*
        stmt -> return arg
            - T_RETURN <arg>
        stmt -> break
            - T_BREAK
        stmt -> continue
            - T_CONTINUE
        stmt -> call
            - T_CALL <call>
    */

    struct AST* ast = malloc(sizeof(struct AST));
    ast->type = AST_STMT;
    ast->stmt = malloc(sizeof(struct ASTstmt));

    switch (tokens[*index]) {
        case T_RETURN:

            dbg(2, "Parsing return statement");

            ast->stmt->tag = AST_RETURN;

            (*index)++;

            // get semicolon index
            int si = *index;
            if (skip_to(tokens,&si,T_SEMICOLON,0) != 0) {
                msg(ERROR,"Missing ';' after return");
            }

            if (si - *index > 1) {
                dbg(3,"Found args");
                ast->stmt->args = malloc(sizeof(struct AST*));
                ast->stmt->args[0] = parse(tokens, index, si - *index);
                ast->stmt->argc = 1;
            } else {
                dbg(3,"No args ");
                ast->stmt->args = NULL;
                ast->stmt->argc = 0;
            }
            break;
        case T_BREAK:

            dbg(2, "Parsing break statement");

            ast->stmt->tag = AST_BREAK;
            ast->stmt->args = NULL;
            break;
        case T_CONTINUE:

            dbg(2, "Parsing continue statement");

            ast->stmt->tag = AST_CONTINUE;
            ast->stmt->args = NULL;
            break;
        default:
            printf("Unknown statement: %d\n", tokens[*index]);
            exit(1);
            break;
    }

    (*index)++;

    return ast;
}

struct AST* parse_condition(int* tokens, int* index) {

    dbg(2, "Parsing condition from %d", tokens[*index]);

    // Syntaxic definition
    /*
        condition -> if ( condition ) block
            - T_IF T_LPAREN <condition> T_RPAREN <block>
        condition -> elif ( condition ) block
            - T_ELIF T_LPAREN <condition> T_RPAREN <block>
        condition -> else block
            - T_ELSE <block>
    */

    struct AST* ast = malloc(sizeof(struct AST));
    ast->type = AST_CONDITION;
    ast->condition = malloc(sizeof(struct ASTcond));

    switch (tokens[*index]) {
        case T_IF:
            ast->condition->tag = AST_IF;
            break;
        case T_ELIF:
            ast->condition->tag = AST_ELIF;
            break;
        case T_ELSE:
            ast->condition->tag = AST_ELSE;
            break;
        default:
            printf("Unknown condition: %d\n", tokens[*index]);
            exit(1);
            break;
    }

    (*index)++;

    if (tokens[*index] != T_LPAREN) {
        printf("Expected '(' in condition\n");
        exit(1);
    }

    (*index)++;

    // get RPAREN index
    int rparen = *index;
    while (tokens[rparen] != T_RPAREN) {
        // if lparen, skip until rparen
        if (tokens[rparen] == T_LPAREN) {
            rparen++;
            while (tokens[rparen] != T_RPAREN) {
                rparen++;
            }
        }

        rparen++;
    }

    ast->condition->condition = parse(tokens, index, rparen - *index);

    if (tokens[*index] != T_RPAREN) {
        printf("Expected ')' in condition\n");
        exit(1);
    }

    (*index)++;

    ast->condition->body = parse(tokens, index,0);

    return ast;
}


struct AST* parse_declare(int* tokens, int* index) {

    dbg(2, "Parsing declare from %d", tokens[*index]);

    struct AST* ast = malloc(sizeof(struct AST));
    ast->stmt = malloc(sizeof(struct ASTstmt));
    ast->stmt->tag = AST_DECLARE;

    if (!t_istype(tokens[*index])) {
        printf("Expected type in declare\n");
        exit(1);
    }

    ast->stmt->tag = AST_DECLARE;
    // first argument is the type
    int capacity = 2;
    ast->stmt->argc = 0;
    ast->stmt->args = malloc(capacity * sizeof(struct AST*));
    ast->stmt->args[0] = parse_specifier(tokens[*index]);
    (*index)++;
    ast->stmt->argc++;
    // second argument is the variable
    if (!t_isvar(tokens[*index])) {
        printf("Expected variable in declare\n");
        exit(1);
    }
    ast->stmt->args[1] = parse_arg(tokens[*index]);
    (*index)++;
    ast->stmt->argc++;

    // now add the arguments in the parenthtesis

    if (tokens[*index] != T_LPAREN) {
        printf("Expected '(' in declare\n");
        exit(1);
    }

    (*index)++;

    while (tokens[*index] != T_RPAREN) {
        if (ast->stmt->argc >= capacity) {
            capacity *= 2;
            ast->stmt->args = realloc(ast->stmt->args, capacity * sizeof(struct AST*));
        }
        // first we should have a type indicator
        if (!t_istype(tokens[*index])) {
            msg(ERROR,"Expected type in declare argument\n");
            exit(1);
        }
        (*index)++;

        ast->stmt->args[ast->stmt->argc] = parse_arg(tokens[*index]);
        (*index)++;
        ast->stmt->argc++;


        if (tokens[*index] == T_COMMA) {
            (*index)++;
        }

    }

    (*index)++;

    return ast;

}

struct AST* parse_block(int* tokens, int* index) {

        dbg(2, "Parsing block from %d", tokens[*index]);

        // Syntaxic definition
        /*
            block -> { stmts }
                - T_LBRACE <stmts> T_RBRACE
            stmts -> stmt stmts | stmt
                - <stmt> <stmts> | <stmt>
        */

        struct AST* ast = malloc(sizeof(struct AST));
        ast->type = AST_BLOCK;
        ast->block = malloc(sizeof(struct ASTblock));
        ast->block->size = 0;
        int capacity = 10;
        ast->block->childs = malloc(capacity * sizeof(struct AST));

        if (tokens[*index] != T_LBRACE) {
            printf("Expected '{' in block\n");
            exit(1);
        }

        (*index)++;

        while (tokens[*index] != T_RBRACE) {
            if (ast->block->size >= capacity) {
                capacity *= 2;
                ast->block->childs = realloc(ast->block->childs, capacity * sizeof(struct AST));
            }
            ast->block->childs[ast->block->size] = parse(tokens, index,0);
            ast->block->size++;
        }

        (*index)++;

        return ast;
}

struct AST* parse_opblock(int* tokens, int* index) {

    dbg(2, "Parsing operation block from %d", tokens[*index]);

    // parse an operation block such as (a + b) * c
    // It should return only nested operations

    struct AST* ast;;

    if (tokens[*index] != T_LPAREN) {
        msg(ERROR, "Expected '(' in operation block\n");
        exit(1);
    }
    (*index)++;

    int nin = *index;
    skip_to(tokens, &nin, T_RPAREN,0);

    dbg(3, "Parsing operation block from %d to %d", *index, nin);


    ast = parse(tokens, index, nin - *index);

    if (tokens[*index] != T_RPAREN) {
        msg(ERROR, "Expected ')' in operation block\n");
        exit(1);
    }
    (*index)++;

    return ast;
}


struct AST* parse(int* tokens, int* index,int len) {

    msg(INFO,"Parsing %d tokens...", len);

    static int rec = 0;
    rec++;
    if (rec > 10) {
        msg(ERROR, "Recursion limit reached");
        exit(1);
    }

    if (len == 0) {
        len = INT_MAX;
    }

    // The parser will be a recursive descent parser

    // The parser need to identify structures in the tokenized code
    // the parser will return an AST (Abstract Syntax Tree) element.

    // in order to identify structure we will use a pattern table

    // create a parser variable (function pointer) to the parse function
    // the parser will return an AST element

    // Assuming match_pattern returns a pair where value is a function pointer
    pattern* pattern = match_pattern(tokens + *index, len);
    if (pattern == NULL) {
        // Handle error: no matching pattern found
        msg(ERROR,"No matching pattern found\n");
        return NULL;
    }

    // Cast the value to a function pointer type
    struct AST* (*parser)(int*, int*) = (void*)pattern->func;

    // Check if parser is not NULL before calling it
    if (parser == NULL) {
        // Handle error: no parser function found
        return NULL;
    }

    // Call the parser function
    struct AST* ast = parser(tokens, index);

    // Check if ast is not NULL before returning it
    if (ast == NULL) {
        // Handle error: parsing failed
        return NULL;
    }

    rec--;
    return ast;
}

struct ASTblock* parsefile(int* tokens, char** litterals) {

    dbg(2, "Parsing file");

    struct ASTblock* block = malloc(sizeof(struct ASTblock));
    block->size = 0;
    int capacity = 10;
    block->childs = malloc(capacity * sizeof(struct AST));

    int index = 0;

    while (tokens[index] != T_EOF) {
        if (block->size >= capacity) {
            capacity *= 2;
            block->childs = realloc(block->childs, capacity * sizeof(struct AST));
        }
        dbg(2, "Parsing child %d", block->size);
        dbg(2, "Parsing token %d == %s", tokens[index],get_token_string(tokens[index]));
        block->childs[block->size] = parse(tokens, &index,0);
        block->size++;
    }

    return block;
}


void visualize_ast(struct AST ast) {
    // print the AST in a tree like structure
    // we will use a recursive function to print the tree
    printf("---\n");
    switch (ast.type) {
        case AST_BLOCK:
            printf(" - Block - \n");
            for (int i = 0; i < ast.block->size; i++) {
                printf("Child %d\n", i);
                visualize_ast(*ast.block->childs[i]);
            }
            break;
        case AST_STMT:
            printf(" - Statement - \n");
            break;
        case AST_LOOP:
            printf(" - Loop - \n");
            visualize_ast(*ast.loop->condition);
            visualize_ast(*ast.loop->body);
            break;
        case AST_CONDITION:
            printf(" - Condition - \n");
            printf("Tag : %d\n", ast.condition->tag);
            printf("Condition : \n");
            visualize_ast(*ast.condition->condition);
            visualize_ast(*ast.condition->body);
            break;
        case AST_OP:
            printf(" - Operation -\n");
            printf("Operator : %d\n", ast.op->tag);
            printf("Left operand : \n");
            visualize_ast(*ast.op->left);
            printf("Right operand : \n");
            visualize_ast(*ast.op->right);
            break;
        case AST_VALUE:
            printf(" - Value -\n");
            printf("Type : %d\n", ast.value->tag);
            printf("Value : %d\n", ast.value->v);
            break;
        default:
            printf("Unknown AST type\n");
            break;
    }
}
