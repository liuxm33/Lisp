#prog 1
(
    (SET A '5')
    (PRINT A)
    (SET B '(1 2)')
    (PRINT B)
    (SET C '(   )')
    (PRINT C)
    (SET D '(0)')
    (PRINT D)
    (SET E '(1 (2    3))')
    (PRINT E)
    (PRINT (CDR E))
    (PRINT E)
    (SET F NIL)
    (PRINT F)
    (SET G A)
    (PRINT G)

    (SET H (CONS A B))
    (PRINT H)
    (PRINT "Hello,   World! 123 !")

    (IF(LESS '2' '1')((PLUS '1' '2'))((LENGTH E)))
    (IF(EQUAL '1' '1')((PLUS A (CAR B)))((LENGTH NULL)))
    (IF(GREATER '1' '3')((PLUS A (CAR B)))((LENGTH '(1 2(3 4)(5))')))
    (WHILE (LESS '0' A)((PRINT A)(SET A(PLUS '-1' A))))
    (WHILE (LESS '0' A)((PRINT A)(SET A(PLUS '-1' A))))
    (PRINT "INTERP OK!")
)
#prog 2
(
    (
        IF(EQUAL '1' '1')(
            (SET B '0')
            (PRINT B)
        )(
            (GARBAGE)
        )
    )
)
#prog 3
(
    (SET B '1')#B->5
    (SET C '(2)')
    (SET D (CONS B C)) # C->(1 2)
    (SET B D)
    (SET D B)
)
#prog 4
(
    (IF(EQUAL '1' '2')((SET A '(1 2)')(PRINT "A"))((SET B '(1 2)')(PRINT B)))
    (IF(EQUAL '1' '1')((SET A '(1 2)')(PRINT "A"))((SET B '(1 2)')(PRINT B)))
)
