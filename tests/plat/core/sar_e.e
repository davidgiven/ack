#
    mes 2, EM_WSIZE, EM_PSIZE

/*
 * Does basic testing of sar. Unfortunately, sar throws ERANGE on
 * error, which we can't catch (or at least, the platforms I've looked at
 * don't allow it to be caught, those platforms which actually throw it on
 * error). So we just test the non-throwing cases, not the negative ones.
 */

#define ARRAY_SIZE 3*EM_WSIZE

array
	bss ARRAY_SIZE, 0, 0

descriptor
	con -1			; lower bound
	con 3			; range
	con EM_WSIZE	; size of element

element0
	con 1
	con 0
	con 0

element1
	con 0
	con 1
	con 0

element2
	con 0
	con 0
	con 1

	exp $_m_a_i_n
	pro $_m_a_i_n, 0

	/* Write element -1 */

	cal $clear_array
	loc 1
	lae array
	loc -1
	lae descriptor
	sar EM_WSIZE

	lae array
	loi ARRAY_SIZE
	lae element0
	loi ARRAY_SIZE
	loc ARRAY_SIZE
	cms
	zeq *1

	loc __LINE__
	cal $fail
	ass EM_WSIZE
1

	/* Write element 0 */

	cal $clear_array
	loc 1
	lae array
	loc 0
	lae descriptor
	sar EM_WSIZE

	lae array
	loi ARRAY_SIZE
	lae element1
	loi ARRAY_SIZE
	loc ARRAY_SIZE
	cms
	zeq *2

	loc __LINE__
	cal $fail
	ass EM_WSIZE
2

	/* Write element 1 */

	cal $clear_array
	loc 1
	lae array
	loc 1
	lae descriptor
	sar EM_WSIZE

	lae array
	loi ARRAY_SIZE
	lae element2
	loi ARRAY_SIZE
	loc ARRAY_SIZE
	cms
	zeq *3

	loc __LINE__
	cal $fail
	ass EM_WSIZE
3

    cal $finished
    end

	exp $clear_array
	pro $clear_array, 0

	loc 0
	lae array + EM_WSIZE*0
	sti EM_WSIZE

	loc 0
	lae array + EM_WSIZE*1
	sti EM_WSIZE

	loc 0
	lae array + EM_WSIZE*2
	sti EM_WSIZE

	ret 0
	end

