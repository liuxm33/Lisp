#include "nuclei.h"

void test(void);
void _unittst_terminal(void);
void _unittst_list(void);
void _unittst_listFunc(void);
void _unittst_intFunc(void);
void _unittst_boolFunc(void);
void _unittst_IOFunc(void);
void _unittst_ifFunc(void);
void _unittst_loopFunc(void);
void _unittst_lexer(void);

int main(int argc, char* argv[])
{
    printf("unit test begin....\n");
    _unittst_terminal();
    _unittst_list();
    _unittst_listFunc();
    _unittst_intFunc();
    _unittst_boolFunc();
    _unittst_IOFunc();
    _unittst_ifFunc();
    _unittst_loopFunc();
    _unittst_lexer();
    printf("unit test end\n");
    UNUSED_VARIABLE(argc);
    UNUSED_VARIABLE(argv);
    
    var varTable[26]; 
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

    for(int i = 0; i < 26; i++){
        lisp_free(& varTable[i].data);
    }
    return 0;
}

void test(void)
{
    printf("black-box testing begin\n");
    printf("black-box testing end\n");
}

void _unittst_terminal(void)
{
    //bool isKeyword(char tokens[MAXTOK][MAXLEN], int* tokenPos, const char* keywordName)
    char tokens[MAXTOK][MAXLEN]= {"(", ")", "wrd", "\'"};
    int tokenPos = 2;
    assert(isKeyword(tokens, &tokenPos, "wrd") == true);
    assert(isKeyword(tokens, &tokenPos, "(") == false);
    tokenPos = 3;
    assert(isKeyword(tokens, &tokenPos, "\'") == true);

    //bool isVar(char tokens[MAXTOK][MAXLEN], int* tokenPos)
    char tokens2[MAXTOK][MAXLEN]= {"123", "'A'", "...", "AB", "H", "Z", "g"};
    tokenPos = 3;
    assert(isVar(tokens2, &tokenPos) == false);
    tokenPos = 5;
    assert(isVar(tokens2, &tokenPos) == true);
    tokenPos = 5;
    assert(isVar(tokens2, &tokenPos) == true);
    tokenPos = 2;
    assert(isVar(tokens2, &tokenPos) == false);
    tokenPos = 1;
    assert(isVar(tokens2, &tokenPos) == false);
    tokenPos = 0;
    assert(isVar(tokens2, &tokenPos) == false);
    tokenPos = 6;
    assert(isVar(tokens2, &tokenPos) == false);
    //bool isLiteral(char tokens[MAXTOK][MAXLEN], int* tokenPos)
    char tokens3[MAXTOK][MAXLEN]= {"123'", "'Abc", "''", "AB", "'a b c'"};
    tokenPos = 0;
    assert(isLiteral(tokens3, &tokenPos) == false);
    tokenPos = 1;
    assert(isLiteral(tokens3, &tokenPos) == false);
    tokenPos = 2;
    assert(isLiteral(tokens3, &tokenPos) == true);
    tokenPos = 3;
    assert(isLiteral(tokens3, &tokenPos) == false);
    tokenPos = 4;
    assert(isLiteral(tokens3, &tokenPos) == true);
    //bool isString(char tokens[MAXTOK][MAXLEN], int* tokenPos) 
    char tokens4[MAXTOK][MAXLEN]= {"\"", "ABC\"", "\"\"", "\"AB\"", "\"a b c\""};
    tokenPos = 0;
    assert(isString(tokens4, &tokenPos) == false);
    tokenPos = 1;
    assert(isString(tokens4, &tokenPos) == false);
    tokenPos = 2;
    assert(isString(tokens4, &tokenPos) == true);
    tokenPos = 3;
    assert(isString(tokens4, &tokenPos) == true);
    tokenPos = 4;
    assert(isString(tokens4, &tokenPos) == true);
}

