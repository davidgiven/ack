#!/usr/bin/env python3
"""LLgen formatter using clang-format for embedded C and grammar comments.

Run ``python3 util/LLgen/formatter.py file.g [file.g ...]`` to write formatted LLgen to
stdout. Embedded C and grammar comments use the project's .clang-format.
clang-format must be on PATH.
--tree prints the syntax tree
as JSON; --tokens also includes whitespace and grammar comments.
Files are formatted independently in argument order. Use --inplace / -i to
update each input file instead of writing to stdout.

parse(source) returns a Document with an immutable token stream and a syntax
tree. Token offsets are Python string offsets; node ranges are half-open token
indices. Every source character belongs to a token, including trailing trivia.
C fragments include their delimiters and are deliberately not parsed as C.
"""

import argparse
from dataclasses import asdict, dataclass
import json
from pathlib import Path
import re
import subprocess
import sys


@dataclass(frozen=True)
class Token:
    kind: str
    text: str
    start: int
    end: int


@dataclass(frozen=True)
class Node:
    kind: str
    start: int
    end: int
    children: tuple = ()


@dataclass(frozen=True)
class Document:
    source: str
    tokens: tuple
    tree: Node

    def text(self, node):
        """Return the original spelling of a node, including internal trivia."""
        return ''.join(t.text for t in self.tokens[node.start:node.end])

    def reconstruct(self):
        return ''.join(t.text for t in self.tokens)


class ParseError(ValueError):
    def __init__(self, source, offset, message):
        self.offset = offset
        self.line = source.count('\n', 0, offset) + 1
        self.column = offset - source.rfind('\n', 0, offset)
        super().__init__(f'{self.line}:{self.column}: {message}')


class Lexer:
    def __init__(self, source):
        self.source = source
        self.pos = 0

    def fail(self, offset, message):
        raise ParseError(self.source, offset, message)

    def quoted(self):
        start = self.pos
        quote = self.source[self.pos]
        self.pos += 1
        while self.pos < len(self.source):
            ch = self.source[self.pos]
            self.pos += 1
            if ch == quote:
                return
            if ch == '\\':
                self.pos += 1
            elif ch in '\r\n':
                self.fail(start, 'newline in quoted literal')
        self.fail(start, 'unterminated quoted literal')

    def comment(self):
        start = self.pos
        if self.source.startswith('/*', start):
            end = self.source.find('*/', start + 2)
            if end < 0:
                self.fail(start, 'unterminated comment')
            self.pos = end + 2
        else:
            # Include continued lines in C // comments.
            self.pos += 2
            while self.pos < len(self.source):
                if self.source.startswith('\\\r\n', self.pos):
                    self.pos += 3
                elif self.source.startswith('\\\n', self.pos):
                    self.pos += 2
                elif self.source[self.pos] in '\r\n':
                    break
                else:
                    self.pos += 1

    def c_fragment(self):
        """Match nested C delimiters, ignoring those in comments/literals."""
        stack = []
        pairs = {'{': '}', '(': ')', '[': ']'}
        while self.pos < len(self.source):
            ch = self.source[self.pos]
            if self.source.startswith(('/*', '//'), self.pos):
                self.comment()
                continue
            if ch in "\"'":
                self.quoted()
                continue
            if ch in pairs:
                stack.append((pairs[ch], self.pos))
            elif ch in '})]':
                if ch != stack[-1][0]:
                    self.fail(self.pos, f'expected {stack[-1][0]!r}, found {ch!r}')
                stack.pop()
                if not stack:
                    self.pos += 1
                    return
            self.pos += 1
        expected, start = stack[-1]
        self.fail(start, f'unterminated C fragment; expected {expected!r}')

    def scan(self):
        tokens = []
        while self.pos < len(self.source):
            start = self.pos
            ch = self.source[start]
            if ch.isspace():
                kind = 'whitespace'
                while self.pos < len(self.source) and self.source[self.pos].isspace():
                    self.pos += 1
            elif self.source.startswith(('/*', '//'), start):
                kind = 'comment'
                self.comment()
            elif ch in '{(':
                kind = 'c_block' if ch == '{' else 'c_parens'
                self.c_fragment()
            elif ch == "'":
                kind = 'literal'
                self.quoted()
            elif ch == '#' and not self.source[self.source.rfind('\n', 0, start) + 1:start].strip():
                kind = 'line_directive'
                end = self.source.find('\n', start)
                self.pos = len(self.source) if end < 0 else end
            else:
                match = re.match(r'%?[A-Za-z_][A-Za-z_0-9]*|[0-9]+', self.source[start:])
                if match:
                    text = match.group()
                    kind = 'directive' if text.startswith('%') else 'number' if text.isdigit() else 'identifier'
                    self.pos += len(text)
                elif ch in ':;|[]*+?,':
                    kind = ch
                    self.pos += 1
                else:
                    self.fail(start, f'unexpected character {ch!r}')
            tokens.append(Token(kind, self.source[start:self.pos], start, self.pos))
        return tuple(tokens)


