from build.ab import Rule, simplerule, export


@Rule
def build_plat_libs(self, name, arch, plat, is_em=False):
    export(
        replaces=self,
        items=(
            {f"$(PLATIND)/{plat}/libend.a": f"mach/{arch}/libend+lib_{plat}"}
            | (
                {}
                if is_em
                else {
                    f"$(PLATIND)/{plat}/libem.a": f"mach/{arch}/libem+lib_{plat}"
                }
            )
        ),
        deps=[
            f"lang/cem/libcc.ansi+all_{plat}",
            f"lang/basic/lib+all_{plat}",
            f"lang/m2/libm2+all_{plat}",
            f"lang/pc/libpc+all_{plat}",
        ],
    )
