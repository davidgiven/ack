extern entity_p getentity();	/* (line_p lnp, *l_out)
				 * Extract the entity lnp refers and enter it
				 * in the table of entities. The main entity
				 * lnp refers to is returned; sometimes there
				 * is more than one entity. The first line that
				 * was involved in pushing it is returned
				 * through l_out.
				 */