class Parser:
    TRIVIA = {'whitespace', 'comment', 'line_directive'}

    def __init__(self, source):
        self.source = source
        self.tokens = Lexer(source).scan()
        self.pos = 0

    def peek(self):
        while self.pos < len(self.tokens) and self.tokens[self.pos].kind in self.TRIVIA:
            self.pos += 1
        return self.tokens[self.pos] if self.pos < len(self.tokens) else Token('eof', '', len(self.source), len(self.source))

    def at(self, value):
        t = self.peek()
        return t.kind == value or (value.startswith('%') and t.text.lower() == value)

    def take(self, value):
        t = self.peek()
        if not self.at(value):
            raise ParseError(self.source, t.start, f'expected {value}, found {t.text!r}' if t.text else f'expected {value}, found end of file')
        start = self.pos
        self.pos += 1
        return Node(t.kind, start, self.pos)

    def c(self, role, kind):
        token = self.take(kind)
        return Node(role, token.start, token.end)

    def declaration(self):
        start = self.pos
        directive = self.peek().text.lower()
        self.take('directive')
        children = [self.take('identifier')]
        if directive in {'%start', '%first'}:
            self.take(',')
            children.append(self.take('identifier'))
        elif directive == '%token':
            while self.at(','):
                self.take(',')
                children.append(self.take('identifier'))
        elif directive not in {'%lexical', '%prefix', '%onerror'}:
            raise ParseError(self.source, self.tokens[start].start, f'unknown declaration {directive}')
        self.take(';')
        return Node('declaration', start, self.pos, tuple(children))

    def rule(self):
        start = self.pos
        children = [self.take('identifier')]
        if self.at('c_parens'):
            children.append(self.c('c_parameters', 'c_parens'))
        if self.at('c_block'):
            children.append(self.c('c_locals', 'c_block'))
        self.take(':')
        children.append(self.productions())
        self.take(';')
        return Node('rule', start, self.pos, tuple(children))

    def productions(self):
        start = self.pos
        children = [self.alternative()]
        while self.at('|'):
            self.take('|')
            children.append(self.alternative())
        return Node('alternatives', start, self.pos, tuple(children))

    def alternative(self):
        self.peek()
        start = self.pos
        children = []
        if self.at('%default'):
            children.append(self.take('%default'))
        if self.at('%if'):
            children.append(self.take('%if'))
            children.append(self.c('c_condition', 'c_parens'))
        elif self.at('%prefer') or self.at('%avoid'):
            children.append(self.take('directive'))
        if self.at('%illegal'):
            children.append(self.take('%illegal'))
        while not any(self.at(k) for k in ('|', ']', ';', 'eof')):
            children.append(self.element())
        return Node('alternative', start, self.pos, tuple(children))

    def element(self):
        self.peek()
        start = self.pos
        children = []
        if self.at('['):
            self.take('[')
            if self.at('%while'):
                children.append(self.take('%while'))
                children.append(self.c('c_condition', 'c_parens'))
            if self.at('%persistent'):
                children.append(self.take('%persistent'))
            children.append(self.productions())
            self.take(']')
            kind = 'group'
        elif self.at('identifier') or self.at('literal') or self.at('%erroneous'):
            if self.at('%erroneous'):
                children.append(self.take('%erroneous'))
            children.append(self.take('literal' if self.at('literal') else 'identifier'))
            if self.at('c_parens') and children[-1].kind == 'identifier':
                children.append(self.c('c_arguments', 'c_parens'))
            kind = 'symbol'
        else:
            if self.at('%substart'):
                children.append(self.take('%substart'))
                children.append(self.take('identifier'))
                while self.at(','):
                    self.take(',')
                    children.append(self.take('identifier'))
                self.take(';')
            children.append(self.c('c_action', 'c_block'))
            kind = 'action'
        node = Node(kind, start, self.pos, tuple(children))
        if any(self.at(k) for k in ('*', '+', '?', 'number')):
            suffix = self.take(self.peek().kind)
            suffixes = [suffix]
            if self.tokens[suffix.start].kind in {'*', '+'} and self.at('number'):
                suffixes.append(self.take('number'))
            node = Node('repeat', start, self.pos, (node, *suffixes))
        return node

    def parse(self):
        children = []
        while not self.at('eof'):
            if self.at('c_block'):
                children.append(self.c('c_global', 'c_block'))
            elif self.at('directive'):
                children.append(self.declaration())
            else:
                children.append(self.rule())
        return Document(self.source, self.tokens, Node('document', 0, len(self.tokens), tuple(children)))


