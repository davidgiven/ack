"""Exercise every machine-description reader with preprocessed descriptions.

Usage: python3 test_readers.py /path/to/installed/ego /path/to/preprocessed/*.descr
"""

import pathlib
import subprocess
import sys
import tempfile

reader_directory = pathlib.Path(sys.argv[1])
descriptions = [pathlib.Path(name).resolve() for name in sys.argv[2:]]
if not descriptions:
    sys.exit("expected at least one preprocessed description")

with tempfile.TemporaryDirectory() as directory:
    directory = pathlib.Path(directory)
    inputs = [directory / name for name in ("p", "d", "l", "b")]
    # Empty procedure table (length, all-fields flag) and data table (length).
    inputs[0].write_bytes(b"\0" * 4)
    inputs[1].write_bytes(b"\0" * 2)
    inputs[2].write_bytes(b"")
    inputs[3].write_bytes(b"")
    outputs = [directory / name for name in ("pout", "dout", "lout", "bout")]
    count = 0
    for description in descriptions:
        for reader in ("ra", "ud", "sr", "cs", "sp"):
            for mode in ("-T", "-S"):
                # Test both flag orders: machine initialization must see the final mode.
                for flags in ([mode, "-M", str(description)], ["-M", str(description), mode]):
                    subprocess.run(
                        [str(reader_directory / reader), *map(str, inputs),
                         *map(str, outputs), *flags],
                        check=True, capture_output=True, timeout=10,
                    )
                    count += 1
print(f"{count} reader initialization checks passed")