void _unittst_list(void)
{
    char tokens[MAXTOK][MAXLEN]= {"C", "'123'", "NIL", \
                                  "(","PLUS", "'1'", "'1'", ")"};
    var varTable[26]; 
    memset(&varTable, 0, sizeof(var)*26);
    #ifdef INTERP
    varTable[2].state = exist;
    varTable[2].srce = frmVar;
    varTable[2].data = lisp_fromstring("(1 2)");
    #endif

    int tokenPos = 0;
    var rstVar = list(tokens, &tokenPos, varTable);
    #ifdef INTERP
    assert(rstVar.state == exist );
    assert(rstVar.srce == frmVar);
    char str[MAXLEN];
    lisp_tostring(rstVar.data, str);
    assert(strcmp(str, "(1 2)") == 0);
    lisp_free(&rstVar.data);
    #endif
 
    tokenPos = 1;
    rstVar = list(tokens, &tokenPos, varTable);
    #ifdef INTERP
    lisp_tostring(rstVar.data, str);
    assert(strcmp(str, "123") == 0);
    lisp_free(&rstVar.data);
    #endif

    tokenPos = 2;
    rstVar = list(tokens, &tokenPos, varTable);
    #ifdef INTERP
    lisp_tostring(rstVar.data, str);
    assert(strcmp(str, "()") == 0);
    lisp_free(&rstVar.data);
    #endif

    //after finishing retFunc, test (PLUS '1' '1')
    tokenPos = 3;
    rstVar = list(tokens, &tokenPos, varTable);
    #ifdef INTERP
    lisp_tostring(rstVar.data, str);
    assert(strcmp(str, "2") == 0);
    lisp_free(&rstVar.data);
    #else
    UNUSED_VARIABLE(rstVar);
    #endif
}
void _unittst_listFunc(void)
{
     char tokens[MAXTOK][MAXLEN]= {"CAR", "'(1 2 3)'",\
                                  "CDR", "'(1 2)'",\
                                  "CONS","A", "B", \
                                  "CONS","A", "(", "CDR", "'(3 4)'", ")"};
    var varTable[26]; 
    memset(&varTable, 0, sizeof(var)*26);
    #ifdef INTERP
    varTable[0].state = exist;
    varTable[0].data = lisp_fromstring("(1 2)");
    varTable[1].state = exist;
    varTable[1].data = lisp_fromstring("(3 4)");
    #endif
    
    int tokenPos = 0;
    var rstVar;
    memset(&rstVar, 0, sizeof(rstVar));
    assert(listFunc(tokens, &tokenPos, &rstVar, varTable) == true );
    #ifdef INTERP
    char str[MAXLEN];
    lisp_tostring(rstVar.data, str);
    assert(strcmp(str, "1") == 0);
    lisp_free(&rstVar.data);
    #endif

    tokenPos = 2;
    assert(listFunc(tokens, &tokenPos, &rstVar, varTable) == true );
    #ifdef INTERP
   
    lisp_tostring(rstVar.data, str);
    assert(strcmp(str, "(2)") == 0);
    lisp_free(&rstVar.data);
    #endif

    tokenPos = 4;
    assert(listFunc(tokens, &tokenPos, &rstVar, varTable) == true );
    #ifdef INTERP
    lisp_tostring(rstVar.data, str);
    assert(strcmp(str, "((1 2) 3 4)") == 0);
    lisp_free(&rstVar.data);
    #endif

    tokenPos = 7;
    assert(listFunc(tokens, &tokenPos, &rstVar, varTable) == true );
    #ifdef INTERP
    lisp_tostring(rstVar.data, str);
    assert(strcmp(str, "((1 2) 4)") == 0);
    lisp_free(&rstVar.data);
    lisp_free(&varTable[0].data);
    lisp_free(&varTable[1].data);
    #endif
   
}

