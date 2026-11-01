"""Run with schemachecker.py, a preprocessed i386 description, and em_mnem.h."""
import pathlib
import subprocess
import sys
import tempfile

checker, description, header = sys.argv[1:]
source = pathlib.Path(description).read_text()
cases = [
    ("valid", source, True),
    ("missing field", source.replace("word_size = 4;", ""), False),
    ("unknown field", source + "\nunknown = 1;\n", False),
    ("duplicate field", source + "\nword_size = 4;\n", False),
    ("wrong type", source.replace("word_size = 4;", 'word_size = "4";'), False),
    ("pointer size", source.replace("pointer_size = 4;", "pointer_size = 3;"), False),
    ("negative registers", source.replace("general = 2;", "general = -1;"), False),
    ("short overflow", source.replace("time = 8;", "time = 32768;"), False),
    ("register class", source.replace('from = "pointer"', 'from = "bogus"'), False),
    ("condition", source.replace('when = "default"', 'when = "bogus"', 1), False),
    ("missing default", source.replace('when = "default"', 'when = "fitbyte"', 1), False),
    ("empty rules", source.replace(
        '{ when = "default"; time = 8; space = 4; }', ''), False),
    ("duplicate condition", source.replace(
        '{ when = "default"; time = 8; space = 4; }',
        '{ when = "fitbyte"; time = 1; space = 1; }, '
        '{ when = "fitbyte"; time = 2; space = 2; }, '
        '{ when = "default"; time = 8; space = 4; }'), False),
    ("early default", source.replace(
        '{ when = "default"; time = 8; space = 4; }',
        '{ when = "default"; time = 8; space = 4; }, '
        '{ when = "fitbyte"; time = 1; space = 1; }'), False),
    ("duplicate case", source.replace('from = "pointer"', 'from = "general"', 1), False),
    ("insufficient saves", source.replace("general = 2;", "general = 4;"), False),
    ("bad instruction", source.replace("addressing_modes = [", "addressing_modes = [ -1,", 1), False),
    ("noninteger instruction", source.replace("addressing_modes = [", 'addressing_modes = [ "adp",', 1), False),
    ("shift range", source.replace("excluded_index_shift_counts = [];", "excluded_index_shift_counts = [32];"), False),
    ("UD condition", source.replace(
        'global_access = ( { when = "default";',
        'global_access = ( { when = "in_0_8";'), False),
    ("syntax", source + "\n{", False),
    ("zero registers", source.replace("general = 2;", "general = 0;"), True),
    ("boolean type", source.replace("overflow_harmful = false;", "overflow_harmful = 0;"), False),
    ("list element type", source.replace(
        '{ when = "default"; time = 8; space = 4; }', '3'), False),
]
with tempfile.TemporaryDirectory() as directory:
    path = pathlib.Path(directory) / "test.descr"
    for name, text, valid in cases:
        assert name == "valid" or text != source, name
        path.write_text(text)
        result = subprocess.run(
            [sys.executable, checker, str(path), header], capture_output=True, text=True
        )
        assert (result.returncode == 0) == valid, (name, result.stderr)
        if not valid:
            assert str(path) in result.stderr, (name, result.stderr)
print(f"{len(cases)} schema checks passed")
