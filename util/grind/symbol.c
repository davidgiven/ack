/* $Header$ */

/* Symbol handling */

#include	<stdio.h>
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
#include	"misc.h"

p_symbol	currfile, listfile;

extern FILE	*db_out;

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
	s = Salloc(s, (unsigned) strlen(s)+1);
	*p = c;
	sym1 = NewSymbol(s,
		  	 PervasiveScope,
		 	 FILELINK,
			 (struct outname *) 0);
	sym1->sy_filelink = sym;
	sym->sy_file->f_base = sym1;
  }
  return sym;
}

static p_scope
def_scope(s)
  p_symbol	s;
{
  switch(s->sy_class) {
  case FILELINK:
	s = s->sy_filelink;
	/* fall through */
  case FILESYM:
	return s->sy_file->f_scope;
  case PROC:
  case FUNCTION:
  case MODULE:
  case TYPE:
  case VAR:
  case REGVAR:
  case LOCVAR:
  case VARPAR:
	return s->sy_name.nm_scope;
  }
  return 0;
}

/* Determine if the OP_SELECT tree indicated by 'p' could lead to scope 'sc'.
*/
int
consistent(p, sc)
  p_tree	p;
  p_scope	sc;
{
  p_tree	arg;
  p_symbol	sym;
  p_scope	target_sc;

  assert(p->t_oper == OP_SELECT);

  p = p->t_args[0];

  switch(p->t_oper) {
  case OP_NAME:
#define CLASS	(FILELINK|FILESYM|PROC|FUNCTION|MODULE|TYPE|VAR|REGVAR|LOCVAR|VARPAR|LBOUND|UBOUND)
	sym = Lookfromscope(p->t_idf, CLASS, sc->sc_static_encl);
	if (sym) {
		int precise = 1;

		target_sc = def_scope(sym);
		while (sc && sc != target_sc) {
			precise = 0;
			sc = sc->sc_static_encl;
		}
		return sc == 0 ? 0 : precise + 1 ;
	}
	return 0;

  case OP_SELECT:
	arg = p->t_args[1];
	sym = Lookfromscope(arg->t_idf, CLASS, sc->sc_static_encl);
	if (sym) {
		int precise = 1;

		target_sc = def_scope(sym);
		while (sc && sc != target_sc) {
			precise = 0;
			sc = sc->sc_static_encl;
		}
		if (sc == 0) return 0;
		if (precise) return consistent(p, sym->sy_scope);
		return consistent(p, sym->sy_scope) != 0;
	}
	return 0;

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
  p_symbol	sym = 0, sym1 = 0;
  register p_symbol s;
  p_tree	arg;
  int precise = 0;

  switch(p->t_oper) {
  case OP_NAME:
	sym = Lookfromscope(p->t_idf, class_set, CurrentScope);
	if (sym) {
		/* Found it. */
		break;
	}

	/* We could not find it using the current scope; now we try to identify
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
	while (s) {
		int temp;
		if ((s->sy_class & class_set) &&
		    (temp = consistent(p, s->sy_scope))) {
			if (temp > precise) {
				sym = s;
				precise = temp;
				sym1 = 0;
			}
			else if (sym && temp == precise) sym1 = s;
		}
		s = s->sy_next;
	}
	if (sym && sym1) {
		error("could not identify \"%s\"", arg->t_str);
		return 0;
	}
	if (!sym && !s) {
		error("could not find \"%s\"", arg->t_str);
		return 0;
	}
	break;

  default:
	assert(0);
  }
  return sym;
}

static
pr_scopes(sc)
  p_scope	sc;
{
  if (! sc) return;
  pr_scopes(sc->sc_static_encl);
  if (sc->sc_definedby) {
  	if (sc->sc_definedby->sy_class == FILESYM &&
	    sc->sc_definedby->sy_file->f_base) {
		fprintf(db_out, "%s`", sc->sc_definedby->sy_file->f_base->sy_idf->id_text);
  	}
  	else fprintf(db_out, "%s`", sc->sc_definedby->sy_idf->id_text);
  }
}

pr_sym(s)
  p_symbol	s;
{
  switch(s->sy_class) {
  case CONST:
	fprintf(db_out, "Constant:\t");
	break;
  case TYPE:
	fprintf(db_out, "Type:\t\t");
	break;
  case TAG:
	fprintf(db_out, "Tag:\t\t");
	break;
  case MODULE:
	fprintf(db_out, "Module:\t\t");
	break;
  case PROC:
  case FUNCTION:
	fprintf(db_out, "Routine:\t");
	break;
  case VAR:
  case REGVAR:
  case LOCVAR:
  case VARPAR:
  case LBOUND:
  case UBOUND:
	fprintf(db_out, "Variable:\t");
	break;
  case FIELD:
	fprintf(db_out, "Field:\t\t");
	break;
  case FILESYM:
  case FILELINK:
	fprintf(db_out, "File:\t\t");
	break;
  default:
	assert(0);
  }
  pr_scopes(s->sy_scope);
  fprintf(db_out, "%s\n", s->sy_idf->id_text);
}

resolve_cross(tp)
  p_type	tp;
{
  register p_symbol	sym = tp->ty_sym->sy_idf->id_def;

  while (sym) {
	if (sym->sy_class == TAG &&
	    sym->sy_type->ty_class == T_CROSS &&
	    sym->sy_type->ty_cross == (p_type) 0 &&
	    sym->sy_type->ty_size == tp->ty_class &&
	    scope_encloses(tp->ty_sym->sy_scope, sym->sy_scope)) {
		sym->sy_type->ty_cross = tp;
		sym->sy_type->ty_size = tp->ty_size;
	}
	sym = sym->sy_next;
  }
}
