"""Run with: python3 -m unittest discover -s util/LLgen -p 'test_*.py'."""

import unittest

from formatter import ParseError, format_source, parse


def walk(node):
    yield node
    for child in node.children:
        yield from walk(child)


class ParserTests(unittest.TestCase):
    def test_lossless_comments_and_roles(self):
        source = '''/* header */\r
%start parser, rule; /* declaration */
{ int global; /* C comment */ }
rule(int n; char *s;) /* parameters */
{ int local; }
:
    /* alternative */ %default %if (n && f(")"))
    [ %while (n > 0) %persistent
        symbol(n, (int[]){1, 2}) /* symbol */
        { if (n) { puts("}"); } /* } */ n--; }
    | ']' /* empty action follows */ {}
    ]*3
| /* empty alternative */
;
/* trailer */
'''
        doc = parse(source)
        self.assertEqual(doc.reconstruct(), source)
        comments = [t.text for t in doc.tokens if t.kind == 'comment']
        self.assertIn('/* alternative */', comments)
        self.assertIn('/* trailer */', comments)
        self.assertNotIn('/* C comment */', comments)
        kinds = {n.kind for n in walk(doc.tree)}
        self.assertTrue({'c_global', 'c_locals', 'c_parameters', 'c_arguments',
                         'c_action', 'c_condition', 'group', 'repeat'} <= kinds)
        # Tokens cover the entire source with neither gaps nor overlap.
        offset = 0
        for token in doc.tokens:
            self.assertEqual(token.start, offset)
            self.assertEqual(source[token.start:token.end], token.text)
            offset = token.end
        self.assertEqual(offset, len(source))

    def test_identifiers_named_like_token_kinds(self):
        parse('number: number(x) | identifier | literal;')

    def test_extensions_and_repeats(self):
        parse("r: %illegal %erroneous TOKEN? | %substart a, b; {}+2 | 'x'4;")

    def test_line_directive_and_c_line_comment(self):
        doc = parse('# 10 "input.g"\nr: { // }\\\n still a comment }\n f(); };')
        self.assertEqual(doc.tokens[0].kind, 'line_directive')
        self.assertEqual(len([n for n in walk(doc.tree) if n.kind == 'c_action']), 1)

    def test_errors(self):
        for source in ('r: {', 'r: { f(]; };', 'r: /* unfinished',
                       "r: 'unfinished", 'r: [ x;', 'r: x',
                       '%unknown x;', 'r: %while (x) y;'):
            with self.subTest(source=source), self.assertRaises(ParseError):
                parse(source)
        with self.assertRaises(ParseError) as error:
            parse('\nrule: @;')
        self.assertEqual((error.exception.line, error.exception.column), (2, 7))


class FormatterTests(unittest.TestCase):
    def test_layout(self):
        self.assertEqual(
            format_source('%token A,B; r(int x;) {int y;}: [%while(x) %persistent A(x)| B]*3 {f();}|;'),
            "%token A, B;\n\nr(/* C parameters */)\n"
            "    { /* C locals */ }\n    : [\n"
            "        %while (/* C condition */)\n        %persistent\n"
            "        A(/* C arguments */)\n        | B\n    ]*3\n"
            "        { /* C action */ }\n    |\n    ;\n")

    def test_alternative_introducers_share_first_symbol(self):
        output = format_source('r: first second | third | [nested | other];')
        self.assertEqual(output, 'r\n    : first second\n    | third\n    | [\n'
                         '        nested\n        | other\n    ]\n    ;\n')
        self.assertEqual(format_source(output), output)

    def test_plain_symbols_group_and_parameterised_symbols_stand_alone(self):
        source = "r: A B call(x) C D other(y) | '#' INTVALUE STRVALUE EOLN;"
        output = format_source(source)
        self.assertEqual(output,
                         "r\n    : A B\n    call(/* C arguments */)\n"
                         "    C D\n    other(/* C arguments */)\n"
                         "    | '#' INTVALUE STRVALUE EOLN\n    ;\n")
        self.assertEqual(format_source(output), output)

    def test_operators_comments_and_actions_separate_symbol_runs(self):
        source = 'r: A B? C D /* keep */ E F {f();} G H [I J | K L]+ M N;'
        output = format_source(source)
        self.assertIn(': A\n    B?\n    C D\n', output)
        self.assertIn('/* keep */\n    E F\n        { /* C action */ }\n    G H\n', output)
        self.assertIn('        I J\n        | K L\n    ]+\n    M N\n', output)
        self.assertEqual(format_source(output), output)

    def test_comments_preserved_in_order_and_c_replaced(self):
        source = ("/* header */ { int secret; /* inside C */ }\n"
                  "r /* before params */ (int n;) /* locals */ {int x;} :\n"
                  "/* first */ x /* arguments */ (n) [/* group */ 'x'\n"
                  "| /* empty */ ] /* suffix */ +2 // trailing\n"
                  "{secret();} ; /* end */")
        output = format_source(source)
        before = parse(source)
        after = parse(output)
        for kind in ('comment', 'line_directive'):
            self.assertEqual([t.text for t in before.tokens if t.kind == kind],
                             [t.text for t in after.tokens if t.kind == kind])
        self.assertNotIn('secret', output)
        self.assertNotIn('inside C', output)
        self.assertEqual(format_source(output), output)

    def test_extensions_literals_and_directives(self):
        source = ('# 1 "test.g"\n%start parser, r;\n'
                  "r: %illegal %erroneous TOKEN? | %substart a,b; {}+2 | ';'4;")
        output = format_source(source)
        self.assertTrue(output.startswith('# 1 "test.g"\n'))
        self.assertIn('%substart a, b;\n        { /* C action */ }+2', output)
        self.assertEqual(format_source(output), output)

    def test_actions_have_one_extra_indent_at_every_grammar_depth(self):
        output = format_source('{int global;} r {int local;}: {} | [A {} [B {}]];')
        actions = [line for line in output.splitlines() if 'C action' in line]
        self.assertEqual(actions, ['    ' * depth + '{ /* C action */ }'
                                   for depth in (2, 3, 4)])
        self.assertTrue(output.startswith('{ /* C global */ }\n'))
        self.assertIn('\n    { /* C locals */ }\n', output)
        self.assertEqual(format_source(output), output)

    def test_empty_and_comments_only(self):
        self.assertEqual(format_source(''), '')
        self.assertEqual(format_source('  /* retained */  '), '/* retained */\n')


if __name__ == '__main__':
    unittest.main()
