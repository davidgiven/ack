/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
extern bool anal_params();	/* (call_p c)
				 * See which parameters of the call
				 * may be expanded in line.
				 * If the formals and actuals do not
				 * match, return FALSE
				 */
extern assign_ratio();		/* (call_p c)
				 * Assigna ratio number to the call,
				 * indicating how desirable it is to
				 * expand the call in line.
				 */
extern call_p abstract();	/* (call_p c)
				 * Abstract essential information from
				 * the call.
				 */
extern select_calls();		/* (call_p alist; FILE *ccf;short space)
				 * Select the best calls to be expanded.
				 * Every procedure gets a list of
				 * selected calls appearing in it.
				 * space is the amount of space that the
				 * program is allowed to grow
				 * (expressed in number of EM instructions).
				 */
extern cleancals();		/* (proc_p plist)
				 * Remove all calls that were not selected.
				 */
extern add_actuals();	/* (proc_p plist; FILE *cfile)
			* Add the actual parameters to the descriptor abstracts
			* of the selected calls.
	 		* the calfile contains the full descriptors of all
			* calls.
	 		* These two are combined to yield a file of full
	 		* descriptors of the selected calls.
	 		*/
extern append_abstract(); /* (call_p a; proc_p p)
			   * Put the call-descriptor abstract in the p_cals
			   * list of p.
			   */