def parse(source):
    """Parse LLgen syntax without discarding comments or changing C text."""
    return Parser(source).parse()


class FormatError(ValueError):
    pass


def clang_format(source, extra_style=''):
    root = Path(__file__).resolve().parents[2]
    style = ('{BasedOnStyle: InheritParentConfig, NamespaceIndentation: All, '
             'IndentPPDirectives: BeforeHash, PPIndentWidth: 4, UseTab: Never, '
             'FixNamespaceComments: false' + extra_style + '}')
    try:
        result = subprocess.run(
            ['clang-format', f'--style={style}',
             f'--assume-filename={root / "llgen-global.cpp"}'],
            input=source.encode('utf-8', 'surrogateescape'),
            capture_output=True, check=True)
    except FileNotFoundError as error:
        raise FormatError('clang-format executable not found on PATH') from error
    except subprocess.CalledProcessError as error:
        message = error.stderr.decode('utf-8', 'replace').strip()
        raise FormatError(f'clang-format failed: {message}') from error
    return result.stdout.decode('utf-8', 'surrogateescape')


def format_global_c(text):
    """Use a temporary namespace and conditional to indent code and directives.

    Both wrappers are removed after formatting. clang-format sees the final
    indentation while making line-wrapping decisions, including for macros.
    """
    wrapped = '#if 1\nnamespace LLgenGlobal\n{\n' + text[1:-1] + '\n}\n#endif\n'
    formatted = clang_format(wrapped)
    lines = formatted.splitlines(keepends=True)
    if ([line.rstrip('\r\n') for line in lines[:3]] !=
            ['#if 1', 'namespace LLgenGlobal', '{'] or
            [line.rstrip('\r\n') for line in lines[-2:]] != ['}', '#endif']):
        raise FormatError('clang-format did not preserve the global block wrapper')
    content = ''.join(lines[3:-2]).strip('\r\n')
    return '{\n' + (content + '\n' if content else '') + '}'


