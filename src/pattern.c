#include "../include/parser.h"
#include "../include/cutils.h"


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>


// First I think we shall define a structure hashtable to store the structure
// The structures will be stored as a regex-like pattern.
// For example, an inline-block will be stored as a pattern of the form:
// {T_LBRACE, *, T_RBRACE}

// TABLE OF SYMBOLS
/*
    - Wildcard: * = 666
    - Any: ? = 667
    - Optional: ! = 668
    - Variable: $ = 669
    - Litteral: ' = 670
    - Any Type: # = 671
    - Any Op: -+* = 672
*/

#define WILDCARD 666
#define ANY 667
#define OPTIONAL 668 // Optional affects the next token
#define OR 669
#define TYPE 670
#define VAR 671
#define STOP 0




// 
#define M_BLOCK -1
#define OP_BLOCK -2
#define VALUE -3
#define OP -4
#define VAR_DEC -5
#define FUNC_DEC -6
#define FUNC_CALL -7
#define CONDITION -8
#define W_LOOP -9
#define F_LOOP -10
#define BREAK -11
#define CONTINUE -12
#define RETURN -13



pattern patterns[] = {
    // Actual patterns
    // Multilign block
    { (int[]){T_LBRACE, WILDCARD, T_RBRACE, STOP}, M_BLOCK, parse_block},
    // Operation block
    {(int[]) {T_LPAREN,VALUE, T_RPAREN, STOP}, OP_BLOCK, parse_opblock},
    // Operation
    {(int[]) {OPTIONAL,VALUE,
                            // arithmetic operators
                            T_PLUS,OR,T_MINUS,OR,T_STAR,OR,T_SLASH,OR,T_PERCENT,OR,
                            // comparison operators
                            T_EQ,OR,T_GT,OR,T_GTE,OR,
                            // logical operators
                            T_AND,OR,T_OR,OR,T_NOT,OR,
                            // bitwise operators
                            T_LSHIFT,OR,T_RSHIFT,OR,
                            // assignment operators
                            T_ASSIGN,
                            VALUE,
                        STOP}, OP, parse_op},
    // variable declaration
    {(int[]) {TYPE,VAR,OPTIONAL,T_ASSIGN,OPTIONAL, VALUE, T_SEMICOLON,STOP}, VAR_DEC, parse_declare},
    // function declaration
    {(int[]) {TYPE,VAR,T_LPAREN,WILDCARD,T_RPAREN,M_BLOCK,STOP}, FUNC_DEC, parse_declare},
    // function call
    {(int[]) {VAR,T_LPAREN,WILDCARD,T_RPAREN,T_SEMICOLON,STOP}, FUNC_CALL, parse_call},
    // conditions statement
    {(int[]) {T_IF,OR,T_ELSE,OR,T_ELIF,OP_BLOCK,M_BLOCK,STOP}, CONDITION, parse_condition},
    // while loop
    {(int[]) {T_WHILE,OP_BLOCK,M_BLOCK,STOP}, W_LOOP, parse_loop},
    // for loop
    {(int[]) {T_FOR,T_LPAREN,VALUE,T_SEMICOLON,VALUE,T_SEMICOLON,VALUE,T_RPAREN,M_BLOCK,STOP}, F_LOOP, parse_loop},
    // return statement
    {(int[]) {T_RETURN,OPTIONAL,VALUE,T_SEMICOLON,STOP}, RETURN, parse_stmt},
    // break statement
    {(int[]) {T_BREAK,T_SEMICOLON,STOP}, BREAK, parse_stmt},
    // continue statement
    {(int[]) {T_CONTINUE,T_SEMICOLON,STOP}, CONTINUE, parse_stmt},

    // Utils for internal use (NULL func)
    {(int[]) {T_NUMBER,OR,VAR,OR,OP,OR,OP_BLOCK,STOP}, VALUE, NULL},
    { NULL, 0, NULL }
};
  

int pattlen(int* pattern) {
    int i = 0;
    while (pattern[i] != STOP) {
        i++;
    }
    return i;
}

pattern get_pattern(int tag) {
    for (int i = 0; patterns[i].exp != NULL; i++) {
        if (patterns[i].tag == tag) {
            return patterns[i];
        }
    }
    return (pattern) {NULL,0,NULL};
}


int match(int token, int** pattern);
int matches(int* tokens, int* pattern,int len);

