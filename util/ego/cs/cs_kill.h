/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
extern	kill_call();	/* (proc_p pp)
			 * Kill all entities that might have an other value
			 * after execution of the procedure in pp.
			 */

extern	kill_much();	/* ()
			 * Kill all killable entities except those for which
			 * a register message was generated.
			 * Constants, addresses, etc are not killable.
			 */

extern	kill_indir();	/* (entity_p enp)
			 * Kill all entities that might have an other value
			 * after indirect assignment to the entity in enp.
			 */

extern	kill_direct();	/* (entity_p enp)
			 * Kill all entities that might have an other value
			 * after direct assignment to the entity in enp.
			 */

extern	kill_all();	/* ()
			 * Kill all entities.
			 */
