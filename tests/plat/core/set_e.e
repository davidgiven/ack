#
    mes 2, EM_WSIZE, EM_PSIZE

    exp $_m_a_i_n
    pro $_m_a_i_n, 0

	/* Create word-sized singleton set. */

word
	rom EM_WSIZE

	loc 1
	loe word		/* to defeat constant folding */
	set

	loc 2
	cmu EM_WSIZE
	zeq *1

    loc __LINE__
    cal $fail
    asp 4
1

	/* Create triple-word-sized set with low bit */

	loc EM_WSIZE*8 + 1
	loe word		/* to defeat constant folding */
	loc 3
	mli EM_WSIZE
	set


	loc 0
	cmu EM_WSIZE
	zne *2
	loc 2
	cmu EM_WSIZE
	zne *2
	loc 0
	cmu EM_WSIZE
	zeq *3
2
	loc __LINE__
	cal $fail
	asp 4
3

    cal $finished
    end
