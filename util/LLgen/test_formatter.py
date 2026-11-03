"""Run with: python3 -m unittest discover -s util/LLgen -p 'test_*.py'."""

import unittest
from unittest.mock import patch
from pathlib import Path
import subprocess
import sys
import tempfile

from formatter import (FormatError, ParseError, c_top_level, format_global_c,
                       format_actions, format_source, parse)


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
    def test_formal_parameter_separators_and_nested_declarators(self):
        source = ('r(int *x; void (*callback)(int,char); char *s;) : '
                  'symbol(f( 1 , 2 ), ";,", (int[]){1,2}); '
                  's(int *x; char *s) : TOKEN;')
        output = format_source(source)
        self.assertIn('r(int* x; void (*callback)(int, char); char* s;)', output)
        self.assertIn('s(int* x; char* s)', output)
        self.assertIn('symbol(f(1, 2), ";,", (int[]) { 1, 2 })', output)
        self.assertEqual(format_source(output), output)

    def test_parameter_comments_and_optional_trailing_semicolons(self):
        source = ('r(int *x; // parameter comment\n int y; /* tail */) : '
                  'symbol(";", f(1,2)); empty(): symbol();')
        output = format_source(source)
        for doc in (parse(source), parse(output)):
            params = [t for t in doc.tokens if t.kind == 'c_parens'][0]
            separators = [t for t, _, _ in c_top_level(params.text[1:-1]) if t in {';', ','}]
            self.assertEqual(separators, [';', ';'])
        self.assertIn('// parameter comment', output)
        self.assertIn('/* tail */', output)
        self.assertIn('empty()', output)
        self.assertIn('symbol()', output)
        self.assertEqual(format_source(output), output)

    def test_parenthesized_fragments_are_batched(self):
        source = 'r(int *x;): %if(a&&b) symbol( x+1 ) [%while(n>0) next( x )];'
        with patch('formatter.subprocess.run', wraps=subprocess.run) as run:
            output = format_source(source)
            self.assertEqual(run.call_count, 1)
        self.assertIn('r(int* x;)', output)
        self.assertIn('%if (a && b)', output)
        self.assertIn('%while (n > 0)', output)
        self.assertIn('symbol(x + 1)', output)
        self.assertNotIn('LLGEN_PARENS', output)
        self.assertEqual(format_source(output), output)

    def test_parenthesized_fragment_wrapping_uses_output_columns(self):
        source = ('very_long_rule_name(' + '; '.join(['int parameter_name_' + str(i)
                   for i in range(6)]) + ';) : %if (' +
                  ' && '.join(['long_condition_' + str(i) for i in range(8)]) +
                  ') long_nonterminal_name(' + ', '.join(['long_argument_' + str(i)
                   for i in range(9)]) + ');')
        output = format_source(source)
        self.assertTrue(all(len(line) <= 100 for line in output.splitlines()))
        self.assertIn('int parameter_name_5;', output)
        self.assertEqual(format_source(output), output)

    def test_comment_indentation_comes_from_grammar_scope(self):
        source = ('r: A;\n\n             /*\n                   global comment\n'
                  '                          */\n\ns: A [\n'
                  '/*\n                     nested comment\n */\n B] ;')
        output = format_source(source)
        self.assertIn(';\n\n/*\nglobal comment\n*/\n\ns', output)
        self.assertIn('        /*\n        nested comment\n        */', output)
        self.assertEqual(format_source(output), output)

    def test_old_block_comment_indentation_is_ignored(self):
        first = 'r: A /*\n          * text\n                          * more\n */ B;'
        second = 'r: A /*\n* text\n  * more\n                 */ B;'
        output = format_source(first)
        self.assertEqual(output, format_source(second))
        self.assertIn('    /*\n     * text\n     * more\n     */', output)
        self.assertEqual(format_source(output), output)

    def test_grammar_comments_are_reflowed_with_surrounding_blank_lines(self):
        sentence = 'A long explanatory comment with useful words.'
        source = 'r: A\n\n/* ' + (sentence + ' ') * 7 + '*/\n\nB;'
        output = format_source(source)
        self.assertIn(': A\n\n    /* ', output)
        self.assertIn(' */\n\n    B', output)
        self.assertIn('\n     * ', output)
        self.assertTrue(all(len(line) <= 100 for line in output.splitlines()))
        words = ' '.join(line.strip().lstrip('/*').rstrip('*/').strip()
                         for line in output.splitlines() if '/*' in line or line.lstrip().startswith('*'))
        self.assertEqual(words, ' '.join([sentence] * 7))
        self.assertEqual(format_source(output), output)

    def test_grammar_comment_alignment_and_batching(self):
        source = '/* header */\nr: A\n/*\n * first\n   * second\n*/\nB; // end'
        with patch('formatter.subprocess.run', wraps=subprocess.run) as run:
            output = format_source(source)
            self.assertEqual(run.call_count, 1)
        self.assertIn('    /*\n     * first\n     * second\n     */', output)
        self.assertIn('\n// end', output)
        self.assertNotIn('LLgenComment', output)
        self.assertEqual(format_source(output), output)

    def test_reflowed_line_comment_stays_stable(self):
        output = format_source('r: A;\n\n// ' + 'explanatory words ' * 20 + '\n\n')
        self.assertGreater(output.count('//'), 1)
        self.assertTrue(output.endswith('\n\n'))
        self.assertEqual(format_source(output), output)

    def test_local_declarations_are_expanded_and_batched_with_actions(self):
        source = ('r(int* p;) {int * q; int values[]={1,2};} : A {f();}; '
                  's {char * name;} : B;')
        with patch('formatter.subprocess.run', wraps=subprocess.run) as run:
            output = format_source(source)
            self.assertEqual(run.call_count, 2)
        self.assertIn('r(int* p;)\n    {\n        int* q;\n'
                      '        int values[] = { 1, 2 };\n    }\n', output)
        self.assertIn('s\n    {\n        char* name;\n    }\n', output)
        self.assertIn('        { f(); }', output)
        self.assertEqual(format_source(output), output)

    def test_single_and_multiple_statement_actions(self):
        output = format_source('r: A {x=1;} B {x=1;y=2;} [C {f();g();}];')
        self.assertIn('        { x = 1; }\n', output)
        self.assertIn('        {\n            x = 1;\n            y = 2;\n        }', output)
        self.assertIn('            {\n                f();\n                g();\n            }', output)
        self.assertEqual(format_source(output), output)

    def test_single_control_statement_and_delimiters(self):
        output = format_source(
            'r: {for(i=0;i<3;i++){f();}} {if(x) f(); else g();} '
            '{puts("; // }");} {int a[]={1,2};} {for(;;) f(); g();};')
        self.assertIn('{ for (i = 0; i < 3; i++) { f(); } }', output)
        self.assertIn('{ if (x) f(); else g(); }', output)
        self.assertIn('{ puts("; // }"); }', output)
        self.assertIn('{ int a[] = { 1, 2 }; }', output)
        self.assertIn('        {\n            for (;;)', output)
        self.assertEqual(format_source(output), output)

    def test_action_comments_and_long_statements_keep_multiline_layout(self):
        long_call = 'f(' + ', '.join(['very_long_argument'] * 10) + ');'
        output = format_source('r: { // keep\nf(); } {' + long_call + '};')
        self.assertIn('        {\n            // keep\n            f();\n        }', output)
        self.assertTrue(all(len(line) <= 100 for line in output.splitlines()))
        self.assertEqual(format_source(output), output)

    def test_actions_are_batched_in_one_clang_format_call(self):
        with patch('formatter.subprocess.run', wraps=subprocess.run) as run:
            output = format_source('r: {f();} {g();h();} [A {k();}];')
            self.assertEqual(run.call_count, 1)
        self.assertNotIn('LLgenAction', output)
        self.assertNotIn('#if 1', output)

    def test_action_comments_and_leading_whitespace_are_stable(self):
        source = ('r: {f(); /* keep */} {  \n f();g(); } '
                  '{ /* explains code */\n#ifdef FEATURE\nf();\n#endif\ng(); };')
        output = format_source(source)
        self.assertIn('{ f(); /* keep */ }', output)
        self.assertIn('            /* explains code */', output)
        self.assertEqual(format_source(output), output)

    def test_layout(self):
        self.assertEqual(
            format_source('%token A,B; r(int x;) {int y;}: [%while(x) %persistent A(x)| B]*3 { f(); }|;'),
            "%token A, B;\n\nr(int x;)\n"
            "    {\n        int y;\n    }\n    : [\n"
            "        %while (x)\n        %persistent\n"
            "        A(x)\n        | B\n    ]*3\n"
            "        { f(); }\n    |\n    ;\n")

    def test_alternative_introducers_share_first_symbol(self):
        output = format_source('r: first second | third | [nested | other];')
        self.assertEqual(output, 'r\n    : first second\n    | third\n    | [\n'
                         '        nested\n        | other\n    ]\n    ;\n')
        self.assertEqual(format_source(output), output)

    def test_conditions_are_formatted(self):
        condition = '(x  && f(")", /* keep */ (int[]){1, 2})\n || y)'
        source = ('r: %if ' + condition + ' A {hidden();} | '
                  '[%while (n-- > 0) B];')
        output = format_source(source)
        self.assertIn('%if (x && f(")", /* keep */ (int[]) { 1, 2 }) || y)', output)
        self.assertIn('%while (n-- > 0)', output)
        self.assertIn('{ hidden(); }', output)
        self.assertEqual(format_source(output), output)

    def test_plain_symbols_group_and_parameterised_symbols_stand_alone(self):
        source = "r: A B call(x) C D other(y) | '#' INTVALUE STRVALUE EOLN;"
        output = format_source(source)
        self.assertEqual(output,
                         "r\n    : A B\n    call(x)\n"
                         "    C D\n    other(y)\n"
                         "    | '#' INTVALUE STRVALUE EOLN\n    ;\n")
        self.assertEqual(format_source(output), output)

    def test_operators_comments_and_actions_separate_symbol_runs(self):
        source = 'r: A B? C D /* keep */ E F { f(); } G H [I J | K L]+ M N;'
        output = format_source(source)
        self.assertIn(': A\n    B?\n    C D\n', output)
        self.assertIn('/* keep */\n    E F\n        { f(); }\n    G H\n', output)
        self.assertIn('        I J\n        | K L\n    ]+\n    M N\n', output)
        self.assertEqual(format_source(output), output)

    def test_comments_and_c_preserved_in_order(self):
        source = ("/* header */ { int secret; /* inside C */ }\n"
                  "r /* before params */ (int n;) /* locals */ {int x;} :\n"
                  "/* first */ x /* arguments */ (n) [/* group */ 'x'\n"
                  "| /* empty */ ] /* suffix */ +2 // trailing\n"
                  "{secret();} ; /* end */")
        output = format_source(source)
        before = parse(source)
        after = parse(output)
        globals_ = {n.start for n in before.tree.children if n.kind == 'c_global'}
        actions = {n.start for n in walk(before.tree) if n.kind == 'c_action'}
        locals_ = {n.start for n in walk(before.tree) if n.kind == 'c_locals'}
        for kind in ('comment', 'line_directive', 'c_block', 'c_parens'):
            self.assertEqual([format_global_c(t.text) if i in globals_ else
                              format_actions([(i, t.text, 2)])[i] if i in actions else
                              format_actions([(i, t.text, 1)], multiline={i})[i]
                              if i in locals_ else t.text
                              for i, t in enumerate(before.tokens) if t.kind == kind],
                             [t.text for t in after.tokens if t.kind == kind])
        self.assertIn('secret', output)
        self.assertIn('inside C', output)
        self.assertEqual(format_source(output), output)

    def test_extensions_literals_and_directives(self):
        source = ('# 1 "test.g"\n%start parser, r;\n'
                  "r: %illegal %erroneous TOKEN? | %substart a,b; {}+2 | ';'4;")
        output = format_source(source)
        self.assertTrue(output.startswith('# 1 "test.g"\n'))
        self.assertIn('%substart a, b;\n        {}+2', output)
        self.assertEqual(format_source(output), output)

    def test_actions_have_one_extra_indent_at_every_grammar_depth(self):
        output = format_source('{int global;} r {int local;}: {} | [A {} [B {}]];')
        actions = [line for line in output.splitlines() if line.strip() == '{}']
        self.assertEqual(actions, ['    ' * depth + '{}'
                                   for depth in (2, 3, 4)])
        self.assertTrue(output.startswith('{\n    int global;\n}\n'))
        self.assertIn('\n    {\n        int local;\n    }\n', output)
        self.assertEqual(format_source(output), output)

    def test_c_fragments_use_project_style(self):
        source = ('{int * p; void f(){if(1){g();}}} '
                  'r(int * p;) {int * q;} : symbol( p ) {if(1){g();}}; '
                  '{int  another;}')
        output = format_source(source)
        self.assertTrue(output.startswith('{\n    int* p;\n    void f()\n    {\n'))
        self.assertIn('\n        if (1)\n        {\n            g();\n', output)
        self.assertIn('r(int* p;)', output)
        self.assertIn('    {\n        int* q;\n    }', output)
        self.assertIn('symbol(p)', output)
        self.assertIn('        { if (1) { g(); } }', output)
        self.assertIn('{\n    int another;\n}', output)
        self.assertEqual(format_source(output), output)

    def test_clang_format_failures_and_no_global_blocks(self):
        with patch('formatter.subprocess.run', side_effect=FileNotFoundError):
            self.assertEqual(format_source('r: A;'), 'r\n    : A\n    ;\n')
            with self.assertRaisesRegex(FormatError, 'not found'):
                format_source('{int x;}')
        error = subprocess.CalledProcessError(1, 'clang-format', stderr=b'bad config')
        with patch('formatter.subprocess.run', side_effect=error):
            with self.assertRaisesRegex(FormatError, 'bad config'):
                format_source('{int x;}')

    def test_global_directives_and_wrapping_include_indentation(self):
        source = ('{\n#include "header.h"\n#define LONG_MACRO ' +
                  ' + '.join(['a_very_long_identifier'] * 6) +
                  '\n#if ENABLED\nint *p;\n#endif\n}')
        output = format_global_c(source)
        for directive in ('#include "header.h"', '#define LONG_MACRO',
                          '#if ENABLED', '#endif'):
            self.assertIn('\n    ' + directive, output)
        self.assertIn('\\\n', output)
        self.assertTrue(all(len(line) <= 100 for line in output.splitlines()))
        self.assertNotIn('\t', output)
        self.assertNotIn('namespace LLgenGlobal', output)
        self.assertNotIn('#if 1\n', output)
        self.assertEqual(format_global_c(output), output)

    def test_global_empty_block_and_wrapper_validation(self):
        self.assertEqual(format_global_c('{}'), '{\n}')
        result = subprocess.CompletedProcess('clang-format', 0, b'broken wrapper\n')
        with patch('formatter.subprocess.run', return_value=result):
            with self.assertRaisesRegex(FormatError, 'wrapper'):
                format_global_c('{int x;}')

    def test_empty_and_comments_only(self):
        self.assertEqual(format_source(''), '')
        self.assertEqual(format_source('  /* retained */  '), '/* retained */\n')

    def test_blank_lines_surrounding_comments_are_preserved(self):
        source = ('\n\n/* header */\n\n\nr: A\n\n/* middle */\n\n\nB;\n'
                  '/* between rules */\ns: C;\n\n// trailer\n\n\n')
        output = format_source(source)
        self.assertTrue(output.startswith('\n\n/* header */\n\n\nr\n'))
        self.assertIn(': A\n\n    /* middle */\n\n\n    B', output)
        self.assertIn(';\n/* between rules */\ns\n', output)
        self.assertTrue(output.endswith(';\n\n// trailer\n\n\n'))
        self.assertEqual(format_source(output), output)

    def test_blank_lines_between_comments_and_crlf_input(self):
        source = '/* first */\r\n \t\r\n\r\n/* second */\r\n\r\nr: A;'
        output = format_source(source)
        self.assertTrue(output.startswith('/* first */\n\n\n/* second */\n\nr\n'))
        self.assertEqual(format_source(output), output)

    def test_inline_comments_do_not_gain_blank_lines(self):
        output = format_source('r: A /* one *//* two */ B;/* end */')
        self.assertNotIn('\n\n', output)
        self.assertEqual(format_source(output), output)

    def test_parameters_and_arguments_are_formatted(self):
        source = ('r(int x;\n char *s;) {\n\tint local;\n} :\n'
                  'call(x,\n s) {\n#if ENABLED\n\tf("}"); // keep\n#endif\n};')
        output = format_source(source)
        self.assertIn('r(int x; char* s;)', output)
        self.assertIn('    {\n        int local;\n    }', output)
        self.assertIn('call(x, s)', output)
        self.assertIn('            f("}"); // keep', output)
        self.assertIn('#if ENABLED', output)
        self.assertEqual(format_source(output), output)


