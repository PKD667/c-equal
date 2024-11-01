#include "stdio.h"

#include "../include/cutils.h"
#include "../include/parser.h"
#include "../include/hashtable.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_TOKENS 1000

pair keywords[] = {
    // some hack because 0 is not a valid value
    {"",0},
    // separators
    {";",T_SEMICOLON},
    {",",T_COMMA},
    {":",T_COLON},
    {".",T_DOT},
    {"(",T_LPAREN},
    {")",T_RPAREN},
    {"{",T_LBRACE},
    {"}",T_RBRACE},
    {"[",T_LBRACKET},
    {"]",T_RBRACKET},
    // operators
    {"+",T_PLUS},
    {"-",T_MINUS},
    {"*",T_STAR},
    {"/",T_SLASH},
    {"==",T_EQ},
    {"!=",T_NEQ},
    {"<",T_LT},
    {">",T_GT},
    {"<=",T_LTE},
    {"%",T_PERCENT},
    {">=",T_GTE},
    {"&&",T_AND},
    {"||",T_OR},
    {"!",T_NOT},
    {">>",T_RSHIFT},
    {"<<",T_LSHIFT},


    {"=",T_ASSIGN},
    // keywords
    {"if",T_IF},
    {"else",T_ELSE},
    {"elif",T_ELIF},
    {"while",T_WHILE},
    {"for",T_FOR},
    {"return",T_RETURN},
    {"break",T_BREAK},
    {"continue",T_CONTINUE},
    // types
    {"t32",T_T32},
    {"t16",T_T16},
    {"t8",T_T8},

    // other
    {"(error)",T_ERROR},
    {"(integer)",T_LINT},
    {"(float)",T_LFLOAT},
    {"(string)",T_LSTRING},
    {"(unknown)",T_UNKNOWN},
        {"EOF",T_EOF}
};

// check is a string is a numeric value (int or float)
int numval(char* cs) {

    if (cs == NULL || *cs == '\0') { 
        return 0;
    }

    int fp = 0;

    char* c = cs;

    while (*c != '\0') {
        if (!isdigit(*c)) {
            if (*c == '.') {
                fp++;
            } else {
                return 0; // `char* cs` is not a number
            }
        }
        c++;
    }

    if (fp > 0) {
        if (fp > 1) {
            return 0; // two dots, not a float
        }
         return 2; // FLOAT
    } 

    return 1; // INT

}
// `char escaped` is the char after the '\'
char unscape(char escaped) {
    switch (escaped) {
        case 'n':
            return '\n';
        case '\\':
            return '\\';
        case '0':
            return '\0';
        default:
            return escaped;
    }
}

// in our litteral storage we will assume everything is a default c INT to be cautious.
// bigger values will get destroyed
// not memeory efficient but who cares about int sizes in executables ? 
// nowadays even string storage doesnt matter that much compared to images and stuff

