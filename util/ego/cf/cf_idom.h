/*  C O N T R O L   F L O W
 *
 *  I M M E D I A T E   D O M I N A T O R S
 */


extern dominator();	/* (bblock_p head, short n)
			 * Compute for every basic block its immediate
			 * dominator. The dominator relation is hence
			 * recorded as a tree in which every node contains
			 * a pointer to its parent, which is its
			 * immediate dominator.
			 * 'n' is the number of nodes (basic blocks) in
			 * the control flow graph.
			 */
