from build.ab import simplerule, Rule, Target, Targets, emit

emit(
    """
BISON ?= bison
FLEX ?= flex
"""
)


@Rule
def bison(self, name, src: Target, deps: Targets = [], stem=None):
    if not stem:
        stem = self.localname
    simplerule(
        replaces=self,
        ins=[src],
        outs=[f"={stem}.c", f"={stem}.h"],
        deps=deps,
        commands=[
            "$(BISON) --defines=$[outs[1]] --output=$[outs[0]] $[ins[0]]"
        ],
        label="BISON",
    )


@Rule
def flex(self, name, src: Target, deps: Targets = []):
    simplerule(
        replaces=self,
        ins=[src],
        outs=[f"={self.localname}.yy.c"],
        commands=["$(FLEX) -s -t $[ins[0]] > $[outs[0]]"],
        label="FLEX",
    )