def single_c_statement(text):
    """Recognize a complete single statement for compact action layout.

    This counts statement boundaries, not semicolons inside delimiters. Unknown
    or incomplete constructs stay multiline. Line comments, multiline comments,
    continued literals and directives stay multiline to preserve their meaning.
    """
    lexer = Lexer(text)
    tokens = []
    while lexer.pos < len(text):
        ch = text[lexer.pos]
        if text.startswith(('//', '\\\n', '\\\r'), lexer.pos) or ch == '#':
            return False
        if text.startswith('/*', lexer.pos):
            start = lexer.pos
            lexer.comment()
            if '\n' in text[start:lexer.pos] or '\r' in text[start:lexer.pos]:
                return False
        elif ch.isspace():
            lexer.pos += 1
        elif ch in "\"'":
            start = lexer.pos
            lexer.quoted()
            if '\n' in text[start:lexer.pos] or '\r' in text[start:lexer.pos]:
                return False
            tokens.append(text[start:lexer.pos])
        else:
            match = re.match(r'[A-Za-z_][A-Za-z_0-9]*', text[lexer.pos:])
            token = match.group() if match else ch
            tokens.append(token)
            lexer.pos += len(token)

    def balanced(pos):
        close = {'(': ')', '[': ']', '{': '}'}[tokens[pos]]
        pos += 1
        while pos < len(tokens) and tokens[pos] != close:
            pos = balanced(pos) if tokens[pos] in {'(', '[', '{'} else pos + 1
        if pos == len(tokens):
            raise ValueError('unclosed delimiter')
        return pos + 1

    def statement(pos):
        head = tokens[pos]
        if head == '{':
            return balanced(pos)
        if head in {'if', 'for', 'while', 'switch'}:
            if tokens[pos + 1] != '(':
                raise ValueError('expected condition')
            end = statement(balanced(pos + 1))
            if head == 'if' and end < len(tokens) and tokens[end] == 'else':
                end = statement(end + 1)
            return end
        if head == 'do':
            end = statement(pos + 1)
            if tokens[end:end + 2] != ['while', '(']:
                raise ValueError('expected while')
            end = balanced(end + 1)
            if tokens[end] != ';':
                raise ValueError('expected semicolon')
            return end + 1
        while pos < len(tokens):
            if tokens[pos] == ';':
                return pos + 1
            pos = balanced(pos) if tokens[pos] in {'(', '[', '{'} else pos + 1
        raise ValueError('expected statement terminator')

    try:
        return bool(tokens) and statement(0) == len(tokens)
    except (ValueError, IndexError):
        return False


def format_actions(actions, *, multiline=()):
    """Batch function-scope blocks; indices in multiline keep expanded layout."""
    if not actions:
        return {}
    wrappers = []
    for index, text, level in actions:
        wrappers.append('#if 1\n' +
                        ''.join(f'namespace LLgenAction{index}_{n}\n{{\n'
                                for n in range(level)) +
                        f'void LLgenActionBody{index}()\n{{\n' +
                        '    ' * (level + 1) + text[1:-1].strip() + '\n}\n' +
                        '}\n' * level + '#endif\n')
    formatted = clang_format(''.join(wrappers))
    result = {}
    for index, text, level in actions:
        match = re.search(rf'\bvoid LLgenActionBody{index}\(\)\s*\{{', formatted)
        if not match:
            raise FormatError('clang-format did not preserve the action wrapper')
        start = match.end() - 1
        lexer = Lexer(formatted)
        lexer.pos = start
        lexer.c_fragment()
        block = formatted[start:lexer.pos]
        if not text[1:-1].strip():
            block = '{}'
        elif index not in multiline and single_c_statement(text[1:-1]):
            compact = ' '.join(line.strip() for line in block.splitlines())
            if len(compact) + 4 * level <= 100:
                block = compact
        result[index] = block
    return result


