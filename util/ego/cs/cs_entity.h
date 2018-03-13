/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
extern lset	entities;	/* The pseudo-symboltable. */

extern entity_p	find_entity(valnum vn);
				/*
				 * Tries to find an entity with value number vn.
				 */

extern entity_p	en_enter(entity_p enp);
				/*
				 * Enter the entity in enp in the set of
				 * entities if it was not already there.
				 */

extern void	clr_entities(void);
				/*
				 * Release all space occupied by our
				 * pseudo-symboltable.
				 */
