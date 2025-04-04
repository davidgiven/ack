#
    mes 2, EM_WSIZE, EM_PSIZE

/*
 * Does basic testing of aar. Unfortunately, aar throws ERANGE on
 * error, which we can't catch (or at least, the platforms I've looked at
 * don't allow it to be caught, those platforms which actually throw it on
 * error). So we just test the non-throwing cases, not the negative ones.
 */

array
	bss 3*EM_WSIZE, 0, 0

descriptor
	con -1			; lower bound
	con 2			; range, *inclusive*
	con EM_WSIZE	; size of element


	exp $_m_a_i_n
	pro $_m_a_i_n, 0

	/* Access element -1 */

	lae array
	loc -1
	lae descriptor
	aar EM_WSIZE

	lae array
	cmu EM_WSIZE
	zeq *1

	loc __LINE__
	cal $fail
	ass EM_WSIZE
1

	/* Access element 0 */

	lae array
	loc 0
	lae descriptor
	aar EM_WSIZE

	lae array
	adp EM_WSIZE
	cmu EM_WSIZE
	zeq *2

	loc __LINE__
	cal $fail
	ass EM_WSIZE
2

	/* Access element 1 */

	lae array
	loc 1
	lae descriptor
	aar EM_WSIZE

	lae array
	adp EM_WSIZE*2
	cmu EM_WSIZE
	zeq *3

	loc __LINE__
	cal $fail
	ass EM_WSIZE
3

    cal $finished
    end