int tokenize(char* str,int** tokens,byte*** litterals,char*** ids) {

    (*ids) = calloc(MAX_IDENTIFIERS, sizeof(char*));
    if (!(*ids)) {
        fprintf(stderr, "Memory allocation failed\n");
        return -1;
    }
    int num_ids = 0;

    (*litterals) = calloc(MAX_LITTERALS, sizeof(char*));
    if (!(*litterals)) {
        fprintf(stderr, "Memory allocation failed\n");
        return -1;
    }
    int litteral_count = 0;

    if (!tokens) {
        fprintf(stderr, "Memory allocation failed\n");
        return -1;
    }

    int token_index = 0;

    hashtable* ht = hm_init(keywords, sizeof(keywords) / sizeof(pair));
    if (!ht) {
        free(tokens);
        fprintf(stderr, "Hashtable initialization failed\n");
        return -2;
    }

    char* cur = str;
    while (*cur != '\0' && token_index < MAX_TOKENS - 1) {
        printf("Current char: %c\n", *cur);  // Print the current character

        // Skip whitespace
        while (isspace(*cur)) {
            cur++;
        }

        // check for comments and if found skip until newline
        if (*cur == '/' && *(cur + 1) == '/') {
            printf("One-Line comment found\n");
            while (*cur != '\n' && *cur != '\0') {
                cur++;
            }
            continue;
        }

        // check for "/*" and if found skip until "*/"
        if (*cur == '/' && *(cur + 1) == '*') {
            printf("Multi-Line comment found\n");
            while (!(*cur == '*' && *(cur + 1) == '/')) {
                cur++;
            }
            cur += 2;
            continue;
        }

        // check for '"' and if found collect the string token
        if (*cur == '"') {
            printf("String token found\n");
            cur++;
            char token_buffer[BUFFER_SIZE] = {0};
            int buffer_index = 0;
            while (*cur != '"' && *cur != '\0' && buffer_index < BUFFER_SIZE - 1) {

                // escaped characters
                if (*cur == '\\') {
                    token_buffer[buffer_index++] = unscape(*(cur+1));
                    cur += 2;
                    continue;
                }

                // normal behavior
                token_buffer[buffer_index++] = *cur++;
            }
            token_buffer[buffer_index] = '\0';
            if (*cur == '\0') {
                fprintf(stderr, "Unterminated string\n");
                hm_destroy(ht);
                free(tokens);
                return -1;
            }
            cur++;
            printf("String token: '%s'\n", token_buffer);  // Print the collected token
            (*tokens)[token_index++] = T_LSTRING;
            (*litterals)[litteral_count] = calloc(strlen(token_buffer) + 1,sizeof(char));
            if ((*litterals)[litteral_count] == NULL) {
                fprintf(stderr, "Memory allocation failed\n");
                exit(1);
            }
            // using memcpy because litterals is byte, but basically strcpy
            memcpy((*litterals)[litteral_count], token_buffer, strlen(token_buffer) + 1);
            litteral_count++;
            continue;
        }

        // Check for end of string
        if (*cur == '\0') {
            break;
        }

        // Buffer to collect a potential token
        char token_buffer[BUFFER_SIZE] = {0};
        int buffer_index = 0;

        // Collect alphanumeric token (identifiers, keywords)
        if (isalpha(*cur)) {
            while (isalnum(*cur) && buffer_index < BUFFER_SIZE - 1) {
                token_buffer[buffer_index++] = *cur++;
            }
            token_buffer[buffer_index] = '\0';

            printf("Alphanumeric token: %s\n", token_buffer);  // Print the collected token

            int tk = hm_get(ht, token_buffer);

            if (tk) { // Keyword found
                printf("Keyword: %s\n", token_buffer);
                (*tokens)[token_index++] = tk;
            } else { // Assumed to be an identifier if not a keyword
                // check if the identifier is already in the list
                int pos= -1;
                for (int i = 0; i < num_ids; i++) {
                    if (strcmp((*ids)[i], token_buffer) == 0) {
                        pos = i;
                        break;
                    }
                }
                if (pos == -1) {
                    // add the identifier to the list
                    (*ids)[num_ids] = calloc(strlen(token_buffer) + 1, sizeof(char));
                    strcpy((*ids)[num_ids], token_buffer);
                    (*tokens)[token_index++] = -(num_ids+1);
                    num_ids++;
                    printf("Identifier: %s -> %d\n", token_buffer,num_ids-1);  // Print the collected token
                }
                else {
                    printf("Identifier: %s, pos %d, (id %d)\n", token_buffer, pos,-(pos+1));
                    (*tokens)[token_index++] = -(pos+1);
                }
            }
        }       
        // Collect numeric tokens
        else if (isdigit(*cur)) {
            printf("Numeric token found\n");
            token_buffer[buffer_index++] = *cur;
            cur++;
            while (numval(token_buffer) && buffer_index < BUFFER_SIZE - 1) {
                printf("Numeric token: %s\n", token_buffer);  // Print the collected token
                token_buffer[buffer_index++] = *cur;
                cur++;
            }
            token_buffer[buffer_index-1] = '\0';
            cur--;
            
            printf("Numeric token: %s\n", token_buffer);  // Print the collected token

            int litt_nval = numval(token_buffer);


            if (litt_nval == 0) {
                fprintf(stderr, "Invalid numeric token\n");
                return -1;
            } else if (litt_nval == 1) {
                printf("Got INT\n");
                // parse the number as an integer 
                // store into the litterals list
                int i = atoi(token_buffer);
                printf("int('%s') == %d\n",token_buffer,i);
                (*litterals)[litteral_count] = calloc(1,sizeof(int));
                memcpy((*litterals)[litteral_count++], &i, sizeof(int));
                (*tokens)[token_index++] = T_LINT;
            } else if (litt_nval == 2) {
                printf("Got FLOAT\n");
                // parse the number as a float
                // store into the num array
                float f = atof(token_buffer);
                (*litterals)[litteral_count] = calloc(1,sizeof(float));
                memcpy((*litterals)[litteral_count++], &f, sizeof(float));
                (*tokens)[token_index++] = T_LFLOAT;
            }

            printf("Done processing number\n");
        }
        // Process symbol tokens, including potential compound symbols like "=="
        else {
            printf("Symbol token found\n");
            token_buffer[0] = *cur;
            int potential_compound = (*(cur + 1) && strchr("=<>!&|", *(cur + 1)) != NULL);
            if (potential_compound && buffer_index < BUFFER_SIZE - 2) {
                token_buffer[1] = *(++cur);
                token_buffer[2] = '\0';
            } else {
                token_buffer[1] = '\0';
            }
            cur++;

            printf("Symbol token: %s\n", token_buffer);  // Print the collected token

            int tk = hm_get(ht, token_buffer);
            if (tk) {
                (*tokens)[token_index++] = tk;
            } else {
                // For symbols not defined in the keywords list, an error or generic symbol token could be set
                printf("Unknown token: %s\n", token_buffer);
                (*tokens)[token_index++] = T_UNKNOWN;
            }
        }
    }
    (*tokens)[token_index] = T_EOF; // Assuming T_EOF is used to mark the end of the tokens array


    // add end NULL to litterals
    (*litterals)[litteral_count] = NULL;

    return 0;
}

char* get_token_string(int token) {

    if (token < 0) {
        char buffer[32];
        sprintf(buffer, "ID(%d)", -token);
        return strdup(buffer);

    }

    // use the keywords pair array to get the string representation of the token
    for (int i = 0; i < sizeof(keywords) / sizeof(pair); i++) {
        if (keywords[i].value == token) {
            return keywords[i].key;
        } 
    }
    return NULL;
}
