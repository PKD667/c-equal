#include "../include/parser.h"
#include "../include/cutils.h"

#include "assert.h"


void test_parser() {

    msg(INFO,"Testing the parser");

    //test the parser with variable declaration
    int var_dec_tokens[] = {T_T8,-1,T_ASSIGN,T_NUMBER,T_SEMICOLON,T_EOF};
    int index = 0;
    struct AST* ast = parse((int*)var_dec_tokens,&index, sizeof(var_dec_tokens)/sizeof(int));

    visualize_ast(*ast);

    /// tst the parser with operations
    int op_tokens[] = {-1,T_ASSIGN,T_NUMBER,T_PLUS,T_NUMBER,T_SEMICOLON,T_EOF};
    index = 0;
    ast = parse((int*)op_tokens,&index, sizeof(op_tokens)/sizeof(int));

    visualize_ast(*ast);

    int func_dec_tokens[] = {T_T8,-1,T_LPAREN,T_T32,-2,T_RPAREN,T_LBRACE,T_RETURN,-2,T_SEMICOLON,T_RBRACE,T_EOF};
    index = 0;
    // test the parser with the function declaration
    ast = parse((int*)func_dec_tokens,&index, sizeof(func_dec_tokens)/sizeof(int));

    visualize_ast(*ast);

    assert(ast->type == AST_STMT);
    assert(ast->stmt->tag == AST_FDECLARE);

    // test the parser with the for loop
    int for_loop_tokens[] = {T_FOR,T_LPAREN,T_T8,-1,T_ASSIGN,T_NUMBER,T_SEMICOLON,-1,T_LT,T_NUMBER,T_SEMICOLON,-1,T_ASSIGN,-1,T_PLUS,T_NUMBER,T_RPAREN,T_LBRACE,T_RBRACE,T_EOF};
    index = 0;
    ast = parse((int*)for_loop_tokens,&index, sizeof(for_loop_tokens)/sizeof(int));

    visualize_ast(*ast);

    assert(ast->type == AST_LOOP);
    assert(ast->stmt->tag == AST_FOR);

    // test the parser with the while loop
    int while_loop_tokens[] = {T_WHILE,T_LPAREN,-1,T_LT,-2,T_RPAREN,T_LBRACE,T_RBRACE,T_EOF};
    index = 0;
    ast = parse((int*)while_loop_tokens,&index, sizeof(while_loop_tokens)/sizeof(int));

    visualize_ast(*ast);

    assert(ast->type == AST_LOOP);
    assert(ast->stmt->tag == AST_WHILE);

    
}

int main() {

    DEBUG=4;

    test_parser();
    return 0;
}
