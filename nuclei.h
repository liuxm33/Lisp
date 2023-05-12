#pragma once 

#include "specific.h"

#define MAXTOK 1000
#define MAXLEN 1000
#define NIL NULL
#define SPACE ' '
#define NEXT(i) (i) = (i) + 1
#define UNUSED_VARIABLE(x) ((void)(x))

typedef enum {notExist, exist} sta;
typedef enum {new, frmVar} source;
typedef enum {none, less, greater, equal, iftrue} flag;

typedef struct var{ 
    sta state; //Default value: notExist. 
    source srce; //Default value: new -> need to be freed in time.
    lisp* data; //Integer needs to be transfered to an atom.
}var;

FILE* readFile(int argc, char* argv[]);

void lexer(FILE* fp, char tokens[MAXTOK][MAXLEN], int tokenLine[MAXTOK]);
void _lexerQuote(char tmpStr[MAXLEN], int* tmpStrPos, char tokens[MAXTOK][MAXLEN], int* tokenPos, int* tokenLetter, char quotaType);
void _lexerBrace(char tmpStr[MAXLEN], int* tmpStrPos, char tokens[MAXTOK][MAXLEN], int* tokenPos, int* tokenLetter);
void _lexerWrds(char tmpStr[MAXLEN], int* tmpStrPos, char tokens[MAXTOK][MAXLEN], int* tokenPos, int* tokenLetter);
void _lexerSkip(char* tmpStr, int* tokenLetter);//skip space and newline characters
void _nextToken(int* tokenPos, int* tokenLetter);

//terminals
bool isKeyword(char tokens[MAXTOK][MAXLEN], int* tokenPos, const char* keywordName);
bool isVar(char tokens[MAXTOK][MAXLEN], int* tokenPos); 
bool isLiteral(char tokens[MAXTOK][MAXLEN], int* tokenPos);
bool isString(char tokens[MAXTOK][MAXLEN], int* tokenPos);  

//nonterminals (need to be interpreted）
var list(char tokens[MAXTOK][MAXLEN], int* tokenPos, var* varTable); 
bool retFunc(char tokens[MAXTOK][MAXLEN], int* tokenPos, var* rstVar, var* varTable);
bool listFunc(char tokens[MAXTOK][MAXLEN], int* tokenPos,var* rstVar, var* varTable);
bool intFunc(char tokens[MAXTOK][MAXLEN], int* tokenPos,var* rstVar, var* varTable);
bool boolFunc(char tokens[MAXTOK][MAXLEN], int* tokenPos,var* rstVar, var* varTable);
//IOFUNC
bool setFunc(char tokens[MAXTOK][MAXLEN], int* tokenPos, var* varTable);
bool printFunc(char tokens[MAXTOK][MAXLEN], int* tokenPos, var* varTable);

bool ifFunc(char tokens[MAXTOK][MAXLEN], int* tokenPos, var* varTable);
bool loopFunc(char tokens[MAXTOK][MAXLEN], int* tokenPos, var* varTable, int* link, bool* boolVar);

void instrct(char tokens[MAXTOK][MAXLEN], int* tokenPos, var* varTable);
void instrcts(char tokens[MAXTOK][MAXLEN], int* tokenPos, var* varTable);
void prog(char tokens[MAXTOK][MAXLEN], int* tokenPos, var* varTable);

void _undeclared_check(char tokens[MAXTOK][MAXLEN], int* tokenPos, var tmpVar);
void _tmpVar_free(var* tmpVar);
void _skipStatmts(char tokens[MAXTOK][MAXLEN], int* tokenPos);
void _bdcheck(int* tokenPos);
