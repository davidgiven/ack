"""Run with: python3 -m unittest discover -s util/LLgen -p 'test_*.py'."""

import unittest

from formatter import ParseError, parse


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


if __name__ == '__main__':
    unittest.main()
