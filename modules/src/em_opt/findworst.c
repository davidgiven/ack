#ifndef NORCSID
static char rcsidp3[] = "$Id$";
#endif

#include "parser.h"

#define UPDATEWORST(backups) if(backups>mostbackups) mostbackups = backups;

PRIVATE int rightmatch(struct mnems,struct mnems,int, int);
PRIVATE int leftmatch(struct mnems, struct mnems, int, int);


void findworst(struct mnems patt,struct mnems repl)
{
	/*
	/ Find the pattern that requires the most backup of output queue.
	/ Let repl be r1 r2 ... rn. All these are already on the output queue.
	/ Possibilities in order of most backup first are:
	/ a)	pattern of form: p1 .... pb r1 r2 .... rn pc ... pd
	/	i.e. <repl> completely in pattern.
	/	requires a backup of b+n instructions
	/	and a goto to state 0.
	/ b)	pattern of form: p1 .... pb r1 r2 .... ri
	/	i.e. a prefix of <repl> ends a pattern.
	/	requires a backup of b+n instructions
	/	and a goto to state 0.
	/ c)	pattern of form: ri ri+1 ... rn pc ... pd
	/	i.e. a suffix of <repl> starts a pattern.
	/	requires a backup of j-i+1 instructions and a goto to state 0.
	/ d)   pattern of the form: ri ri+1 ... rj
	/	i.e. a substring of <repl> is a complete pattern
	/	requires a backup of j-i+1 instructions and a goto to state 0.
	*/
	int n = repl.m_len;
	int diff = patt.m_len - repl.m_len;
	int first,i,j;
	int s;
	int mostbackups = 0;
	if(n==0) {
		fprintf(ofile,"\t\tOO_mkrepl(0,%d,%d);\n",diff,maxpattern-1);
		return;
	}
	for(s=1;s<=higheststate;s++) {
		/* only match complete patterns */
		if(actions[s]==(struct action *)NULL)
			continue;
		/* look for case a */
		if( (first=rightmatch(patterns[s],repl,1,n))) {
			UPDATEWORST(first-1+n);
		}
		/* look for case b */
		for(i=n-1;i;i--) {
			if((first=rightmatch(patterns[s],repl,1,i)) &&
			   (first+i-1==patterns[s].m_len)) {
				UPDATEWORST(first-1+n);
			}
		}
		/* look for case c */
		for(i=2;i<=n;i++) {
			if((first=leftmatch(patterns[s],repl,i,n)) &&
			   (first==1)) {
				UPDATEWORST(n-i+1);
			}
		}
		/* look for case d */
		for(i=2;i<=n;i++) {
			for(j=n-1;j>i;j--) {
				if((first=leftmatch(patterns[s],repl,i,j)) &&
				   (first==1)&&
				   (j-i+1 == patterns[s].m_len)) {
					UPDATEWORST(n-i+1);
				}
			}
		}
	}
	fprintf(ofile,"\t\tOO_mkrepl(%d,%d,%d);\n",n,diff,mostbackups);
}

void findfail(int state, int *resout, int *rescpy, int *resgto)
{
	/*
	/ If pattern matching fails in 'state', how many outputs and how many
	/ push backs are requires. If pattern is of the form p1 p2 .... pn
	/ look for patterns of the form p2 p3 ... pn; then p3 p4 ... pn; etc.
	/ The first such match of the form pi pi+1 ... pn requires an output
	/ of p1 p2 ... pi-1 and a push back of pn pn-1 ... pi.
	*/
	int s,i;
	struct state *p;
	int istrans;
	int n = patterns[state].m_len;
	for(i=2;i<=n;i++) {
		for(s=1;s<=higheststate;s++) {
			/* exclude those on transitions from this state */
			istrans = 0;
			for(p=states[state];p!=(struct state *)NULL;p=p->next)
				if(s==p->goto_state)
					istrans++;
			if(istrans)
				continue;
			if((leftmatch(patterns[s],patterns[state],i,n)==1)&&
				patterns[s].m_len==(n-i+1)) {
					*resout = i-1;
					*rescpy = n-i+1;
					*resgto = s;
					return;
				}
		}
	}
	*resout = n;
	*rescpy = 0;
	*resgto = 0;
}

PRIVATE int leftmatch(struct mnems patt,struct mnems repl,int i,int j)
{
	/*
	/ Return the first complete match of the mnems <ri,ri+1,..,rj> of
	/ 'repl' in the mnems of 'patt'.  Find the leftmost match.
	/ Return 0 if fails.
	*/
	int lenrij = j-i+1;
	int lastpos = patt.m_len - lenrij + 1;
	int k,n;
	for(k=1;k<=lastpos;k++) {
		for(n=1;n<=lenrij;n++) {
			if(patt.m_elems[(k+n-1)-1]->op_code != repl.m_elems[(i+n-1)-1]->op_code)
				break;
		}
		if(n>lenrij) {
			return(k);
		}
	}
	return(0);
}

PRIVATE int rightmatch(struct mnems patt,struct mnems repl,int i,int j)
{
	/*
	/ Return the first complete match of the mnems <ri,ri+1,..,rj> of
	/ 'repl' in the mnems of 'patt'.  Find the rightmost match.
	/ Return 0 if fails.
	*/
	int lenrij = j-i+1;
	int lastpos = patt.m_len - lenrij + 1;
	int k,n;
	for(k=lastpos;k>=1;k--) {
		for(n=1;n<=lenrij;n++) {
			if(patt.m_elems[(k+n-1)-1]->op_code != repl.m_elems[(i+n-1)-1]->op_code)
				break;
		}
		if(n>lenrij) {
			return(k);
		}
	}
	return(0);
}
