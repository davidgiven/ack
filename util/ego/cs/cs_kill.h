/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
extern void kill_call(proc_p pp);
			/*
			 * Kill all entities that might have an other value
			 * after execution of the procedure in pp.
			 */

extern void kill_much(void);
			/*
			 * Kill all killable entities except those for which
			 * a register message was generated.
			 * Constants, addresses, etc are not killable.
			 */

extern void kill_indir(entity_p enp);
			/*
			 * Kill all entities that might have an other value
			 * after indirect assignment to the entity in enp.
			 */

extern void kill_direct(entity_p enp);
			/*
			 * Kill all entities that might have an other value
			 * after direct assignment to the entity in enp.
			 */

extern void kill_all(void);
			/*
			 * Kill all entities.
			 */