void _unittst_intFunc(void)
{
    char tokens[MAXTOK][MAXLEN]= {"PLUS", "'1'","'1'",\
                                  "LENGTH", "'(1 2 3)'"};
    var varTable[26]; 
    memset(&varTable, 0, sizeof(var)*26);
    
    int tokenPos = 0;
    var rstVar;
    memset(&rstVar, 0, sizeof(rstVar));
    assert(intFunc(tokens, &tokenPos, &rstVar, varTable) == true );
    #ifdef INTERP
    char str[MAXLEN];
    lisp_tostring(rstVar.data, str);
    assert(strcmp(str, "2") == 0);
    lisp_free(&rstVar.data);
    #endif

    tokenPos = 3;
    assert(intFunc(tokens, &tokenPos, &rstVar, varTable) == true );
    #ifdef INTERP
    lisp_tostring(rstVar.data, str);
    assert(strcmp(str, "3") == 0);
    lisp_free(&rstVar.data);
    #endif
}
void _unittst_boolFunc(void)
{
    char tokens[MAXTOK][MAXLEN]= {"LESS", "'1'","'2'",\
                                  "GREATER", "'4'", "'3'",\
                                  "EQUAL", "'2'", "'2'"};
    var varTable[26]; 
    memset(&varTable, 0, sizeof(var)*26);
    
    int tokenPos = 0;
    var rstVar;
    memset(&rstVar, 0, sizeof(rstVar));
    assert(boolFunc(tokens, &tokenPos, &rstVar, varTable) == true );
    #ifdef INTERP
    char str[MAXLEN];
    lisp_tostring(rstVar.data, str);
    assert(strcmp(str, "1") == 0);
    lisp_free(&rstVar.data);
    #endif

    tokenPos = 3;
    assert(boolFunc(tokens, &tokenPos, &rstVar, varTable) == true );
    #ifdef INTERP
    lisp_tostring(rstVar.data, str);
    assert(strcmp(str, "1") == 0);
    lisp_free(&rstVar.data);
    #endif

    tokenPos = 6;
    assert(boolFunc(tokens, &tokenPos, &rstVar, varTable) == true );
    #ifdef INTERP
    lisp_tostring(rstVar.data, str);
    assert(strcmp(str, "1") == 0);
    lisp_free(&rstVar.data);
    #endif

}

void _unittst_IOFunc(void)
{
    char tokens[MAXTOK][MAXLEN]= {"SET", "A", "NIL", \
                                  "SET","B", "'(1 2)'",\
                                  "SET","C", "B", \
                                  "SET","D", "(", "CAR", "C", ")"};
    var varTable[26]; 
    memset(&varTable, 0, sizeof(var)*26);
    
    int tokenPos = 0;
    assert(setFunc(tokens, &tokenPos, varTable) == true );
    #ifdef INTERP
    assert(varTable[0].data == NULL);
    assert(varTable[0].state == exist);
    assert(varTable[0].srce == new);
    lisp_free(&varTable[0].data);
    #endif

    tokenPos = 3;
    assert(setFunc(tokens, &tokenPos, varTable) == true );
    #ifdef INTERP
    char str[MAXLEN];
    lisp_tostring(varTable[1].data, str);
    assert(strcmp(str, "(1 2)") == 0);
    #endif

    tokenPos = 6;
    assert(setFunc(tokens, &tokenPos, varTable) == true );
    #ifdef INTERP
    
    lisp_tostring(varTable[1].data, str);
    assert(strcmp(str, "(1 2)") == 0);
    #endif

    tokenPos = 9;
    assert(setFunc(tokens, &tokenPos, varTable) == true );
    #ifdef INTERP
  
    lisp_tostring(varTable[3].data, str);
    assert(strcmp(str, "1") == 0);
    lisp_free(&varTable[1].data);
    lisp_free(&varTable[2].data);
    lisp_free(&varTable[3].data);
    #endif
    #ifndef INTERP
    char tokens1[MAXTOK][MAXLEN]= {"PRINT", "A", \
                                  "PRINT","'(1 2)'",\
                                  "PRINT","NULL", \
                                  "PRINT", "(", "CAR", "C", ")"};
    tokenPos = 0;
    assert(printFunc(tokens1, &tokenPos, varTable) == true );
    tokenPos = 2;
    assert(printFunc(tokens1, &tokenPos, varTable) == true );
    tokenPos = 4;
    assert(printFunc(tokens1, &tokenPos, varTable) == true );
    tokenPos = 6;
    assert(printFunc(tokens1, &tokenPos, varTable) == true );
    #endif
}

