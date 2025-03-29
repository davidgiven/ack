from build.c import clibrary
from glob import glob

headers = glob("**/*.h", root_dir="lang/cem/libcc.ansi/headers", recursive=True)

clibrary(name="headers", hdrs={k: f"./{k}" for k in headers})
