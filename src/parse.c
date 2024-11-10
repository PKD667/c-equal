#include "../include/parser.h"
#include "../include/cutils.h"

#include <stdlib.h>
#include <stdlib.h>
#include <stdio.h>
#include "limits.h"

int linectrl = 0;

struct AST* parse_arg(int t) {

    dbg(2, "Parsing argument %d", t);

    struct AST* ast = malloc(sizeof(struct AST));
    if (t < 0 ) {
        dbg(3,"Parsing variable");
        // variable
        ast->type = AST_VALUE;
        ast->value = malloc(sizeof(struct ASTvalue));
        ast->value->tag = AST_ID; // Identifier
        ast->value->v = -t;
    }
    else if (t == T_LINT || t == T_LFLOAT || t == T_LSTRING) {
        // literal
        ast->type = AST_VALUE;
        ast->value = malloc(sizeof(struct ASTvalue));
        switch (t) {
            case T_LINT:
                ast->value->tag = AST_LINT;
                break;
            case T_LFLOAT:
                ast->value->tag = AST_LFLOAT;
                break;
            case T_LSTRING:
                ast->value->tag = AST_LSTRING;
                break;
        }
        ast->value->v = get_laddr();
        dbg(3,"Literal address: %d", ast->value->v);
    }
    else {
        msg(ERROR, "Unknown argument type\n");
        return NULL;
    }
    return ast;
}

struct AST* parse_value(int* tokens, int* index) {

    dbg(2, "Parsing value from %d", tokens[*index]);

    struct AST* ast = malloc(sizeof(struct AST));
    ast->type = AST_VALUE;
    ast->value = malloc(sizeof(struct ASTvalue));

    // Syntaxic definition
    /*
        value -> var
            - <var>
        value -> lit
            - <lit>
    */

    // Index is set at the beginning of a value
   // use parse_arg to parse the value
    ast = parse_arg(tokens[*index]);

    (*index)++;

    return ast;
}

struct AST* parse_op(int* tokens, int* index) {

    int nset = 0;
    if (!linectrl) {
        linectrl = 1;
        nset = 1;
    }

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


    // first token is the left argument
    // first check if it is a variable or a literal
    struct AST* left = parse_arg(tokens[*index]);
    if (left == NULL) {
        msg(ERROR, "Failed to parse left argument\n");
        return NULL;
    }

    (*index)++;

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
        case T_LT:
            ast->op->tag = AST_CHLT;
            break;
        case T_GT:
            ast->op->tag = AST_CHGT;
            break;
        case T_LTE:
            ast->op->tag = AST_CHLTE;
            break;
        case T_GTE:
            ast->op->tag = AST_CHGTE;
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
        case T_ASSIGN:
            ast->op->tag = AST_ASSIGN;
            break;
        default:
            printf("Unknown operator: %d\n", tokens[*index]);
            exit(1);
            break;
    }
    (*index)++;
    
    struct AST* right = parse(tokens, index,0);
    dbg(3,"Got right arg");

    ast->op->left = left;
    ast->op->right = right;

    if (tokens[*index] == T_SEMICOLON && nset) (*index)++;
    linectrl -= nset;

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
            dbg(3, "Parsing for loop child %d", i);
            condblock->block->childs[i] = parse(tokens, index,0);
            // check for etra semicolons
            if (tokens[*index] == T_SEMICOLON) {
                (*index)++;
            } else if (tokens[*index] == T_RPAREN) {
                dbg(3, "End of for loop condition");
                break;
            } else {
                msg(ERROR, "Expected ';' in loop\n");
                // visualise the current token
                printf("Current token: %s\n", get_token_string(tokens[*index]));
                for (int j = *index+1; j < (*index + 5); j++) {
                    printf("Token %d: %s\n", j, get_token_string(tokens[j]));
                }
            }
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

    int nset = 0;
    if (!linectrl) {
        linectrl = 1;
        nset = 1;
    }

    dbg(2, "Parsing call from %d", tokens[*index]);

    // Syntaxic definition
    /*
        call -> call ( args )
            - <name> T_LPAREN <args> T_RPAREN
        args -> arg , args | arg
            - <arg> T_COMMA <args> | <arg>
    */
    // AST representation
    /*
        ASTstmt {
            .tag = AST_CALL
            .args = [
                ASTvalue id,
                AST* arg_1,
                ...
                AST* arg_n
            ]
            .argc n;
        }
    */
    struct AST* ast = malloc(sizeof(struct AST));
    ast->type = AST_STMT;
    ast->stmt = malloc(sizeof(struct ASTstmt));
    ast->stmt->tag = AST_CALL;

    if (!t_isvar(tokens[*index])) {
        msg(ERROR, "Expected variable in call\n");
        exit(1);
    }

    int call_id = tokens[*index];

    (*index)++;

    if (tokens[*index] != T_LPAREN) {
        msg(ERROR, "Expected '(' in call\n");
        exit(1);
    }
    (*index)++;

    ast->stmt->argc = 0;
    int capacity = 10;
    ast->stmt->args = malloc(capacity * sizeof(struct AST*));

    // adding the identifier as the first argument
    dbg(3,"Parsing arg %s",get_token_string(call_id));
    ast->stmt->args[0] = parse_arg(call_id);
    ast->stmt->argc = 1;


    int pi = *index; // parenthesis index
    if (skip_to(tokens, &pi, T_RPAREN,0) != 0) {
        msg(ERROR, "Expected ')' in call\n");
        exit(1);
    }
    int mlen = pi - *index;
    dbg(3,"mlen = %d",mlen);

    while (tokens[*index] != T_RPAREN) {

        int ci = *index; // comma index
        if (skip_to(tokens, &ci, T_COMMA, mlen) == 0) {
            mlen = ci - *index;
        }
        dbg(3,"mlen = %d",mlen);

        if (ast->stmt->argc >= capacity) {
            capacity *= 2;
            ast->stmt->args = realloc(ast->stmt->args, capacity * sizeof(struct AST*));
        }
        ast->stmt->args[ast->stmt->argc] = parse_arg(tokens[*index]);
        ast->stmt->argc++;
        (*index)++;
        if (tokens[*index] == T_COMMA) {
            (*index)++;
        }
    }
    (*index)++;

    dbg(3,"Got call() args");
    // check for semicolon
    if (tokens[*index] == T_SEMICOLON && nset ) {
        (*index)++;
    }
    linectrl -= nset;
    
    return ast;
}


