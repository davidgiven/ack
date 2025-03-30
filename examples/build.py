from build import PLATS
from build.ab import export
from build.ack import ackcprogram

exports = {}
for prog in ["hilo.c", "mandelbrot.c", "paranoia.c", "startrek.c"]:
    name = prog.replace(".", "_")
    exports |= {f"$(PLATIND)/examples/{prog}": f"./{prog}"}
    for plat in PLATS:
        ackcprogram(
            name=f"{name}_{plat}",
            plat=plat,
            lang="c",
            srcs=[f"./{prog}"],
            cflags=["-O2"],
        )
        exports[f"$(PLATIND)/examples/{name}_{plat}.exe"] = f".+{name}_{plat}"

export(name="all", items=exports)
