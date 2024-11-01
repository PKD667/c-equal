#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"

#include "../include/parser.h"
#include "../include/assembly.h"
#include "../include/cutils.h"


//main function
int main(int argc,char** argv)
{ 

    DEBUG = 4;
    //DEBUG_FILE = "parse.c";
    //DEBUG_FN = "parse_call";

    //check if there is a file to compile
    if(argc < 2)
    {
        printf("No file given to compile\n");
        return 1;
    }
    // check if the file exists
    char* path = argv[1];
    if (access(path, F_OK) == -1) {
        printf("File does not exist\n");
        return 1;
    }

    // read file
    FILE* file = fopen(path, "r");
    if(file == NULL)
    {
        printf("Error reading file\n");
        return 1;
    }
    // get file size
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // read file into buffer
    char* buffer = malloc(size + 1);
    fread(buffer, 1, size, file);
    buffer[size] = '\0';

    fclose(file);

    byte** litterals;
    char** ids;
    int* tokens = calloc(MAX_TOKENS, sizeof(int));


    int ret = tokenize(buffer,(int**)&tokens, &litterals,&ids);
    if (ret == -1) {
        printf("Tokenization failed\n");
        return 1;
    }

    int l_count = 0;
 
    for (int i = 0;litterals[i]; i++) {
        l_count++;
    }
    printf("\nEncoding litterals: %d\n", l_count);
    char* encoded_literals;

    printf("Litterals:\n");
    for (int i = 0; litterals[i]; i++) {
        printf("'");
        for (int j = 0; litterals[i][j]; j++) {
            printf("%c",litterals[i][j]);
        }
        printf("'\n");
    }

    printf("\n");
    printf("Identifiers: \n");
    for (int i = 0; ids[i]; i++) {
        printf("%s\n", ids[i]);
    }


    // print the tokens
    for (int i = 0; tokens[i] != T_EOF; i++) {
        printf("%s ", get_token_string(tokens[i]));
    }

    struct ASTblock* tree = parsefile(tokens);
    if (!tree) {
        printf("Parsing failed\n");
        return 1;
    }

    printf("Parsing successful\n");

    struct AST* t = malloc(sizeof(struct AST));
    t->type = AST_BLOCK;
    t->block = tree;
    visualize_ast(*t, litterals, ids);

    struct ASM_FIlE* assembly = assemblify(tree, litterals, ids);


    return 0;
}
