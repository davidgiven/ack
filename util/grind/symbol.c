/* $Header$ */

/* Symbol handling */

#include	<alloc.h>
#include	<out.h>
#include	<stb.h>
#include	<assert.h>

#include	"position.h"
#include	"file.h"
#include	"idf.h"
#include	"type.h"
#include	"symbol.h"
#include	"scope.h"
#include	"tree.h"
#include	"operator.h"

p_symbol	currfile;

p_symbol
NewSymbol(s, scope, class, nam)
  char	*s;
  register p_scope scope;
  struct outname *nam;
{
  register p_symbol sym;
  
  sym = new_symbol();
  sym->sy_idf = str2idf(s, 0);
  sym->sy_scope = scope;
  sym->sy_prev_sc = scope->sc_symbs;
  scope->sc_symbs = sym;
  sym->sy_next = sym->sy_idf->id_def;
  sym->sy_idf->id_def = sym;
  sym->sy_class = class;
  switch(class) {
  case MODULE:
  case PROC:
  case FUNCTION:
  case VAR:
  case REGVAR:
  case LOCVAR:
  case VARPAR:
	sym->sy_name.nm_value = nam->on_valu;
	break;
  default:
	break;
  }
  return sym;
}

/* Lookup a definition for 'id' in scope 'scope' with class in the 'class'
   bitset.
*/
p_symbol
Lookup(id, scope, class)
  struct idf *id;
  p_scope scope;
  int	class;
{
  register p_symbol p = id ? id->id_def : 0;

  while (p) {
	if (p->sy_scope == scope && (p->sy_class & class)) {
		return p;
	}
	p = p->sy_next;
  }
  return (p_symbol) 0;
}

/* Lookup a definition for 'id' with class in the 'class' bitset,
   starting in scope 'sc' and also looking in enclosing scopes.
*/
p_symbol
Lookfromscope(id, class, sc)
  register struct idf *id;
  int	class;
  register p_scope	sc;
{
  if (! id) return (p_symbol) 0;

  while (sc) {
	register p_symbol sym = id->id_def;
	while (sym) {
		if (sym->sy_scope == sc && (sym->sy_class & class)) {
			return sym;
		}
		sym = sym->sy_next;
	}
	sc = sc->sc_static_encl;
  }
  return (p_symbol) 0;
}

/* Lookup a definition for 'id' with class in the 'class' bitset,
   starting in scope 'CurrentScope' and also looking in enclosing scopes.
*/
p_symbol
Lookfor(id, class)
  register struct idf *id;
  int	class;
{
  return Lookfromscope(id, class, CurrentScope);
}

extern char *strrindex();

p_symbol
add_file(s)
  char	*s;
{
  register p_symbol sym = NewSymbol(s,
				    PervasiveScope,
				    FILESYM,
				    (struct outname *) 0);
  register char *p;

  sym->sy_file = new_file();
  sym->sy_file->f_sym = sym;
  p = strrindex(s, '.');
  if (p) {
	char c = *p;
	p_symbol sym1;

	*p = 0;
	sym1 = NewSymbol(Salloc(s, (unsigned) strlen(s)+1),
		  	 PervasiveScope,
		 	 FILELINK,
			 (struct outname *) 0);
	*p = c;
	sym1->sy_filelink = sym;
  }
  return sym;
}

/* Determine if the OP_SELECT tree indicated by 'p' could lead to scope 'sc'.
*/
static int
consistent(p, sc)
  p_tree	p;
  p_scope	sc;
{
  p_tree	arg;
  p_symbol	sym;

  assert(p->t_oper == OP_SELECT);
  sc = sc->sc_static_encl;
  if (!sc) return 0;

  p = p->t_args[0];

  switch(p->t_oper) {
  case OP_NAME:
	sym = Lookfromscope(p->t_idf, FILELINK|FILESYM|PROC|MODULE, sc);
	return sym != 0;

  case OP_SELECT:
	arg = p->t_args[1];
	sym = Lookfromscope(arg->t_idf, FILELINK|FILESYM|PROC|MODULE, sc);
	if (sym == 0) return 0;
	return consistent(p, sym->sy_scope);

  default:
	assert(0);
  }
  return 0;	/* notreached? */
}

/* Try to find the name referred to in the node indicated by 'p', and
   try to be just a little bit intelligent about it.
*/
p_symbol
identify(p, class_set)
  p_tree	p;
  int		class_set;
{
  p_symbol	sym = 0;
  register p_symbol s;
  p_tree	arg;

  switch(p->t_oper) {
  case OP_NAME:
	if (! p->t_sc) p->t_sc = CurrentScope;
	sym = Lookfromscope(p->t_idf, class_set, p->t_sc);
	if (sym) {
		/* Found it. */
		break;
	}

	/* We could not find it using scope p->t_sc; now we try to identify
	   it using class_set. If this results in only one definition, we
	   take this one.
	*/
	s = p->t_idf->id_def;
	while (s) {
		if (s->sy_class & class_set) {
			if (sym) {
				error("could not identify \"%s\"", p->t_str);
				sym = 0;
				break;
			}
			sym = s;
		}
		s = s->sy_next;
	}
	if (!sym && !s) {
		error("could not find \"%s\"", p->t_str);
	}
	break;

  case OP_SELECT:
	arg = p->t_args[1];
	assert(arg->t_oper == OP_NAME);
	s = arg->t_idf->id_def;
	sym = 0;
	while (s) {
		if ((s->sy_class & class_set) && consistent(p, s->sy_scope)) {
			if (sym) {
				error("could not identify \"%s\"", arg->t_str);
				sym = 0;
			}
			sym = s;
		}
		s = s->sy_next;
	}
	if (!sym && !s) {
		error("could not find \"%s\"", arg->t_str);
	}
	break;

  default:
	assert(0);
  }
  return sym;
}