def format_comments(comments):
    """Batch grammar comments at their output indentation, preserving boundaries."""
    if not comments:
        return {}
    wrappers = []
    for index, text, level in comments:
        if text.startswith('/*'):
            # Historical comment indentation is unreliable. Give every line
            # the surrounding grammar's baseline; clang-format aligns stars.
            text = ('\n' + '    ' * level).join(
                line.lstrip(' \t') for line in text.splitlines())
        wrappers.append(
            ''.join(f'namespace LLgenComment{index}_{n}\n{{\n' for n in range(level)) +
            f'int LLgenCommentStart{index};\n\n' + '    ' * level + text +
            f'\n\nint LLgenCommentEnd{index};\n' + '}\n' * level)
    formatted = clang_format(''.join(wrappers))
    result = {}
    for index, _, _ in comments:
        match = re.search(
            rf'^[ \t]*int LLgenCommentStart{index};\r?\n(.*?)'
            rf'^[ \t]*int LLgenCommentEnd{index};', formatted, re.MULTILINE | re.DOTALL)
        if not match:
            raise FormatError('clang-format did not preserve the comment wrapper')
        result[index] = match.group(1).strip('\r\n')
    return result


def c_top_level(text):
    """Yield nontrivia C tokens, treating nested delimiters as opaque tokens."""
    lexer = Lexer(text)
    while lexer.pos < len(text):
        start = lexer.pos
        ch = text[start]
        if ch.isspace():
            lexer.pos += 1
            continue
        if text.startswith(('/*', '//'), start):
            lexer.comment()
            continue
        if ch in "\"'":
            lexer.quoted()
        elif ch in '({[':
            lexer.c_fragment()
        else:
            lexer.pos += 1
        yield text[start:lexer.pos], start, lexer.pos


def format_parentheses(fragments):
    """Batch conditions, actual arguments and LLgen formal parameter lists.

    Dummy names match the emitted prefix width, so clang-format sees the final
    opening-parenthesis column. Formal parameters temporarily use C separators.
    """
    if not fragments:
        return {}
    wrappers = []
    metadata = {}
    for index, text, role, level, width in fragments:
        name = 'P' * max(1, width)
        content = text[1:-1]
        separators = []
        trailing = False
        if role == 'c_parameters':
            tokens = list(c_top_level(content))
            trailing = bool(tokens and tokens[-1][0] == ';')
            trailing_pos = tokens[-1][1] if trailing else -1
            replacements = []
            for token, start, end in tokens:
                if token in {';', ','}:
                    if start == trailing_pos:
                        replacements.append((start, end, ''))
                    else:
                        separators.append(token)
                        replacements.append((start, end, ','))
            for start, end, replacement in reversed(replacements):
                content = content[:start] + replacement + content[end:]
        depth = level if role == 'c_parameters' else max(0, level - 1)
        namespaces = ''.join(f'namespace LLgenParen{index}_{n}\n{{\n'
                             for n in range(depth))
        if role == 'c_parameters':
            code = f'void\n{name}({content});\n'
        else:
            code = f'void LLgenParenBody{index}()\n{{\n{name}({content});\n}}\n'
        wrappers.append(f'// LLGEN_PARENS_{index}\n' + namespaces + code + '}\n' * depth)
        metadata[index] = (name, separators, trailing)
    formatted = clang_format(''.join(wrappers), ', AlwaysBreakAfterReturnType: All')
    result = {}
    for index, _, role, _, _ in fragments:
        name, separators, trailing = metadata[index]
        marker = re.search(rf'^\s*// LLGEN_PARENS_{index}\s*$', formatted, re.MULTILINE)
        if not marker:
            raise FormatError('clang-format did not preserve the parameter wrapper')
        match = re.search(rf'\b{name}\s*\(', formatted[marker.end():])
        if not match:
            raise FormatError('clang-format did not preserve the parameter wrapper')
        start = marker.end() + match.end() - 1
        lexer = Lexer(formatted)
        lexer.pos = start
        lexer.c_fragment()
        content = formatted[start + 1:lexer.pos - 1]
        if role == 'c_parameters':
            commas = [(a, b) for token, a, b in c_top_level(content) if token == ',']
            if len(commas) != len(separators):
                raise FormatError('clang-format changed the parameter separators')
            for (a, b), separator in reversed(list(zip(commas, separators))):
                content = content[:a] + separator + content[b:]
            if trailing:
                tokens = list(c_top_level(content))
                end = tokens[-1][2] if tokens else 0
                content = content[:end] + ';' + content[end:]
        result[index] = '(' + content + ')'
    return result


