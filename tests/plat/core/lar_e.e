#
    mes 2, EM_WSIZE, EM_PSIZE

/*
 * Does basic testing of lar. Unfortunately, lar throws ERANGE on
 * error, which we can't catch (or at least, the platforms I've looked at
 * don't allow it to be caught, those platforms which actually throw it on
 * error). So we just test the non-throwing cases, not the negative ones.
 */

#define ARRAY_SIZE 3*EM_WSIZE

array
	con 1
	con 2
	con 3

descriptor
	con -1			; lower bound
	con 3			; range
	con EM_WSIZE	; size of element

	exp $_m_a_i_n
	pro $_m_a_i_n, 0

	/* Read element -1 */

	lae array
	loc -1
	lae descriptor
	lar EM_WSIZE

	loc 1
	beq *1

	loc __LINE__
	cal $fail
	ass EM_WSIZE
1

	/* Read element 0 */

	lae array
	loc 0
	lae descriptor
	lar EM_WSIZE

	loc 2
	beq *2

	loc __LINE__
	cal $fail
	ass EM_WSIZE
2

	/* Read element 1 */

	lae array
	loc 1
	lae descriptor
	lar EM_WSIZE

	loc 3
	beq *3

	loc __LINE__
	cal $fail
	ass EM_WSIZE
3

    cal $finished
    end

