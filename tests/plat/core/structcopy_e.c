#include "test.h"

/* ACK's C compiler uses EM's loi, sti, blm, or an inline loop to copy
 * these structs.  The compiler doesn't call memcpy() or other
 * functions in libc, so this test passes without linking the CRT.
 */

struct c5 {       /* not a whole number of words */
	char one[5];
};

struct ii {       /* two words */
	int one;
	int two;
};

struct iii {      /* three words */
	int one;
	int two;
	int three;
};

int equal5(char *a, char *b) {  /* a, b must have 5 characters */
	int i;

	for (i = 0; i < 5; i++)
		if (a[i] != b[i]) return 0;
	return 1;
}

struct c5 make_c5(char *str) {  /* str must have 5 characters */
	struct c5 out;
	int i;

	for (i = 0; i < 5; i++)
		out.one[i] = str[i];
	return out;
}

struct ii make_ii(int i, int j) {
	struct ii out;

	out.one = i;
	out.two = j;
	return out;
}

struct iii make_iii(struct ii in, int k) {
	struct iii out;

	out.one = in.one;
	out.two = in.two;
	out.three = k;
	return out;
}

struct c5 rotate_left_c5(struct c5 in) {
	int i;
	char c = in.one[0];

	/* Modifies our copy of _in_, not caller's copy. */
	for (i = 0; i < 4; i++)
		in.one[i] = in.one[i + 1];
	in.one[4] = c;
	return in;
}

struct iii rotate_left_iii(struct iii in) {
	int i = in.one;

	/* Modifies our copy of _in_, not caller's copy. */
	in.one = in.two;
	in.two = in.three;
	in.three = i;
	return in;
}

/* Bypasses the CRT, so there's no stdio. */
void _m_a_i_n(void) {
	struct c5 earth, heart, dup_heart, rol_heart;
	struct ii pair, dup_pair;
	struct iii triple, dup_triple, rol_triple;

	earth = make_c5("earth");
	heart = make_c5("heart");
	dup_heart = heart;
	rol_heart = rotate_left_c5(heart);
	ASSERT(equal5(earth.one, "earth"));
	ASSERT(equal5(heart.one, "heart"));
	ASSERT(equal5(dup_heart.one, "heart"));
	ASSERT(equal5(rol_heart.one, "earth"));

	pair = make_ii(29, 31);
	dup_pair = pair;
	triple = make_iii(pair, -9);
	dup_triple = triple;
	rol_triple = rotate_left_iii(triple);
	ASSERT(pair.one == 29);
	ASSERT(pair.two == 31);
	ASSERT(dup_pair.one == 29);
	ASSERT(dup_pair.two == 31);
	ASSERT(triple.one == 29);
	ASSERT(triple.two == 31);
	ASSERT(triple.three == -9);
	ASSERT(dup_triple.one == 29);
	ASSERT(dup_triple.two == 31);
	ASSERT(dup_triple.three == -9);
	ASSERT(rol_triple.one == 31);
	ASSERT(rol_triple.two == -9);
	ASSERT(rol_triple.three == 29);

	finished();
}
