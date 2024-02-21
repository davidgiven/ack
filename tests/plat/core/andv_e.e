#
#define W EM_WSIZE
#define P EM_PSIZE
    mes 2, W, P

    exp $_m_a_i_n
    pro $_m_a_i_n, 0

	/* And word-sized set */

four
	rom W

	loc 32769
	loc 1
	loe four		/* to defeat constant folding */
	and
	loc 1
	cmu W
	zeq *1

    loc __LINE__
    cal $fail
    asp 4
1

	/* And triple-word-sized set */

four_by_three
	rom W*3

	loc 32769
	loc 32770
	loc 32772
	loc 1
	loc 2
	loc 4
	loe four_by_three
	and
	loc 4
	cmu W
	zne *2
	loc 2
	cmu W
	zne *2
	loc 1
	cmu W
	zeq *3
2
	loc __LINE__
	cal $fail
	asp 4
3

    cal $finished
    end
