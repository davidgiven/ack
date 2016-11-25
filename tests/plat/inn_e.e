#
    mes 2, 4, 4

    exp $_m_a_i_n
    pro $_m_a_i_n, 0

    /* Test non-existent bit */

.1
    rom 0I1, 0I1, 0I1, 0I1
    loe .1
    loc 1       /* bit number */
    inn 4
    zeq *1

    loc __LINE__
    cal $fail
    ass 4
1

    /* Test existent bit */

.2
    rom 2I1, 0I1, 0I1, 0I1
    loe .2
    loc 1       /* bit number */
    inn 4
    zne *2

    loc __LINE__
    cal $fail
    ass 4
2

    /* Test non-existent high bit */

.3
    rom 0I1, 0I1, 0I1, 0I1
    rom 0I1, 0I1, 0I1, 0I1
.31
    rom 33      /* to defeat constant folding */

    lae .3
    loi 8
    loe .31     /* bit number */
    inn 8
    zeq *3

    loc __LINE__
    cal $fail
    ass 4
3

    /* Test existent high bit */

.4
    rom 0I1, 0I1, 0I1, 0I1
    rom 2I1, 0I1, 0I1, 0I1
.41
    rom 33      /* to defeat constant folding */

    lae .4
    loi 8
    loe .41     /* bit number */
    inn 8
    zne *4

    loc __LINE__
    cal $fail
    ass 4
4

    cal $finished
    ret 0
    
    end
