#pragma once
#include "hashtable.h"

enum t_token {
    //
    T_ERROR,
    // separators
    T_SEMICOLON, T_COMMA, T_COLON, T_DOT, T_LPAREN, T_RPAREN, T_LBRACE, T_RBRACE, T_LBRACKET, T_RBRACKET,
    // operators
        // simple operators
        T_PLUS, T_MINUS, T_STAR, T_SLASH, T_PERCENT, T_EQ, T_AND, T_OR, T_NOT,T_LSHIFT,T_RSHIFT,
        // compound operators
        T_NEQ, T_LT, T_GT, T_LTE, T_GTE,
        // assignment
        T_ASSIGN,
    // keywords
    // conditionals
    T_IF, T_ELSE, T_ELIF,
    // loops
    T_WHILE, T_FOR, 
    // statements
    T_RETURN, T_BREAK, T_CONTINUE,
    // literals
    T_LINT,T_LFLOAT, T_LSTRING,
    // type identifiers
    T_T32, T_T16, T_T8,
    // end of file
    T_EOF,
    // MISC
    T_UNKNOWN

};

#define MAX_TOKENS 1000
#define BUFFER_SIZE 512

#define MAX_IDENTIFIERS 512
#define MAX_IDENTIFIERS_LENGTH 128

#define MAX_LITTERALS 512
#define MAX_LITTERALS_LENGTH 512

// we'll use bytes when it's random stuff (mix of ints and strings)
#define byte unsigned char

int tokenize(char* str,int** tokens,byte*** litterals,char*** ids);


enum ASTType {
    AST_NULL, // so that AST_FN != 0
    AST_FN, // Function declare
    AST_VAR, // Variable declare
    AST_STMT, // Statements (not really you'll see)
    AST_OP, // Operation 
    AST_CONDITION, // Condition (Control Flow)
    AST_LOOP, // Loop (Control Flow)
    AST_BLOCK, // Block (bunch of AST branches)
    AST_VALUE,  // A value (litteral or var)
    AST_SPEC, // Type specification (size) [Might extend]
};

struct AST {
    enum ASTType type;
    union {
        struct ASTstmt* stmt;
        struct ASTop* op;
        struct ASTcond* condition;
        struct ASTblock* block;
        struct ASTvalue* value;
        struct ASTloop* loop;
        struct ASTspec* spec;
        struct ASTfn* fn;
        struct ASTvar* var;
    };
};

// statements
// actually not really statements in the C sense
// but I'm to lazy to change
struct ASTstmt {
    enum {
        AST_CALL, AST_RETURN, AST_BREAK, AST_CONTINUE,
    } tag;

    struct AST** args;
    int argc;
};

// function declaration
struct ASTfn {
    char* name;
    struct AST** args;
    int argc;
    struct ASTblock* body;
};

// variable declaration
struct ASTvar {
    // type
    struct AST* spec; // should be AST Spec
    struct AST* init; // Can be anything that returns a value I guess

    // Id we'll find in ASTvalues
    struct AST* id;
    

};

struct ASTop {
    enum {
        AST_ADD, AST_SUB, AST_MUL, AST_DIV, AST_MOD,
        AST_CMP,AST_CHLT,AST_CHGT, AST_CHLTE, AST_CHGTE,
        AST_AND,AST_OR,AST_NOT,AST_XOR,
        AST_SHL,AST_SHR,
        AST_ASSIGN,
    } tag;
    struct AST* left;
    struct AST* right;
};

struct ASTcond {
    enum {
        AST_IF, AST_ELIF, AST_ELSE,
    } tag;
    struct AST* condition;
    struct AST* body;
};

struct ASTloop {
    enum {
        AST_WHILE, AST_FOR,
    } tag;
    struct AST* condition;
    struct AST* body;
};

struct ASTblock {
    struct AST** childs;
    int size;

    struct AST* parent;
};

struct ASTvalue {
    enum {
        AST_LFLOAT, AST_LINT, AST_LSTRING,
        AST_ID,
    } tag;
    int v;
};

struct ASTspec {
    enum {
        AST_8, AST_16, AST_32,
    } tag;
};

// Main parser functions
struct ASTblock* parsefile(int* tokens);
struct AST* parse(int* tokens, int* index,int len);
struct AST* parse_stmt(int* tokens, int* index);
struct AST* parse_op(int* tokens, int* index);
struct AST* parse_opblock(int* tokens, int* index);
struct AST* parse_block(int* tokens, int* index);
// declare block
struct AST* parse_function_declare(int* tokens, int* index);
struct AST* parse_variable_declare(int* tokens, int* index);

struct AST* parse_call(int* tokens, int* index);
struct AST* parse_loop(int* tokens, int* index);
struct AST* parse_value(int* tokens, int* index);
struct AST* parse_condition(int* tokens, int* index);


// parseutils
int t_isop(int token);
int t_isopcomp(int token);
int t_iskeyword(int token);
int t_islit(int token);
int t_isvar(int token);
int t_istype(int token);
int t_issep(int token);
int t_iscond(int token);
int t_isloop(int token);
int t_isstmt(int token);

int get_laddr();

int skip_to(int* tokens, int* index, int token,int len);

typedef struct {
    // pattern expression
    int* exp;

    // Tag of the pattern (so it can be reused by other patterns)
    // negative int to not conflict with the token values
    int tag;

    struct AST* (*func)(int* tokens, int* index);

} pattern ;


// Pattern matching
pattern* match_pattern(int* tokens,int len);


// Utilites for debugging
char* get_token_string(int token);
char* get_pattern_string(int pattern_value);
void visualize_pattern(int* pattern);
void visualize_ast(struct AST ast,byte** litterals,char** ids);