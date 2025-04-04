#
    mes 2, EM_WSIZE, EM_PSIZE

    exp $_m_a_i_n
    pro $_m_a_i_n, 0

	/* Compare equal word sets. */

word
	rom EM_WSIZE

	loc 1
	loc 1
	loe word		/* to defeat constant folding */
	cms
	zeq *1

    loc __LINE__
    cal $fail
    asp 4
1

	/* Compare non-equal word sets. */

	loc 1
	loc 2
	loe word		/* to defeat constant folding */
	cms
	zne *2

    loc __LINE__
    cal $fail
    asp 4
2

	/* Compare equal triple-word sets. */

	loc 1
	loc 2
	loc 3
	loc 1
	loc 2
	loc 3
	loe word		/* to defeat constant folding */
	loc 3
	mli EM_WSIZE
	cms
	zeq *3

    loc __LINE__
    cal $fail
    asp 4
3

	/* Compare non-equal triple-word sets. */

	loc 1
	loc 2
	loc 3
	loc 1
	loc 2
	loc 4
	loe word		/* to defeat constant folding */
	loc 3
	mli EM_WSIZE
	cms
	zne *4

    loc __LINE__
    cal $fail
    asp 4
4

    cal $finished
    end
