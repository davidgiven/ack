"""Validate a preprocessed EGO description when libconf is installed."""

import argparse
import re
import sys
from pathlib import Path

try:
    import libconf
except ImportError:
    print(
        "warning: libconf is not installed; skipping description schema validation. "
        "Install it with: python3 -m pip install libconf",
        file=sys.stderr,
    )
    sys.exit(0)


class SchemaError(ValueError):
    pass


class Group(dict):
    """Retain setting locations for schema diagnostics."""

    def __init__(self, line):
        super().__init__()
        self.line = line
        self.lines = {}


class Parser(libconf.Parser):
    # libconf's default dictionaries silently overwrite duplicate settings.
    def setting_list_or_empty(self):
        token = self.tokens.peek()
        result = Group(token.row if token else 1)
        while True:
            token = self.tokens.peek()
            setting = self.setting()
            if setting is None:
                return result
            name, value = setting
            if name in result:
                raise SchemaError(f"line {token.row}: duplicate setting {name!r}")
            result[name] = value
            result.lines[name] = token.row


def fail(path, line, message):
    raise SchemaError(f"{line}: {path or '<root>'}: {message}")


def integer(low, high):
    def check(value, path, line):
        # bool and libconf's explicit int64 values must not pass as C ints.
        if type(value) is not int:
            fail(path, line, "expected integer")
        if not low <= value <= high:
            fail(path, line, f"expected integer in {low}..{high}")
    return check


def boolean(value, path, line):
    if type(value) is not bool:
        fail(path, line, "expected boolean")


def enumeration(*choices):
    def check(value, path, line):
        if type(value) is not str or value not in choices:
            fail(path, line, f"expected one of {', '.join(choices)}")
    return check


def sequence(element, *, array=False, nonempty=False, unique=(), rules=False):
    def check(value, path, line):
        expected = list if array else tuple
        if type(value) is not expected:
            fail(path, line, "expected array" if array else "expected list")
        if nonempty and not value:
            fail(path, line, "must not be empty")
        seen = set()
        for index, item in enumerate(value):
            child = f"{path}[{index}]"
            item_line = getattr(item, "line", line)
            validate(item, element, child, item_line)
            if rules and ((item["when"] == "default") != (index == len(value) - 1)):
                fail(child, item_line, "default rule must occur exactly once, at the end")
            if unique:
                key = tuple(item[name] for name in unique)
                if key in seen:
                    fail(child, item_line, "duplicate rule or register case")
                seen.add(key)
    return check


def validate(value, schema, path="", line=1):
    if not isinstance(schema, dict):
        schema(value, path, line)
        return
    if not isinstance(value, Group):
        fail(path, line, "expected group")
    for name in value:
        if name not in schema:
            fail(f"{path}.{name}" if path else name, value.lines[name], "unknown setting")
    for name, child_schema in schema.items():
        child = f"{path}.{name}" if path else name
        if name not in value:
            fail(child, value.line, "required setting missing")
        validate(value[name], child_schema, child, value.lines[name])


COST = {"time": integer(-32768, 32767), "space": integer(-32768, 32767)}
REGISTER_CLASS = enumeration("general", "pointer", "float")


def rule_list(*conditions):
    return sequence(
        {"when": enumeration(*conditions), **COST},
        nonempty=True, unique=("when",), rules=True,
    )


RULES = rule_list("default", "fitbyte", "in_0_63", "in_0_8")
ACCESS_RULES = rule_list("default", "fitbyte")
SCORE_CASES = sequence(
    {"from": REGISTER_CLASS, "to": REGISTER_CLASS, "rules": RULES},
    unique=("from", "to"),
)
OPENING_CASES = sequence(
    {"register_class": REGISTER_CLASS, "rules": RULES}, unique=("register_class",),
)


def costs(cases):
    return {
        "local_variable": cases,
        "local_address": cases,
        "constant": RULES,
        "double_constant": RULES,
        "global_address": RULES,
        "procedure_address": RULES,
    }


def schema(instructions, word_size):
    def instruction(value, path, line):
        if type(value) is not int or value not in instructions:
            fail(path, line, "expected EM instruction number")

    policy = {
        "addressing_modes": sequence(instruction, array=True),
        "cheap_operations": sequence(instruction, array=True),
        "lexical_threshold": integer(0, 32767),
        "convert_remainder_to_division": boolean,
        "excluded_index_shift_counts": sequence(integer(0, 8 * word_size - 1), array=True),
        "forbidden_operations": sequence(instruction, array=True),
    }
    return {
        "word_size": integer(1, 32767),
        "pointer_size": integer(1, 32767),
        "register_allocation": {
            "registers": {
                "general": integer(0, 32767),
                "pointer": integer(0, 32767),
                "float": integer(0, 32767),
                "general_as_pointer": boolean,
            },
            "scores": costs(SCORE_CASES),
            "opening_costs": costs(OPENING_CASES),
            "save_costs": sequence(COST),
        },
        "constant_propagation": {"global_access": ACCESS_RULES, "local_access": ACCESS_RULES},
        "strength_reduction": {
            "overflow_harmful": boolean,
            "array_bounds_harmful": boolean,
            "shift_threshold": integer(0, 2147483647),
        },
        "common_subexpressions": {
            "time": policy, "space": policy, "indirection_limit": integer(0, 32767),
        },
        "stack_pollution": {"global_allowed": boolean},
    }


def check(description, header):
    with description.open(encoding="utf-8") as stream:
        config = Parser(libconf.TokenStream.from_file(stream)).parse()
    # Validate this before constructing the word-size-dependent shift constraint.
    if "word_size" not in config:
        fail("word_size", config.line, "required setting missing")
    integer(1, 32767)(config["word_size"], "word_size", config.lines["word_size"])
    instructions = {
        int(number) for number in re.findall(
            r"^#define\s+op_\w+\s+(\d+)\s*$", header.read_text(), re.MULTILINE
        )
    }
    if not instructions:
        raise SchemaError(f"no EM instruction definitions found in {header}")
    validate(config, schema(instructions, config["word_size"]))
    if config["pointer_size"] not in (config["word_size"], 2 * config["word_size"]):
        fail("pointer_size", config.lines["pointer_size"],
             "must equal word_size or twice word_size")
    allocation = config["register_allocation"]
    registers = allocation["registers"]
    total = sum(registers[name] for name in ("general", "pointer", "float"))
    if len(allocation["save_costs"]) <= total:
        fail("register_allocation.save_costs", allocation.lines["save_costs"],
             "must cover every possible total of saved registers, including zero")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("description", type=Path)
    parser.add_argument("em_mnem", type=Path, help="generated em_mnem.h")
    args = parser.parse_args()
    try:
        check(args.description, args.em_mnem)
    except (SchemaError, libconf.ConfigParseError, OSError, ValueError) as error:
        print(f"{args.description}:{error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