struct AST* parse_stmt(int* tokens, int* index) {

    int nset = 0;
    if (!linectrl) {
        linectrl = 1;
        nset = 1;
    }

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
                dbg(3,"Found semicolon");
                msg(ERROR,"Missing ';' after return");
            }
            printf("si: %d",si - *index);

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
            (*index)++;

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

    // skip semicolon
    if (nset && tokens[*index] == T_SEMICOLON) (*index)++;
    linectrl -= nset;

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


struct AST* parse_function_declare(int* tokens, int* index) {

    dbg(2, "Parsing declare from %d", tokens[*index]);

    struct AST* ast = malloc(sizeof(struct AST));
    ast->fn = malloc(sizeof(struct ASTstmt));
    ast->type = AST_FN;

    if (!t_istype(tokens[*index])) {
        printf("Expected type in declare\n");
        exit(1);
    }

    // first argument is the type
    int capacity = 2;
    ast->fn->argc = 0;
    ast->fn->args = malloc(capacity * sizeof(struct AST*));
    ast->fn->args[0] = parse_specifier(tokens[*index]);
    (*index)++;
    ast->fn->argc++;
    // second argument is the variable
    if (!t_isvar(tokens[*index])) {
        printf("Expected variable in declare\n");
        exit(1);
    }
    ast->fn->args[1] = parse_arg(tokens[*index]);
    (*index)++;
    ast->fn->argc++;

    // now add the arguments in the parenthtesis

    if (tokens[*index] != T_LPAREN) {
        printf("Expected '(' in declare\n");
        exit(1);
    }

    (*index)++;

    while (tokens[*index] != T_RPAREN) {
        if (ast->fn->argc >= capacity) {
            capacity *= 2;
            ast->fn->args = realloc(ast->fn->args, capacity * sizeof(struct AST*));
        }
        // first we should have a type indicator
        if (!t_istype(tokens[*index])) {
            msg(ERROR,"Expected type in declare argument\n");
            exit(1);
        }
        (*index)++;

        ast->fn->args[ast->fn->argc] = parse_arg(tokens[*index]);
        (*index)++;
        ast->fn->argc++;


        if (tokens[*index] == T_COMMA) {
            (*index)++;
        }

    }

    (*index)++;

    struct AST* body = parse(tokens, index,0);

    // check if block is returned
    if (body->type != AST_BLOCK) {
        printf("Expected block in function declaration\n");
        exit(1);
    }

    ast->fn->body = body->block;

    return ast;

}

struct AST* parse_variable_declare(int* tokens, int* index) {

    dbg(2, "Parsing declare from %d", tokens[*index]);

    int nset = 0;
    if (!linectrl) {
        linectrl = 1;
        nset = 1;
    }

    struct AST* ast = calloc(1,sizeof(struct AST));
    ast->type = AST_VAR;
    ast->var = malloc(sizeof(struct ASTvalue));

    if (!t_istype(tokens[*index])) {
        printf("Expected type in declare\n");
        exit(1);
    }


