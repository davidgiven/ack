extern		cs_machinit();	/* (FILE *f)
				 * Read phase-specific information from f.
				 */

extern bool	desirable();	/* (avail_p avp)
				 * Return whether it is desirable to eliminate
				 * the recurrences of the expression in avp.
				 * At the same time delete the recurrences
				 * for which it is not allowed.
				 */
