#include "specific.h"

#define INVALID 0
#define TMPSTR 1000

lisp* lisp_atom(const atomtype a)
{
    lisp* atom = (lisp*)ncalloc(1, sizeof(lisp));
    atom->a = a;
    return atom;
}

lisp* lisp_cons(const lisp* l1,  const lisp* l2)
{
    lisp* cons = (lisp*)ncalloc(1, sizeof(lisp));
    //lisp作业变更
    // cons->car = lisp_copy((lisp*)l1);
    // cons->cdr = lisp_copy((lisp*)l2);
    cons->car = (lisp*)l1;
    cons->cdr = (lisp*)l2;
    return cons;
}

lisp* lisp_car(const lisp* l)
{
    if(l==NULL){
        return NULL; 
    }
    return l->car;
}

lisp* lisp_cdr(const lisp* l)
{
    if(l==NULL){
        return NULL; 
    }
    return l->cdr; 
}

atomtype lisp_getval(const lisp* l)
{
    if(l==NULL){
        return INVALID; 
    }

    if(lisp_isatomic(l)){
        return l->a;
    }
    return INVALID;
}

bool lisp_isatomic(const lisp* l)
{
    if(l == NULL){
        return false;
    }
    if(l->car == NULL && l->cdr == NULL){
        return true;
    }
    return false;
}

void lisp_tostring(const lisp* l, char* str)
{
    //Exceptions
    if(l == NULL){
        strcpy(str,"()");
        return;
    }
    //get leaf nodes
    if(lisp_isatomic(l) == true){
        if(sprintf(str, FORMATSTR, lisp_getval(l)) < 0){
            on_error("cannot read the atomvalue");
        }
        return;
    }
    strcpy(str,"(");
    lisp* p =(lisp*)l;
    char tmp[TMPSTR];
    //the head of the list;
    lisp_tostring(lisp_car(p), tmp);
    strcat(str, tmp);
    //the remainder of the list; 
    p = p->cdr;
    while(p != NULL){
        strcat(str, " ");
        lisp_tostring(lisp_car(p), tmp);
        strcat(str, tmp);
        p = p->cdr;
    }
    strcat(str,")");
}

lisp* lisp_copy(const lisp* l)
{
    if(l == NULL){
        return NULL;
    }
    lisp* cpy = (lisp*)ncalloc(1, sizeof(lisp));
    cpy->a = l->a;
    cpy->car = lisp_copy(lisp_car(l));
    cpy->cdr = lisp_copy(lisp_cdr(l));
    return cpy;
}

void lisp_free(lisp** l)
{
    if(*l == NULL ){
        return;
    }
    lisp_free(&((*l)->car));
    lisp_free(&((*l)->cdr));
    free((*l));
    *l = NULL; 
}

int lisp_length(const lisp* l)
{
    if(l == NULL){
        return 0;
    }
    //an atom is not a list.
    if(lisp_isatomic(l) == true){
        return 0;
    }
    int len = 0;
    lisp* p =(lisp*)l;
    do{
        len ++;
        p = p->cdr;
    }while(p != NULL);
    return len;
}

/*****************************************************************/
/*                      Extensions Functions                       */
/*****************************************************************/

lisp* lisp_list(const int n, ...)
{
    //must have at least one list
    if(n <= 0){
        return NULL;
    }
    va_list valist;
    va_start(valist,n);

    lisp* list = lisp_cons(va_arg(valist, lisp*), NULL);
    lisp* p = list; //a temporary pointer
    for(int i = 1; i < n; i++){
        p->cdr = lisp_cons(va_arg(valist, lisp*), NULL);
        p = p->cdr;
    }
    va_end(valist);
    return list;
}

void lisp_reduce(void (*func)(lisp* l, atomtype* n), lisp* l, atomtype* acc)
{
    if(lisp_isatomic(l) == true){
         func(l, acc);
         return;
    }

    lisp* p =(lisp*)l;
    lisp_reduce(func, lisp_car(p), acc);
    while(lisp_cdr(p) != NULL){
        p = lisp_cdr(p);
        lisp_reduce(func, lisp_car(p), acc);
    }
}

lisp* lisp_fromstring(const char* str)
{
    if(strlen(str) == 0 || strcmp(str, "()") == 0){
        return NULL;
    }
    int i = 0;
    if(str[i] == '('){
        lisp* l = (lisp*)ncalloc(1, sizeof(lisp));
        i += 1;
        _parseString(l, str, &i);
        return l;
    }
    if(strcmp(str,"0") == 0){
        return lisp_atom(0);
    }
    int val = atoi(str);
    if(val){ 
        return lisp_atom(val);
    }
    fprintf(stderr, "Using an invalid LITERAL\n");
    exit(EXIT_FAILURE);
}

/*****************************************************************/
/*                      Auxiliary Functions                       */
/*****************************************************************/

void _parseString(lisp* l, const char* str, int* i)
{
    *i += _skipSpace(str+*i);
    if(str[*i] == '('){
        *i += 1;//length of a '('
        l->car = (lisp*)ncalloc(1, sizeof(lisp));   
        _parseString(l->car, str, i);
        _tailCdr(l, str, i);
    }
    else if(*i == (int)strlen(str) - 1){
        if(str[*i] != ')'){
            fprintf(stderr, "Using an invalid LITERAL\n");
            exit(EXIT_FAILURE);
        }
    }
    else if(str[*i] == ')'){
        if(*i == (int)strlen(str) - 1){
            return;
        }
        *i += 1; //length of a ')'
    }
    else{
        int val = atoi(str + *i);
        l->car = lisp_atom(val);
        //"move" i to the index of next number
        char tem[TMPSTR];
        sprintf(tem,"%i",val); 
        *i += (int)strlen(tem);
        *i += _skipSpace(str+*i);
        _tailCdr(l, str, i); 
    }
}

//If CDR is at the end of a list, don't allocat memory for him
void _tailCdr(lisp* l, const char* str, int* i)
{
    if(str[*i] == ')'){
            _parseString(l->cdr, str, i);
    }else{
        l->cdr = (lisp*)ncalloc(1, sizeof(lisp));
        _parseString(l->cdr, str, i); 
    } 
}

//Calculate the number of spaces in front of a string
int _skipSpace(const char* str)
{
    int i = 0;
    while(str[i] == ' '){
        i++;
    }   
    return i; 
}