class Formatter:
    """Lay out grammar tokens using the tree and format C blocks and comments.

    Layout is attached to token indices so comments between syntax nodes are
    never lost. Embedded C is represented by a single token; global, local and
    action blocks are reformatted. The input Document remains unchanged.
    """

    def __init__(self, document):
        self.document = document
        self.tokens = document.tokens
        self.breaks = {}
        self.roles = {}
        self.lines = []
        self.line = ''
        self.level = 0

    def significant(self, node):
        return [i for i in range(node.start, node.end)
                if self.tokens[i].kind not in Parser.TRIVIA]

    def mark(self, index, level, blank=False):
        self.breaks[index] = (level, blank)

    def layout(self, node, level=0):
        kind = node.kind
        indices = self.significant(node)
        if kind.startswith('c_'):
            self.roles[node.start] = kind
            if kind == 'c_action':
                self.mark(node.start, level + 1)
        if kind == 'document':
            previous = None
            for child in node.children:
                self.layout(child, 0)
                self.mark(child.start, 0, previous is not None and
                          (previous != 'declaration' or child.kind != 'declaration'))
                previous = child.kind
            return
        if kind == 'rule':
            colon = next(i for i in indices if self.tokens[i].kind == ':')
            self.mark(colon, level + 1)
            self.mark(indices[-1], level + 1)
            for child in node.children:
                self.layout(child, level + 1 if child.kind in {'alternatives', 'c_locals'} else level)
                if child.kind == 'c_locals':
                    self.mark(child.start, level + 1)
            return
        if kind == 'alternatives':
            for i in indices:
                if self.tokens[i].kind == '|':
                    # Nested alternatives will override their own separators.
                    self.mark(i, level)
            for child in node.children:
                self.layout(child, level)
            return
        if kind == 'alternative':
            previous_plain = False
            for child in node.children:
                plain = (child.kind == 'symbol' and len(child.children) == 1
                         and child.children[0].kind in {'identifier', 'literal'})
                if child.kind != 'c_condition' and not (plain and previous_plain):
                    self.mark(child.start, level)
                self.layout(child, level)
                previous_plain = plain
            return
        if kind == 'group':
            self.mark(indices[-1], level)
            for child in node.children:
                self.layout(child, level + 1)
                if child.kind not in {'alternatives', 'c_condition'}:
                    self.mark(child.start, level + 1)
            return
        for child in node.children:
            self.layout(child, level)

    def flush(self):
        if self.line:
            self.lines.append(self.line)
            self.line = ''

    def write(self, text, space=True):
        if not self.line:
            self.line = '    ' * self.level + text
        else:
            self.line += (' ' if space else '') + text

    def comment_gaps(self):
        """Map token boundaries adjoining comments to their blank-line counts."""
        indices = [-1] + [i for i, token in enumerate(self.tokens)
                          if token.kind != 'whitespace'] + [len(self.tokens)]
        gaps = {}
        for left, right in zip(indices, indices[1:]):
            if not ((left >= 0 and self.tokens[left].kind == 'comment') or
                    (right < len(self.tokens) and self.tokens[right].kind == 'comment')):
                continue
            whitespace = ''.join(t.text for t in self.tokens[left + 1:right])
            newlines = len(re.findall(r'\r\n|\r|\n', whitespace))
            gaps[right] = max(0, newlines - (left >= 0))
        return gaps

    def comment_levels(self):
        """Assign comments to grammar scopes instead of the previous output line."""
        levels = {i: 0 for i, token in enumerate(self.tokens) if token.kind == 'comment'}

        def visit(node, level):
            if node.kind in {'rule', 'group'}:
                level += 1
            for i in range(node.start, node.end):
                if i in levels:
                    levels[i] = level
            for child in node.children:
                visit(child, level)

        visit(self.document.tree, 0)
        return levels

    def format(self):
        self.layout(self.document.tree)
        blocks = format_actions(
            [(i, self.tokens[i].text, self.breaks[i][0])
             for i, role in self.roles.items() if role in {'c_action', 'c_locals'}],
            multiline={i for i, role in self.roles.items() if role == 'c_locals'})
        comments = format_comments([(i, self.tokens[i].text, level)
                                    for i, level in self.comment_levels().items()])
        comment_gaps = self.comment_gaps()
        parentheses = []
        previous = None
        for i, token in enumerate(self.tokens):
            if token.kind == 'whitespace':
                continue
            if i in self.breaks:
                # Start an alternative beside its introducer. Comments flush
                # the line themselves, so none are moved across grammar tokens.
                if (self.line.strip() not in {':', '|'} or token.kind in {'|', ']', ';'}
                        or self.roles.get(i) == 'c_action'):
                    self.flush()
                self.level, blank = self.breaks[i]
                if blank and i not in comment_gaps and self.lines and self.lines[-1]:
                    self.lines.append('')
            if i in comment_gaps:
                self.flush()
                self.lines.extend([''] * comment_gaps[i])
            if token.kind in {'comment', 'line_directive'}:
                self.flush()
                # Comments are already indented by clang-format. Line directives
                # stay at column 1; surrounding blank lines are handled separately.
                self.line = comments[i] if token.kind == 'comment' else token.text
                self.flush()
                previous = None
                continue
            role = self.roles.get(i)
            if role:
                if role in {'c_condition', 'c_parameters', 'c_arguments'}:
                    space = role == 'c_condition'
                    prefix = self.line.rsplit('\n', 1)[-1].lstrip(' ')
                    parentheses.append((i, token.text, role, self.level, len(prefix) + space))
                    text = f'@@LLGEN_PARENS_{i}@@'
                else:
                    text = (format_global_c(token.text) if role == 'c_global' else blocks[i])
                self.write(text, space=role not in {'c_parameters', 'c_arguments'})
            else:
                attach = token.kind in {',', ';', '*', '+', '?'} or (
                    token.kind == 'number' and previous in {']', '*', '+', 'identifier', 'literal', 'c_block', 'c_parens'})
                self.write(token.text, space=not attach)
            previous = token.kind
        self.flush()
        self.lines.extend([''] * comment_gaps.get(len(self.tokens), 0))
        output = '\n'.join(self.lines) + ('\n' if self.lines else '')
        for index, text in format_parentheses(parentheses).items():
            output = output.replace(f'@@LLGEN_PARENS_{index}@@', text, 1)
        return output


