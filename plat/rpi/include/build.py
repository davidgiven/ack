from build.c import clibrary
from glob import glob

headers = glob("**/*.h", root_dir="plat/rpi/include", recursive=True)

clibrary(name="include", hdrs={k: f"./{k}" for k in headers})