int match_WILDCARD(int token, int** pattern) {

    if (**pattern == WILDCARD) {

        int* n_pattern = *pattern + 1;
        if (match(token, &n_pattern)) {
            *pattern = n_pattern;
            return 1;
        }
        return 1;
    }
    return 0;
}

int match_ANY(int token, int** pattern) {
    if (**pattern == ANY) {
        (*pattern)++;
        return 1;
    }
    return 0;
}




int match_TYPE(int token, int** pattern) {

    if (**pattern == TYPE) {
        if (t_istype(token)) {
            (*pattern)++;
            return 1;
        }
    }
    return 0;
}

int match_VAR(int token, int** pattern) {
    if (**pattern == VAR) {
        if (t_isvar(token)) {
            (*pattern)++;
            return 1;
        }
    }
    return 0;
}



int match(int token, int** pattern) {
      if (match_ANY(token, pattern)) {
    } else if (match_WILDCARD(token, pattern)) {
    } else if (match_VAR(token, pattern)) {
    } else if (match_TYPE(token, pattern)) {
    } else if (token == **pattern) {
        (*pattern)++;
    } else {
        return 0;
    }

    return 1;
}




// we will define a function to match the pattern
int matches(int* tokens, int* pattern,int len) {


    static int depths = 0;
    depths++;
    if (depths > 10) {
        msg(ERROR,"Pattern too deep");
        exit(1);
    }

    printf("Matching pattern: ");
    visualize_pattern(pattern);
    printf("With tokens: ");
    for (int j = 0; j < len && tokens[j] != T_EOF; j++) {
        printf("%s ", get_token_string(tokens[j]));
    }
    printf("\n");

    if (len == 0) {
        len = INT_MAX;
    }

    

    for (int* btok = tokens ;*tokens != T_EOF || tokens - btok  < len; tokens++) {

        if (*pattern == STOP) {
            return 0;
        }

        if (*pattern == OR) {
            // this mean we matched the previous pattern
            // thus we should skip to the end of the OR
            pattern++;
            while (*pattern != STOP && *(pattern+1) ==  OR) {
                pattern++;
            }
        }       

        if (*pattern == OPTIONAL) {
            printf("Checking for optional : %s\n",get_pattern_string(*(pattern+1)));
            if (matches(tokens, pattern + 1,1) == 0) {
                pattern+=2;
            }
            pattern++;
        } else if (*pattern < 0) {
            printf("Recursing into pattern %s\n",get_pattern_string(*pattern));
            if (matches(tokens, get_pattern(*pattern).exp,len) != 0) {
                return -1;
            }
            pattern++;
        } else if (match(*tokens, &pattern)) {
            printf("Matched %s\n",get_token_string(*tokens));
        } else if (*(pattern+1) == OR) {
            pattern += 2;
        } else {
            printf("Failed to match %s\n",get_token_string(*tokens));
            return -1;
        }


    }

    return -2;



}

pattern* match_pattern(int* tokens,int len) {
    // we need to match the toekns with the patterns
    // we will iterate over the patterns and compare the tokens with the pattern
    // if the pattern matches, we will return the function to call
    dbg(2,"Tokens: ");
    for (int j = 0; j < len && tokens[j] != T_EOF; j++) {
        printf("%s ", get_token_string(tokens[j]));
    }
    printf("\n");
    for (int i = 0; patterns[i].func != NULL; i++) {
        dbg(2,"Pattern (%d): ",i);
        visualize_pattern(patterns[i].exp);
        if (matches(tokens, patterns[i].exp,len) == 0) {
            return &patterns[i];
        }
        printf("\n");
    }

    return NULL;
}

char* get_pattern_string(int pattern_value) {

    if (pattern_value == STOP) {
        return "STOP";
    } else

    if (pattern_value  < 0) {
        char buffer[10];
        sprintf(buffer,"TAG(%d)",pattern_value);
        return strdup(buffer);
    } else if (pattern_value == WILDCARD) {
        return "*";
    } else if (pattern_value == ANY) {
        return "?";
    } else if (pattern_value == OPTIONAL) {
        return "!";
    } else if (pattern_value == VAR) {
        return "ID";
    } else if (pattern_value == TYPE) {
        return "TYPE";
    } else if (pattern_value == OR) {
        return "|";
    } else {
        return get_token_string(pattern_value);
    }
}

void visualize_pattern(int* pattern) {
    int i = 0;
    while (pattern[i] != STOP) {
        printf("%s ",get_pattern_string(pattern[i]));
        i++;
    }
    printf("\n");
}