#pragma once
#include "lisp.h" 
#include <string.h>
#include <assert.h>
#include <ctype.h>

#define FORMATSTR "%i"
#define LISPIMPL "Linked"

struct lisp{
    atomtype a; 
    struct lisp* car;
    struct lisp* cdr;
};

int _skipSpace(const char* str);
void _parseString(lisp* l, const char* str, int* i);
void _tailCdr(lisp* l, const char* str, int* i);