void _unittst_ifFunc(void)
{
    char tokens1[MAXTOK][MAXLEN]= {"IF", "(","EQUAL","'1'","'1'",")", \
                                  "(","(","SET","A","'(1 2)'",")",")",\
                                  "(","(","SET","B","'(1 2)'",")",")"};

    var varTable[26]; 
    memset(&varTable, 0, sizeof(var)*26);
    
    int tokenPos = 0;
    assert(ifFunc(tokens1, &tokenPos, varTable) == true );
    #ifdef INTERP
    assert(varTable[0].data != NULL);
    assert(varTable[1].data == NULL);
    lisp_free(&varTable[0].data);
    lisp_free(&varTable[1].data);
    #endif

    char tokens2[MAXTOK][MAXLEN]= {"IF", "(","EQUAL","'2'","'1'",")", \
                                  "(","(","SET","A","'(1 2)'",")",")",\
                                  "(","(","SET","B","'(1 2)'",")",")",")"};
    tokenPos = 0;
    assert(ifFunc(tokens2, &tokenPos, varTable) == true );
    #ifdef INTERP
    assert(varTable[0].data == NULL);
    assert(varTable[1].data != NULL);
    lisp_free(&varTable[0].data);
    lisp_free(&varTable[1].data);
    #endif
}
void _unittst_loopFunc(void)
{
    char tokens1[MAXTOK][MAXLEN]= {"WHILE", "(","EQUAL","'1'","'1'",")", \
                                  "(","(","SET","A","'(1 2)'",")",")",")"};

    var varTable[26]; 
    memset(&varTable, 0, sizeof(var)*26);
    
    int tokenPos = 0;
    int link;
    bool boolVar;
    assert(loopFunc(tokens1, &tokenPos, varTable, &link, &boolVar) == true );
    #ifdef INTERP
    assert(varTable[0].data != NULL);
    lisp_free(&varTable[0].data);
    assert(boolVar == true);
    assert(link == -1);
    #endif

    char tokens2[MAXTOK][MAXLEN]= {"WHILE", "(","EQUAL","'2'","'1'",")", \
                                  "(","(","SET","A","'(1 2)'",")",")",")"};
    tokenPos = 0;                 
    assert(loopFunc(tokens2, &tokenPos, varTable, &link, &boolVar) == true );
    #ifdef INTERP
    assert(varTable[0].data == NULL);
    lisp_free(&varTable[0].data);
    assert(boolVar == false);
    assert(link == -1);
    #endif
}

void _unittst_lexer(void)
{
    char tmpStr1[MAXLEN] = " (  SET( A ') \n";
    char tokens[MAXTOK][MAXLEN] = {{0}};
    
    int tmpStrPos = 4;
    int tokenPos = 0;
    int tokenLetter = 0;
    _lexerWrds(tmpStr1, &tmpStrPos, tokens, &tokenPos, &tokenLetter);
    assert(tmpStrPos == 7);
    assert(tokenLetter == 0);
    assert(tokenPos == 1);
    assert(strcmp(tokens[0], "SET") == 0);

    tmpStrPos = 11;
    tokenPos = 0;
    tokenLetter = 0;
    _lexerQuote(tmpStr1, &tmpStrPos, tokens, &tokenPos, &tokenLetter, '\'');
    assert(tmpStrPos == 15);
    assert(tokenLetter == 0);
    assert(tokenPos == 1);
    assert(strcmp(tokens[0], "') ") == 0);

    char tmpStr2[MAXLEN] = " ( \"Hello\" ) \n";
    tokenPos = 0;
    tokenLetter = 0;
    tmpStrPos = 3;
    _lexerQuote(tmpStr2, &tmpStrPos, tokens, &tokenPos, &tokenLetter, '\"');
    assert(tmpStrPos == 10);
    assert(tokenLetter == 0);
    assert(tokenPos == 1);
    assert(strcmp(tokens[0], "\"Hello\"") == 0);

    tmpStrPos = 3;
    _lexerBrace(tmpStr2, &tmpStrPos, tokens, &tokenPos, &tokenLetter);

    tokenPos = 0;
    tokenLetter = 0;
    _nextToken(&tokenPos, &tokenLetter);
    assert(tokenPos == 1);
    assert(tokenLetter == 0);

    tmpStrPos = 2;
    _lexerSkip(tmpStr1, &tmpStrPos);
    assert(tmpStrPos == 4);
    tmpStrPos = 13;
    _lexerSkip(tmpStr1, &tmpStrPos);
    assert(tmpStrPos == 15);
}