    ast->var->spec = parse_specifier(tokens[*index]);
    (*index)++;
    // second argument is the variable
    if (!t_isvar(tokens[*index])) {
        printf("Expected variable in declare\n");
        exit(1);
    }
    struct AST* v_val = parse_arg(tokens[*index]);
    ast->var->id = v_val;
    (*index)++;

    // get distance to semicolon
    // get semicolon index
    int si = *index;
    if (skip_to(tokens,&si,T_SEMICOLON,0) != 0) {
        dbg(3,"Found semicolon");
        msg(ERROR,"Missing ';' after return");
    }
    int sd = si - *index; 

    dbg(3,"Semicolon distance: %d",sd);

    if (si == 0) {
        // no assigned init value 
        // skip to next token
        (*index)++;
    } else {
        if (tokens[*index] == T_ASSIGN) {
            // assigned init value
            (*index)--;

            ast->var->init = parse(tokens, index,0);
        } 
        if (tokens[*index] != T_SEMICOLON && nset) {
            printf("Expected ';' in declare\n");
            printf("Current token: %s\n", get_token_string(tokens[*index]));
            exit(1);
        }
        (*index)++;
    }
    linectrl -= nset;
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
        if (DEBUG) {
            msg(FATAL,"Exiting...");
            exit(1);
        }
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


struct ASTblock* parsefile(int* tokens) {

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


void visualize_ast(struct AST ast, byte** litterals, char** ids) {
    static int indent = 0;
    indent++;

    // Print the AST in a tree-like structure using a recursive function
    for (int i = 0; i < indent; i++) {
        printf("  ");
    }
    printf("---\n");

    for (int i = 0; i < indent; i++) {
        printf("  ");
    }
    switch (ast.type) {
        case AST_FN: 
            printf(" - Function -\n"); 
            visualize_ast(*ast.fn->args[0], litterals, ids);
            visualize_ast(*ast.fn->args[1], litterals, ids);
            for (int i = 2; i < ast.fn->argc;i++) {
                visualize_ast(*ast.fn->args[i], litterals,ids);
            }
            // stuff specific to function. Might chnage later
            struct AST body_wrp;
            body_wrp.type = AST_BLOCK;
            body_wrp.block = ast.fn->body;
            visualize_ast(body_wrp, litterals, ids);
            break;
        case AST_VAR:

            printf(" - Variable Declaration - \n"); 
            visualize_ast(*ast.var->spec, litterals, ids);
            visualize_ast(*ast.var->id, litterals, ids);
            if (ast.var->init != NULL) {
                visualize_ast(*ast.var->init, litterals, ids);
            }
            break;
        case AST_BLOCK:
            printf(" - Block - \n");
            for (int i = 0; i < ast.block->size; i++) {
                for (int j = 0; j < indent; j++) {
                    printf("  ");
                }
                printf("Child %d\n", i);
                if (ast.block->childs[i] != NULL) {
                    visualize_ast(*ast.block->childs[i], litterals, ids);
                } else {
                    for (int j = 0; j < indent; j++) {
                        printf("  ");
                    }
                    printf("NULL child\n");
                }
            }
            break;
        case AST_STMT:
            printf(" - Statement - \n");
            for (int i = 0; i < indent; i++) {
                printf("  ");
            }
            switch (ast.stmt->tag) {
                case AST_CALL: 
                    printf("Function Call\n");
                    visualize_ast(*ast.stmt->args[0],litterals,ids);
                    for (int i = 1; i < ast.stmt->argc;i++) {
                        visualize_ast(*ast.stmt->args[i], litterals,ids);
                    }
                    break;
                case AST_RETURN:
                    printf("Return Statement\n");
                    if (ast.stmt->argc > 0) {
                        for (int i = 0; i < ast.stmt->argc; i++){
                            visualize_ast(*ast.stmt->args[i], litterals, ids);
                        }
                    } else {
                        for (int i = 0; i < indent; i++) {
                            printf("  ");
                        }
                        printf("NULL return value\n");
                    }
                    break;
                case AST_BREAK: printf("Break Statement\n"); break;
                case AST_CONTINUE: printf("Continue Statement\n"); break;
                default: printf("Unknown Statement\n"); break;
            }
            break;
        case AST_LOOP:
            printf(" - Loop - \n");
            for (int i = 0; i < indent; i++) {
                printf("  ");
            }
            switch (ast.loop->tag) {
                case AST_WHILE: printf("While Loop\n"); break;
                case AST_FOR: printf("For Loop\n"); break;
                default: printf("Unknown Loop\n"); break;
            }
            if (ast.loop->condition != NULL) {
                visualize_ast(*ast.loop->condition, litterals, ids);
            } else {
                for (int i = 0; i < indent; i++) {
                    printf("  ");
                }
                printf("NULL condition\n");
            }
            if (ast.loop->body != NULL) {
                visualize_ast(*ast.loop->body, litterals, ids);
            } else {
                for (int i = 0; i < indent; i++) {
                    printf("  ");
                }
                printf("NULL body\n");
            }
            break;
        case AST_CONDITION:
            printf(" - Condition - \n");
            for (int i = 0; i < indent; i++) {
                printf("  ");
            }
            switch (ast.condition->tag) {
                case AST_IF: printf("If Condition\n"); break;
                case AST_ELIF: printf("Else If Condition\n"); break;
                case AST_ELSE: printf("Else Condition\n"); break;
                default: printf("Unknown Condition\n"); break;
            }
            if (ast.condition->condition != NULL) {
                visualize_ast(*ast.condition->condition, litterals, ids);
            } else {
                for (int i = 0; i < indent; i++) {
                    printf("  ");
                }
                printf("NULL condition\n");
            }
            if (ast.condition->body != NULL) {
                visualize_ast(*ast.condition->body, litterals, ids);
            } else {
                for (int i = 0; i < indent; i++) {
                    printf("  ");
                }
                printf("NULL body\n");
            }
            break;
        case AST_OP:
            printf(" - Operation -\n");
            for (int i = 0; i < indent; i++) {
                printf("  ");
            }
            switch (ast.op->tag) {
                case AST_ADD: printf("Addition\n"); break;
                case AST_SUB: printf("Subtraction\n"); break;
                case AST_MUL: printf("Multiplication\n"); break;
                case AST_DIV: printf("Division\n"); break;
                case AST_MOD: printf("Modulo\n"); break;
                case AST_CMP: printf("Comparison\n"); break;
                case AST_CHLT: printf("Less Than\n"); break;
                case AST_CHGT: printf("Greater Than\n"); break;
                case AST_CHLTE: printf("Less Than or Equal\n"); break;
                case AST_CHGTE: printf("Greater Than or Equal\n"); break;
                case AST_AND: printf("Logical AND\n"); break;
                case AST_OR: printf("Logical OR\n"); break;
                case AST_NOT: printf("Logical NOT\n"); break;
                case AST_XOR: printf("Logical XOR\n"); break;
                case AST_SHL: printf("Shift Left\n"); break;
                case AST_SHR: printf("Shift Right\n"); break;
                case AST_ASSIGN: printf("Assignment\n"); break;
                default: printf("Unknown Operation\n"); break;
            }
            if (ast.op->left != NULL) {
                visualize_ast(*ast.op->left, litterals, ids);
            } else {
                for (int i = 0; i < indent; i++) {
                    printf("  ");
                }
                printf("NULL left operand\n");
            }
            if (ast.op->right != NULL) {
                visualize_ast(*ast.op->right, litterals, ids);
            } else {
                for (int i = 0; i < indent; i++) {
                    printf("  ");
                }
                printf("NULL right operand\n");
            }
            break;
        case AST_VALUE:
            printf(" - Value -\n");
            for (int i = 0; i < indent; i++) {
                printf("  ");
            }
            switch (ast.value->tag) {
                case (AST_LINT || AST_LFLOAT || AST_LSTRING) : 
                    printf("Litteral id: %d\n", ast.value->v); 
                    for (int i = 0; i < indent; i++) {
                        printf("  ");
                    }
                    switch (ast.value->tag) {
                        case AST_LINT: printf("Litteral Type: Integer\n"); break;
                        case AST_LFLOAT: printf("Litteral Type: Float\n"); break;
                        case AST_LSTRING: printf("Litteral Type: String\n"); break;
                        default: printf("Unknown Litteral Type\n"); break;
                    }
                    for (int i = 0; i < indent; i++) {
                        printf("  ");
                    }
                    printf("Litteral Value: %s\n", litterals[ast.value->v]);
                    break;
                case AST_ID: 
                    printf("Variable id: %d\n", ast.value->v); 
                    for (int i = 0; i < indent; i++) {
                        printf("  ");
                    }
                    printf("Variable Name: %s\n",ids[ast.value->v - 1]);
                    break;

                default: printf("Unknown Value\n"); break;
            }
            break;
        case AST_SPEC:
            printf(" - Spec -\n");
            for (int i = 0; i < indent; i++) {
                printf("  ");
            }
            switch (ast.spec->tag) {
                case AST_8: printf("8-bit Spec\n"); break;
                case AST_16: printf("16-bit Spec\n"); break;
                case AST_32: printf("32-bit Spec\n"); break;
                default: printf("Unknown Spec\n"); break;
            }
            break;
        case AST_NULL:
            printf(" - NULL -\n");
            msg(WARNING,"Someone forgot to add the type to the AST creator.");
            break;
        default:
            printf("Unknown AST type\n");
            break;
    }

    indent--;
}