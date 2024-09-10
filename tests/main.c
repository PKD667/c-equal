#include "../include/parser.h"
#include "../include/cutils.h"

#include "assert.h"


void test_parser() {

    msg(INFO,"Testing the parser");

    int func_dec_tokens[] = {T_T8,-1,T_LPAREN,T_T32,-2,T_RPAREN,T_LBRACE,T_RETURN,-2,T_SEMICOLON,T_RBRACE,T_EOF};
    int index = 0;
    // test the parser with the function declaration
    struct AST* ast = parse((int*)func_dec_tokens,&index, sizeof(func_dec_tokens)/sizeof(int));

    visualize_ast(*ast);

    assert(ast->type == AST_STMT);
    assert(ast->stmt->tag == AST_DECLARE);

    // test the parser with the for loop
    int for_loop_tokens[] = {T_FOR,T_LPAREN,T_T8,T_T8,T_T8,T_RPAREN,T_LBRACE,T_RBRACE,T_EOF};
    index = 0;
    ast = parse((int*)for_loop_tokens,&index, sizeof(for_loop_tokens)/sizeof(int));

    visualize_ast(*ast);

    assert(ast->type == AST_STMT);
    assert(ast->stmt->tag == AST_FOR);

}

int main() {

    DEBUG=4;

    test_parser();
    return 0;
}