class CommandLineTests(unittest.TestCase):
    def run_formatter(self, path, *options):
        return subprocess.run(
            [sys.executable, str(Path(__file__).with_name('formatter.py')), *options, str(path)],
            capture_output=True, text=True)

    def test_multiple_files_stdout_and_inplace(self):
        with tempfile.TemporaryDirectory() as directory:
            first = Path(directory) / 'first.g'
            second = Path(directory) / 'second.g'
            first.write_text('first: A;')
            second.write_text('second: B C;')
            result = self.run_formatter(second, str(first))
            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertEqual(result.stdout, format_source('first: A;') +
                             format_source('second: B C;'))
            self.assertEqual(first.read_text(), 'first: A;')
            self.assertEqual(second.read_text(), 'second: B C;')
            result = self.run_formatter(second, '-i', str(first))
            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertEqual(result.stdout, '')
            self.assertEqual(first.read_text(), format_source('first: A;'))
            self.assertEqual(second.read_text(), format_source('second: B C;'))

    def test_file_errors_do_not_prevent_other_files_being_formatted(self):
        with tempfile.TemporaryDirectory() as directory:
            good = Path(directory) / 'good.g'
            bad = Path(directory) / 'bad.g'
            missing = Path(directory) / 'missing.g'
            good.write_text('good: A;')
            bad.write_text('bad: [;')
            result = self.run_formatter(good, '-i', str(bad), str(missing))
            self.assertEqual(result.returncode, 1)
            self.assertIn(str(bad), result.stderr)
            self.assertIn(str(missing), result.stderr)
            self.assertEqual(good.read_text(), format_source('good: A;'))
            self.assertEqual(bad.read_text(), 'bad: [;')

    def test_inplace_aliases_and_default_stdout(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / 'input.g'
            source = b'/* comment */\n\nr: A B { puts("\xff"); };'
            expected = format_source(source.decode('utf-8', 'surrogateescape')).encode(
                'utf-8', 'surrogateescape')
            for option in ('--inplace', '-i'):
                path.write_bytes(source)
                result = self.run_formatter(path, option)
                self.assertEqual(result.returncode, 0, result.stderr)
                self.assertEqual(result.stdout, '')
                self.assertEqual(path.read_bytes(), expected)
                result = self.run_formatter(path, option)
                self.assertEqual(result.returncode, 0, result.stderr)
                self.assertEqual(path.read_bytes(), expected)
            path.write_text('r: A;')
            result = self.run_formatter(path)
            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertEqual(result.stdout, format_source('r: A;'))
            self.assertEqual(path.read_text(), 'r: A;')

    def test_errors_leave_input_unchanged(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / 'input.g'
            path.write_text('r: [ unfinished;')
            result = self.run_formatter(path, '-i')
            self.assertNotEqual(result.returncode, 0)
            self.assertEqual(path.read_text(), 'r: [ unfinished;')
            for option in ('--tree', '--tokens'):
                result = self.run_formatter(path, '-i', option)
                self.assertEqual(result.returncode, 2)
                self.assertIn('cannot be combined', result.stderr)
                self.assertEqual(path.read_text(), 'r: [ unfinished;')


if __name__ == '__main__':
    unittest.main()