def format_document(document):
    """Format LLgen, grammar comments and all embedded C fragments."""
    return Formatter(document).format()


def format_source(source):
    return format_document(parse(source))


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('files', type=Path, nargs='+', metavar='file')
    ap.add_argument('--inplace', '-i', action='store_true', help='format each input file in place')
    ap.add_argument('--tree', action='store_true', help='print syntax tree instead of source')
    ap.add_argument('--tokens', action='store_true', help='include lossless token stream')
    args = ap.parse_args()
    if args.inplace and (args.tree or args.tokens):
        ap.error('--inplace cannot be combined with --tree or --tokens')
    status = 0
    for path in args.files:
        try:
            with path.open(encoding='utf-8', errors='surrogateescape', newline='') as stream:
                document = parse(stream.read())
            if not (args.tree or args.tokens):
                formatted = format_document(document)
                if args.inplace:
                    if formatted != document.source:
                        with path.open('w', encoding='utf-8', errors='surrogateescape', newline='') as stream:
                            stream.write(formatted)
                else:
                    print(formatted, end='')
            else:
                result = {'tree': asdict(document.tree)}
                if args.tokens:
                    result['tokens'] = [asdict(t) for t in document.tokens]
                print(json.dumps(result, indent=2, ensure_ascii=True))
        except (OSError, ParseError, FormatError) as error:
            print(f'{path}:{error}', file=sys.stderr)
            status = 1
    return status


if __name__ == '__main__':
    sys.exit(main())
