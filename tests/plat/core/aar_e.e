#
#define W EM_WSIZE
#define P EM_PSIZE

    mes 2, W, P

/*
 * Does basic testing of aar. Unfortunately, aar throws ERANGE on
 * error, which we can't catch (or at least, the platforms I've looked at
 * don't allow it to be caught, those platforms which actually throw it on
 * error). So we just test the non-throwing cases, not the negative ones.
 */

array
	bss 3*W, 0, 0

descriptor
	con -1			; lower bound
	con 2			; range, *inclusive*
	con W	        ; size of element


	exp $_m_a_i_n
	pro $_m_a_i_n, 0

	/* Access element -1 */

	lae array
	loc -1
	lae descriptor
	aar W

	lae array
	cmu W
	zeq *1

	loc __LINE__
	cal $fail
	ass W
1

	/* Access element 0 */

	lae array
	loc 0
	lae descriptor
	aar W

	lae array
	adp W
	cmu W
	zeq *2

	loc __LINE__
	cal $fail
	ass W
2

	/* Access element 1 */

	lae array
	loc 1
	lae descriptor
	aar W

	lae array
	adp W*2
	cmu W
	zeq *3

	loc __LINE__
	cal $fail
	ass W
3

    cal $finished
    end

/* vim: set sw=4 ts=4 et ft=asm: */
