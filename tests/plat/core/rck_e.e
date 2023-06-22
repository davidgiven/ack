#
    mes 2, EM_WSIZE, EM_PSIZE

/*
 * Uses _rck_ for range checks.  Catches the EM trap if a value is out
 * of range, and continues with the next instruction after _rck_.
 */

testnr
    con 1         ; test number
caught
    con 0         ; number of caught traps

    inp $next
    inp $catch
    inp $never
    exp $_m_a_i_n
    pro $_m_a_i_n,0

/* These architecture ignore _rck_. */
#if defined i80
    cal $finished
#endif

    lim           ; load ignore mask
    loc 2
    and EM_WSIZE  ; check bit 1 << ERANGE
    zeq *1        ; fail if ignoring ERANGE
.1
    rom 1I4
    lae .1
    loi EM_WSIZE
    cal $fail
    asp EM_WSIZE
1

    cal $next     ; increment testnr, catch next trap
    loc 10125
.2
    rom 4283, 13644
    lae .2
    rck EM_WSIZE  ; testnr 2 in range
    asp EM_WSIZE

    cal $next
    loc 4282
    lae .2
    rck EM_WSIZE  ; testnr 3 out of range
    asp EM_WSIZE

    cal $next
    loc 4283
    lae .2
    rck EM_WSIZE  ; testnr 4 in range
    asp EM_WSIZE

    cal $next
    loc 13644
    lae .2
    rck EM_WSIZE  ; testnr 5 in range
    asp EM_WSIZE

    cal $next
    loc 13655
    lae .2
    rck EM_WSIZE  ; testnr 6 out of range
    asp EM_WSIZE

    cal $next
    loc -13015
.7
    rom -31344, -1898
    lae .7
    rck EM_WSIZE  ; testnr 7 in range
    asp EM_WSIZE

    cal $next
    loc 8580
.8
    rom -26315, 4588
    lae .8
    rck EM_WSIZE  ; testnr 8 out of range
    asp EM_WSIZE

    ; The last test raised a trap, so now there is no trap handler.
    lpi $never
    sig           ; push old trap handler
    loc 0
    loc EM_WSIZE
    loc EM_PSIZE
    cuu           ; push NULL pointer
    cmp
    zeq *17       ; fail unless old handler is NULL
.17
    rom 17I4
    lae .17
    loi EM_WSIZE
    cal $fail
    asp EM_WSIZE
17
    ; Change the trap handler from $never to $catch.
    lpi $catch
    sig
    lpi $never
    cmp
    zeq *18
.18
    rom 18I4
    lae .18
    loi EM_WSIZE
    cal $fail
    asp EM_WSIZE
18
    ; Begin ignoring range traps.
    loc 2         ; 1 << ERANGE
    sim
    loc 18
    ste testnr
    loc 8580
    lae .8
    rck EM_WSIZE  ; testnr 18 out of range but ignored

    ; Fail if we caught the wrong number of traps.
    loe caught
    loc 3
    beq *20
.20
    rom 20I4
    lae .20
    loi EM_WSIZE
    cal $fail
    asp EM_WSIZE
20
    cal $finished
    end

    pro $next,0
    ine testnr    ; next test
    lpi $catch
    sig           ; catch next EM trap (only one trap)
    asp EM_PSIZE
    ret 0
    end

    pro $catch,0
    ine caught    ; count this trap

    lol 0         ; load trap number
    loc 1
    beq *1        ; fail if trap != ERANGE
.101
    rom 257I4
    lae .101
    loi EM_WSIZE
    cal $fail
    ; Wrong type of trap.  _rtt_ might not work, so exit now.
    cal $finished
1
    ; Fail if the wrong test raised this trap.
    loe testnr
    loc 3
    beq *2
    loe testnr
    loc 6
    beq *2
    loe testnr
    loc 8
    beq *2
    loc 256
    loe testnr
    adi EM_WSIZE  ; 0x100 + testnr
    cal $fail
    asp EM_WSIZE
2
    rtt           ; return from trap handler
    end

    pro $never,0
.200
    rom 200I4
    lae .200
    loi EM_WSIZE
    cal $fail
    asp EM_WSIZE
    rtt
    end
