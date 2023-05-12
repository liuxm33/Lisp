# In *my* code, I had the interpreter code added to the parser code.
# This additional code is protected by #ifdef INTERP / #endif clauses.
# This code is only "seen" by the compiler if you have #defined INTERP
# This #define would be annoying to change inside nuclei.c/.h, so instead it is
# set in the gcc/clang statement using -DINTERP
# In this way ./parse & ./interp can both be built from the same source file.

CC      := clang
DEBUG   := -g3
OPTIM   := -O3
CFLAGS  := -Wall -Wextra -Wpedantic -Wfloat-equal -Wvla -std=c99 -Werror
RELEASE := $(CFLAGS) $(OPTIM)
SANI    := $(CFLAGS) -fsanitize=undefined -fsanitize=address $(DEBUG)
VALG    := $(CFLAGS)  $(DEBUG)

NCLS    := $(wildcard *.ncl)
PRES := $(NCLS:.ncl=.pres)
IRES := $(NCLS:.ncl=.ires)

LIBS    := -lm

parse: nuclei.c linked.c general.c nuclei.h lisp.h general.h
	$(CC) nuclei.c linked.c general.c $(RELEASE) -o parse $(LIBS)

parse_s: nuclei.c linked.c general.c nuclei.h lisp.h general.h
	$(CC) nuclei.c linked.c general.c $(SANI) -o parse_s $(LIBS)

parse_v: nuclei.c linked.c general.c nuclei.h lisp.h general.h
	$(CC) nuclei.c linked.c general.c $(VALG) -o parse_v $(LIBS)

all: parse parse_s parse_v interp interp_s interp_v

interp: nuclei.c linked.c general.c nuclei.h lisp.h general.h
	$(CC) nuclei.c linked.c general.c $(RELEASE) -DINTERP -o interp $(LIBS)

interp_s: nuclei.c linked.c general.c nuclei.h lisp.h general.h
	$(CC) nuclei.c linked.c general.c $(SANI) -DINTERP -o interp_s $(LIBS)

interp_v: nuclei.c linked.c general.c nuclei.h lisp.h general.h
	$(CC) nuclei.c linked.c general.c $(VALG) -DINTERP -o interp_v $(LIBS)

#################################### Test ###################################
tst_parse: nuclei.c linked.c general.c nuclei.h lisp.h general.h testing.c
	gcc testing.c nuclei.c linked.c general.c $(RELEASE) -DTEST -DUTEST -o tst_parse $(LIBS)

alltst: tst_parse tst_interp rep_interp

tst_interp: nuclei.c linked.c general.c nuclei.h lisp.h general.h testing.c
	gcc testing.c nuclei.c linked.c general.c $(RELEASE) -DTEST -DINTERP -DUTEST -o tst_interp $(LIBS)
#test coverage report
rep_interp: nuclei.c linked.c general.c nuclei.h lisp.h general.h 
	gcc testing.c nuclei.c linked.c general.c -fprofile-arcs -ftest-coverage $(RELEASE) -DTEST -DINTERP  -o rep_interp $(LIBS)

# For all .ncl files, run them and output result to a .pres (prase result) 
# or .ires (interpretted result) file.
runall : ./parse_s ./interp_s $(PRES) $(IRES)

%.pres:
	-./parse_s  $*.ncl > $*.pres 2>&1
%.ires:
	-./interp_s $*.ncl > $*.ires 2>&1

clean:
	rm -f parse parse_s parse_v interp interp_s interp_v $(PRES) $(IRES)

cleantst:
	rm -f tst_parse tst_interp rep_interp --force *.gcda *.gcno *.gcov

