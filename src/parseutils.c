// This file will include a collection of utilities to help parse the tokens and litterals from the lexer.

#include "../include/parser.h"
#include "limits.h"

// check if a token is an operator
int t_isop(int token) {
    return token >= T_PLUS && token <= T_ASSIGN;
}

int t_isopcomp(int token) {
    return token >= T_NEQ && token <= T_GTE;
}


// check if a token is a keyword
int t_iskeyword(int token) {
    return token >= T_IF && token <= T_T8;
}

int t_iscond(int token) {
    return token >= T_IF && token <= T_ELIF;
}

int t_isloop(int token) {
    return token >= T_WHILE && token <= T_FOR;
}

int t_isstmt(int token) {
    return token >= T_RETURN && token <= T_CONTINUE;
}

int t_istype(int token) {
    return token >= T_T32 && token <= T_T8;
}



// check if a token is a literal
int t_islit(int token) {
    return token == T_NUMBER || token == T_STRING;
}

int t_isvar(int token) {
    return token < 0;
}

// check if a token is a separator
int t_issep(int token) {
    return token >= T_SEMICOLON && token <= T_RBRACKET;
}

int get_laddr() {
    static int laddr = 0;
    return laddr++;
}

int skip_to(int* tokens, int* index, int token,int len) {


    if (len == 0) {
        len = INT_MAX;
    }

    int sind = *index;

    while (*index - sind <= len) {
        
        if (tokens[*index] == T_EOF) {
            return -1;
        }

        if (tokens[*index] == token) {
            break;
        }

        // if we encounter a block, we skip it
        if (tokens[*index] == T_LBRACE) {
            int depth = 1;
            while (depth > 0) {
                (*index)++;
                if (tokens[*index] == T_LBRACE) {
                    depth++;
                } else if (tokens[*index] == T_RBRACE) {
                    depth--;
                }
            }
        }

        (*index)++;
    }
    if (tokens[*index] != token) {
        return -1;
    }
    return 0;
}