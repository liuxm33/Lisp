#include "nuclei.h"

#ifndef TEST
int main(int argc, char* argv[])
{
    var varTable[26]; // for 26 letters from A-Z in alphabet.
    memset(&varTable, 0, sizeof(var)*26);
    char tokens[MAXTOK][MAXLEN] = {{0}};
    int tokenLine[MAXTOK] = {0}; // for error reporting.

    FILE* fp = readFile(argc, argv);
    lexer(fp, tokens, tokenLine);
    fclose(fp);
    
    int tokenPos = 0; //token position
    //parsing programs
    int pn = 1; //program number
    int len = (int)strlen(*(tokens+tokenPos));
    while(len != 0){
        prog(tokens, &tokenPos, varTable);
        printf("Program %i Parsed OK\n", pn);
        pn++;
        len = (int)strlen(*(tokens+tokenPos));
    }
    //free variables in varTable
    #ifdef INTERP
    for(int i = 0; i < 26; i++){
        lisp_free(& varTable[i].data);
    }
    #endif
    return 0;
}
#endif

FILE* readFile(int argc, char* argv[])
{
    if(argc != 2){
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    FILE* fp = fopen(argv[1], "r");
    if(!fp){
        fprintf(stderr, "Cannot read from %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    return fp;
}

void lexer(FILE* fp, char tokens[MAXTOK][MAXLEN], int tokenLine[MAXTOK])
{
    int tokenPos = 0;
    int tokenLetter = 0;
    int line = 1;
    char tmpStr[MAXLEN];

    while(fgets(tmpStr, MAXLEN, fp)){
        int tmpStrPos = 0; 
        _lexerSkip(tmpStr, &tmpStrPos);

        int len = (int)strlen(tmpStr);
        while(tmpStrPos < len){    
            if(tmpStr[tmpStrPos] == '\''){
                _lexerQuote(tmpStr, &tmpStrPos, tokens, &tokenPos, &tokenLetter, '\'');
                tokenLine[tokenPos-1] = line;
            }
            else if(tmpStr[tmpStrPos] == '\"'){
                _lexerQuote(tmpStr, &tmpStrPos, tokens, &tokenPos, &tokenLetter, '\"');
                tokenLine[tokenPos-1] = line;
            }
            else if(tmpStr[tmpStrPos] == '(' || tmpStr[tmpStrPos] == ')'){
                _lexerBrace(tmpStr, &tmpStrPos, tokens, &tokenPos, &tokenLetter);
                tokenLine[tokenPos-1] = line;
            }
            else if(tmpStr[tmpStrPos] == SPACE || tmpStr[tmpStrPos] == '\n'){
                _lexerSkip(tmpStr, &tmpStrPos);
            }
            else if(tmpStr[tmpStrPos] == '#'){
                tmpStrPos = len;
            }
            else{
                _lexerWrds(tmpStr, &tmpStrPos, tokens, &tokenPos, &tokenLetter);
                tokenLine[tokenPos-1] = line;
            }
        }
        line ++;
    }
}

/*****************************************************************/
/*                 Auxiliary Functions of Lexer                  */
/*****************************************************************/

void _lexerWrds(char tmpStr[MAXLEN], int* tmpStrPos, char tokens[MAXTOK][MAXLEN], int* tokenPos, int* tokenLetter)
{
    int len = (int)strlen(tmpStr);
    while(*tmpStrPos < len){
        tokens[*tokenPos][*tokenLetter] = tmpStr[*tmpStrPos];
        NEXT(*tmpStrPos);
        NEXT(*tokenLetter);
        if(tmpStr[*tmpStrPos] == SPACE || tmpStr[*tmpStrPos] == '\n' \
           || tmpStr[*tmpStrPos] == '(' || tmpStr[*tmpStrPos] == ')' \
           || tmpStr[*tmpStrPos] == '\''){
            tokens[*tokenPos][*tokenLetter] = '\0';
            _nextToken(tokenPos, tokenLetter);
            return;
        }
    }
}

void _lexerBrace(char tmpStr[MAXLEN], int* tmpStrPos, char tokens[MAXTOK][MAXLEN], int* tokenPos, int* tokenLetter)
{
        tokens[*tokenPos][*tokenLetter] = tmpStr[*tmpStrPos];
        tokens[*tokenPos][*tokenLetter + 1] = '\0';
        NEXT(*tmpStrPos);
        _nextToken(tokenPos, tokenLetter);
}

// Find a pair of quota, until the end of the line.
void _lexerQuote(char tmpStr[MAXLEN], int* tmpStrPos, char tokens[MAXTOK][MAXLEN], int* tokenPos, int* tokenLetter, char quotaType)
{
    tokens[*tokenPos][*tokenLetter] = tmpStr[*tmpStrPos];
    NEXT(*tokenLetter);
    NEXT(*tmpStrPos);

    int len = (int)strlen(tmpStr);
    while(*tmpStrPos < len){
        if(tmpStr[*tmpStrPos] == '\n'){
            tokens[*tokenPos][*tokenLetter] = '\0'; //Didn't find another quota. then next line.
            NEXT(*tmpStrPos);
            _nextToken(tokenPos, tokenLetter);
            return; // This token would be an invalid one.
        }

        tokens[*tokenPos][*tokenLetter] = tmpStr[*tmpStrPos];

        if(tmpStr[*tmpStrPos] == quotaType){ //Find another quota.
            tokens[*tokenPos][*tokenLetter+1] = '\0';
            NEXT(*tmpStrPos);
            _nextToken(tokenPos, tokenLetter);
            return;
        }
        NEXT(*tokenLetter);
        NEXT(*tmpStrPos);
    }
}

void _nextToken(int* tokenPos, int* tokenLetter)
{
    *tokenLetter = 0;
    NEXT(*tokenPos);
}

//skip space and newline character
void _lexerSkip(char* tmpStr, int* tmpStrPos) 
{
    int len = (int)strlen(tmpStr);
    while(*tmpStrPos < len && \
         (tmpStr[*tmpStrPos] == SPACE || tmpStr[*tmpStrPos] == '\n')){
        NEXT(*tmpStrPos);
    }
}

/*****************************************************************/
/*                      Terminal Functions                       */
/*****************************************************************/

void _bdcheck(int* tokenPos)
{
    if(*tokenPos >= MAXTOK -1){ // -1 is for NEXT(*tokenPos)
        on_error("The program is too long\n");
    }
}

bool isKeyword(char tokens[MAXTOK][MAXLEN], int* tokenPos, const char* keywordName)
{
    _bdcheck(tokenPos);
    if(strcmp(*(tokens + *tokenPos), keywordName) == 0){
        NEXT(*tokenPos);
        return true;
    }
    return false;
}

bool isVar(char tokens[MAXTOK][MAXLEN], int* tokenPos)
{
    _bdcheck(tokenPos);
    int len = (int)strlen(*(tokens + *tokenPos));
    if(len == 1){ //letter A-Z
        if(isupper(tokens[*tokenPos][0])){
            NEXT(*tokenPos);
            return true;
        }
    }
    return false;
}

bool isLiteral(char tokens[MAXTOK][MAXLEN], int* tokenPos) 
{ 
    _bdcheck(tokenPos);
    int len = (int)strlen(*(tokens + *tokenPos));
    if(tokens[*tokenPos][0] == '\''){ //the first quote
        for(int n = 1; n < len ; n++){
            if(tokens[*tokenPos][n] == '\''){ //the second quote
                NEXT(*tokenPos);
                return true;
            }
        }
        
    }
    return false;
}

bool isString(char tokens[MAXTOK][MAXLEN], int* tokenPos) 
{
    _bdcheck(tokenPos);
    int len = (int)strlen(*(tokens + *tokenPos));
    if(tokens[*tokenPos][0] == '\"'){ //the first quote
        for(int n = 1; n < len ; n++){
            if(tokens[*tokenPos][n] == '\"'){ //the second quote
                NEXT(*tokenPos);
                return true;
            }
        }
    }
    return false;
}

/*****************************************************************/
/*               Auxiliary Functions of Interpreter              */
/*****************************************************************/

void _undeclared_check(char tokens[MAXTOK][MAXLEN], int* tokenPos, var tmpVar)
{
   if(tmpVar.state != exist && tmpVar.srce == frmVar){ 
        fprintf(stderr, "Use of undeclared identifier '%c'\n", tokens[*tokenPos-1][0]);
        exit(EXIT_FAILURE);
    }
}

void _tmpVar_free(var* tmpVar)
{
    if(tmpVar->srce == new){
        lisp_free(&tmpVar->data);
    }
}

void _skipStatmts(char tokens[MAXTOK][MAXLEN], int* tokenPos)
{
    int brackCnt = 1; 
    do{
        if(isKeyword(tokens, tokenPos, "(")){
            brackCnt ++;     
        }
        else if(isKeyword(tokens, tokenPos, ")")){
            brackCnt --;
        }else{
            NEXT(*tokenPos);
        }
    }while(brackCnt != 0); 
}

int _getLine(int* tokenPos, int tokenLine[MAXTOK])
{
    return tokenLine[*tokenPos];
}

/*****************************************************************/
/*                    Nonterminal Functions                      */
/*****************************************************************/

var list(char tokens[MAXTOK][MAXLEN], int* tokenPos, var* varTable)
{
    var tmpVar; //store return value of list function.
    memset(&tmpVar, 0, sizeof(var));

    if(isVar(tokens, tokenPos)){
        #ifdef INTERP
        // Copy the value in the variable table.
        tmpVar.srce = frmVar;
        // One token position back to get the variable name.
        tmpVar.state = varTable[tokens[*tokenPos-1][0]-'A'].state; 
        tmpVar.data = varTable[tokens[*tokenPos-1][0]-'A'].data; 
        #endif
        return tmpVar;
    }
    else if(isLiteral(tokens, tokenPos)){
        #ifdef INTERP
        char tmpStr[MAXLEN];
        strcpy(tmpStr, *(tokens + *tokenPos -1)); // One token position back to get the literal.

        int len = (int)strlen(tmpStr);
        tmpStr[len - 1] = '\0'; //take away the last quote.
        tmpVar.data = lisp_fromstring(tmpStr+1);//take away the first quote.
        #endif
        return tmpVar;
    }
    else if(strcmp(*(tokens + *tokenPos), "NIL") == 0 || strcmp(*(tokens + *tokenPos), "NULL") == 0){
        NEXT(*tokenPos); //No terminal function, so we need to move the token position manually.
        #ifdef INTERP
        //tmpVar.srce = new; //NULL is a special lisp.
        tmpVar.data = NULL;
        #endif
        return tmpVar;
    }
    else if(isKeyword(tokens, tokenPos, "(")){
        retFunc(tokens, tokenPos, &tmpVar, varTable);
        if( ! isKeyword(tokens, tokenPos, ")")){
            on_error("expected ')' of your retFunc function");
        }
        return tmpVar;
    }
    else{
        fprintf(stderr, "Not a valid LIST\n");
        exit(EXIT_FAILURE);
    }
} 

/************************** IO Functions *************************/
// <SET> ::= "SET" <VAR> <LIST>
bool setFunc(char tokens[MAXTOK][MAXLEN], int* tokenPos, var* varTable)
{
    if(isKeyword(tokens, tokenPos, "SET")){ // SET
        if(isVar(tokens, tokenPos)){ // <VAR>
            #ifdef INTERP
            // One token position back to get the variable name.
            int varIndex = tokens[*tokenPos-1][0]-'A';
            // <LIST>  
            var tmpVar = list(tokens, tokenPos, varTable);   
            varTable[varIndex].state = exist; 
            //Free the value that was here because of the posibility of redefinition
            lisp_free(&varTable[varIndex].data);
            varTable[varIndex].data = lisp_copy(tmpVar.data);
            varTable[varIndex].srce = tmpVar.srce;
            //free tmpVar because we have deep copied the value.
            _tmpVar_free(&tmpVar); 
            #else
            list(tokens, tokenPos, varTable); 
            #endif
            return true;
        }else{
            on_error("expected a variable name after SET");
        }
    }
    return false;
}
//<PRINT> ::= "PRINT" <LIST> | "PRINT" <STRING>
bool printFunc(char tokens[MAXTOK][MAXLEN], int* tokenPos, var* varTable)
{
    if(isKeyword(tokens, tokenPos, "PRINT")){ // PRINT
        if(isString(tokens, tokenPos)){ // <STRING>
            #ifdef INTERP
            char tmpStr[MAXLEN];
            strcpy(tmpStr, *(tokens + *tokenPos - 1));
            int len = strlen(tmpStr);
            //take away the last quote.
            tmpStr[len - 1] = '\0'; 
            printf("%s\n", tmpStr+1); //take away the first quote. print out.
            #endif
            return true;
        }
        else{
            #ifdef INTERP
            var tmpVar = list(tokens, tokenPos, varTable); // <LIST>
            char tmpStr[MAXLEN];
            _undeclared_check(tokens, tokenPos, tmpVar);
            lisp_tostring(tmpVar.data, tmpStr);
            printf("%s\n", tmpStr); //print out
            _tmpVar_free(&tmpVar);
            #else
            list(tokens, tokenPos, varTable);
            #endif
            return true;
        }
    }
    return false;
}

/************************** RET Functions *************************/
bool retFunc(char tokens[MAXTOK][MAXLEN], int* tokenPos, var* rstVar, var* varTable)
{
    if(listFunc(tokens, tokenPos, rstVar, varTable) || \
       intFunc(tokens, tokenPos, rstVar,varTable) || boolFunc(tokens, tokenPos, rstVar,varTable)){
        return true; 
    }
    return false;
}

bool listFunc(char tokens[MAXTOK][MAXLEN], int* tokenPos, var* rstVar, var* varTable)
{
    if(isKeyword(tokens, tokenPos, "CAR")){
        #ifdef INTERP
        var tmpVar = list(tokens, tokenPos, varTable);
        _undeclared_check(tokens, tokenPos, tmpVar);
        rstVar->data = lisp_copy(lisp_car(tmpVar.data));//deep copy
        _tmpVar_free(&tmpVar);
        #else
        list(tokens, tokenPos, varTable);
        UNUSED_VARIABLE(rstVar);
        #endif
        return true;
    }
    else if(isKeyword(tokens, tokenPos, "CDR")){
        #ifdef INTERP
        var tmpVar = list(tokens, tokenPos, varTable);
        _undeclared_check(tokens, tokenPos, tmpVar);
        rstVar->data = lisp_copy(lisp_cdr(tmpVar.data));
        _tmpVar_free(&tmpVar);
        #else
        list(tokens, tokenPos, varTable);
        #endif
        return true;
    }
    else if(isKeyword(tokens, tokenPos, "CONS")){
        #ifdef INTERP
        var tmpVar1 = list(tokens, tokenPos, varTable);
        _undeclared_check(tokens, tokenPos, tmpVar1);
        var tmpVar2 = list(tokens, tokenPos, varTable);
        _undeclared_check(tokens, tokenPos, tmpVar2);

        rstVar->data = (lisp*)ncalloc(1, sizeof(lisp));
        rstVar->data->car = lisp_copy(tmpVar1.data);
        rstVar->data->cdr = lisp_copy(tmpVar2.data);
        _tmpVar_free(&tmpVar1);
        _tmpVar_free(&tmpVar2); 
        
        #else
        list(tokens, tokenPos, varTable);
        list(tokens, tokenPos, varTable);
        #endif
        return true;
    }
    return false;
}

//return an atom with value of the integer;
bool intFunc(char tokens[MAXTOK][MAXLEN], int* tokenPos,var* rstVar, var* varTable) 
{
    if(isKeyword(tokens, tokenPos, "PLUS")){
        #ifdef INTERP
        var tmpVar1 = list(tokens, tokenPos, varTable);
        _undeclared_check(tokens, tokenPos, tmpVar1);
        var tmpVar2 = list(tokens, tokenPos, varTable);
        _undeclared_check(tokens, tokenPos, tmpVar2);
        if(lisp_isatomic(tmpVar1.data) && lisp_isatomic(tmpVar2.data)){
            rstVar->data = lisp_atom(lisp_getval(tmpVar1.data) + lisp_getval(tmpVar2.data)); //malloc
            _tmpVar_free(&tmpVar1);
            _tmpVar_free(&tmpVar2);
        }else{
            on_error("PLUS function: needed exe atom !\n");
        }
        #else
        list(tokens, tokenPos, varTable);
        list(tokens, tokenPos, varTable);
        #endif
        return true;
    }
    else if(isKeyword(tokens, tokenPos, "LENGTH")){
        #ifdef INTERP
        var tmpVar = list(tokens, tokenPos, varTable);
        _undeclared_check(tokens, tokenPos, tmpVar);
        rstVar->data = lisp_atom(lisp_length(tmpVar.data)); 

        _tmpVar_free(&tmpVar);
        #else
        list(tokens, tokenPos, varTable);
        UNUSED_VARIABLE(rstVar);
        #endif
        return true;
    }
    return false;    
}

//return an atom with value 1 or 0;
bool boolFunc(char tokens[MAXTOK][MAXLEN], int* tokenPos, var* rstVar, var* varTable)
{
    flag f = none;
    if(isKeyword(tokens, tokenPos, "LESS")){ f = less;}
    else if(isKeyword(tokens, tokenPos, "GREATER")){ f = greater;}
    else if(isKeyword(tokens, tokenPos, "EQUAL")){ f = equal;}
    else{ return false; }

    #ifdef INTERP
    var tmpVar1 = list(tokens, tokenPos, varTable);
    _undeclared_check(tokens, tokenPos, tmpVar1);
    var tmpVar2 = list(tokens, tokenPos, varTable);
    _undeclared_check(tokens, tokenPos, tmpVar2);
    if(lisp_isatomic(tmpVar1.data) && lisp_isatomic(tmpVar2.data)){
        if(f == less && lisp_getval(tmpVar1.data) < lisp_getval(tmpVar2.data)){
            rstVar->data = lisp_atom(1);
        }
        else if(f == greater && lisp_getval(tmpVar1.data) > lisp_getval(tmpVar2.data)){
            rstVar->data = lisp_atom(1);
        }
        else if(f == equal && lisp_getval(tmpVar1.data) == lisp_getval(tmpVar2.data)){
            rstVar->data = lisp_atom(1);
        }  
        else{
            rstVar->data = lisp_atom(0);
        }
        _tmpVar_free(&tmpVar1);
        _tmpVar_free(&tmpVar2);
    }
    else{
        on_error("Bool function: needed exe atom !");
    }
    #else
    list(tokens, tokenPos, varTable);
    list(tokens, tokenPos, varTable);
    UNUSED_VARIABLE(rstVar);
    UNUSED_VARIABLE(f);
    #endif
    return true;
}

//<IF>  ::= "IF" "(" <BOOLFUNC> ")" "(" <INSTRCTS> "(" <INSTRCTS>
bool ifFunc(char tokens[MAXTOK][MAXLEN], int* tokenPos, var* varTable)
{
    if(isKeyword(tokens, tokenPos, "IF")){ 
        if(isKeyword(tokens, tokenPos, "(")){
            var tmpVar;
            memset (&tmpVar, 0, sizeof(tmpVar));
            boolFunc(tokens, tokenPos, &tmpVar, varTable);
            #ifdef INTERP
            flag f = none;
            if(lisp_getval(tmpVar.data)){ //get atom value
                f = iftrue;
            }
            #endif
            _tmpVar_free(&tmpVar);
            if(isKeyword(tokens, tokenPos, ")")){
            // if - true part
                if(isKeyword(tokens, tokenPos, "(")){
                    #ifdef INTERP
                    if(f == iftrue){
                       instrcts(tokens, tokenPos, varTable);
                    }else{
                        _skipStatmts(tokens, tokenPos);
                    }
                    #else
                    instrcts(tokens, tokenPos, varTable);
                    #endif
            // if - false part
                    if(isKeyword(tokens, tokenPos, "(")){
                        #ifdef INTERP
                        if(f == iftrue){
                            _skipStatmts(tokens, tokenPos);
                            return true;
                        }
                        #endif
                        instrcts(tokens, tokenPos, varTable);
                        return true;
                    }else{
                        fprintf(stderr, "expected an else-branch in an if function\n");
                        exit(EXIT_FAILURE);
                    }
                }else{
                    fprintf(stderr, "expected an if-branch in an if function\n");
                    exit(EXIT_FAILURE);
                }
            }else{
                fprintf(stderr, "expected ')' of an if function\n");
                exit(EXIT_FAILURE);
            }
        }else{
            fprintf(stderr, "expected '(' of an if function\n");
            exit(EXIT_FAILURE);
        }
    }
    return false;
}

//<LOOP> ::= "WHILE""(" <BOOLFUNC> ")" "(" <INSTRCTS>
bool loopFunc(char tokens[MAXTOK][MAXLEN], int* tokenPos, var* varTable, int* link, bool* boolVar)
{
    *link = *tokenPos - 1; //back to the start of loop -> the '(' of the loop.
    if(isKeyword(tokens, tokenPos, "WHILE")){ 
        if(isKeyword(tokens, tokenPos, "(")){
            var tmpVar;
            memset (&tmpVar, 0, sizeof(tmpVar));
            boolFunc(tokens, tokenPos, &tmpVar, varTable);
            #ifdef INTERP
            flag f = none;
            if(lisp_getval(tmpVar.data)){ //get bool value from an atom.
                f = iftrue;
            }
            #endif
            _tmpVar_free(&tmpVar);
            if(isKeyword(tokens, tokenPos, ")")){
            //loop body
                if(isKeyword(tokens, tokenPos, "(")){
                    #ifdef INTERP
                    if(f == iftrue){
                       *boolVar = true;
                        instrcts(tokens, tokenPos, varTable);
                    }else{
                        *boolVar = false;
                        _skipStatmts(tokens, tokenPos);
                    }
                    #else
                    instrcts(tokens, tokenPos, varTable);
                    UNUSED_VARIABLE(boolVar);
                    #endif
                    return true;
                }else{
                    on_error("expected expression in a loop function");
                }
            }else{
                on_error("expected ')' of a loop function");
            }
        }else{
            on_error("expected '(' of a loop function");
        }
    }
    return false;
}

void instrct(char tokens[MAXTOK][MAXLEN], int* tokenPos, var* varTable)
{
    var tmpVar;
    memset (&tmpVar, 0, sizeof(tmpVar));
    int link = 0;
    bool boolVar = false;

    if(isKeyword(tokens, tokenPos, "(")){
        if(setFunc(tokens, tokenPos, varTable) || printFunc(tokens, tokenPos, varTable) ||\
           retFunc(tokens, tokenPos, &tmpVar, varTable) || ifFunc(tokens, tokenPos, varTable)){
            if(isKeyword(tokens, tokenPos, ")")){
                _tmpVar_free(&tmpVar);
            }else{
                on_error("expected ')' of a function");
            }
        }
        else if(loopFunc(tokens, tokenPos, varTable, &link, &boolVar)){
            if(isKeyword(tokens, tokenPos, ")")){ 
                if(boolVar == true){
                *tokenPos = link;
                }
                _tmpVar_free(&tmpVar);
            }else{
                on_error("expected ')' of a loop function");
            }
        }else{
            on_error("expected a function name ?");
        }
    }else{
        on_error("expected '(' of a function");
    }  
}

void instrcts(char tokens[MAXTOK][MAXLEN], int* tokenPos, var* varTable)
{
    if(isKeyword(tokens, tokenPos, ")")){
    }
    else{
        instrct(tokens, tokenPos, varTable);
        instrcts(tokens, tokenPos, varTable);
    }
}

void prog(char tokens[MAXTOK][MAXLEN], int* tokenPos, var* varTable)
{
    if(isKeyword(tokens, tokenPos, "(")){
        instrcts(tokens, tokenPos, varTable);
    }
    else{
        on_error("expected '(' of a program");
    }
}
