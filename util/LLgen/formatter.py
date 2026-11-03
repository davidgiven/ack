#!/usr/bin/env python3
"""Lossless LLgen parser; no formatting is performed.

Run ``python3 util/LLgen/formatter.py file.g`` to inspect the syntax tree as
JSON. --tokens includes whitespace and grammar comments in the output.

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


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('file', type=Path)
    ap.add_argument('--tokens', action='store_true', help='include lossless token stream')
    args = ap.parse_args()
    try:
        with args.file.open(encoding='utf-8', errors='surrogateescape', newline='') as stream:
            document = parse(stream.read())
    except (OSError, ParseError) as error:
        ap.exit(1, f'{args.file}:{error}\n')
    result = {'tree': asdict(document.tree)}
    if args.tokens:
        result['tokens'] = [asdict(t) for t in document.tokens]
    print(json.dumps(result, indent=2, ensure_ascii=True))


if __name__ == '__main__':
    main()
