from build.ab import export
from build.c import cprogram

i = {}


def simpleprogram(name):
    p = cprogram(
        name=name,
        srcs=[f"./{name}.c"],
        deps=["h", "modules/src/object", "modules/src/system"],
    )

    i[f"$(INSDIR)/bin/{name}$(EXT)"] = p
    i[f"$(INSDIR)/share/man/man1/{name}.1"] = f"./{name}.1"


simpleprogram("aelflod")
simpleprogram("anm")
simpleprogram("ashow")
simpleprogram("asize")
simpleprogram("aslod")
simpleprogram("astrip")

export(name="all", items=i)
