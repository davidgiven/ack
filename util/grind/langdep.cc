/* $Id$ */

#include "langdep.h"

struct langlist {
  struct langlist	*l_next;
  struct langdep	*l_lang;
  char			*l_suff;
};

/* STATICALLOCDEF "langlist" 5 */

static struct langlist *list;

struct langdep	*currlang;

static void
add_language(suff, lang)
  char	*suff;
  struct langdep *lang;
{
  struct langlist *p = new_langlist();

  p->l_next = list;
  p->l_suff = suff;
  p->l_lang = lang;
  list = p;
}

init_languages()
{
  add_language(".p", pascal_dep);
  add_language(".mod", m2_dep);
  add_language(".c", c_dep);
}

find_language(suff)
  char	*suff;
{
  register struct langlist *p = list;

  if (! suff) {
	currlang = c_dep;
	return;
  }
  while (p) {
	currlang = p->l_lang;
	if (! strcmp(p->l_suff, suff)) break;
	p = p->l_next;
  }
  if (! currlang) {
	currlang = c_dep;
  }
}
