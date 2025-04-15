from build.ab import Rule, Targets, simplerule
from build.toolchain import Toolchain, HostToolchain

Toolchain.PC = ["$(FPC) -FE$[dir] -o$[outs[0]] $[ins] $(FPCFLAGS) $[fpcflags]"]
HostToolchain.PC = ["$(HOSTFPC) -FE$[dir] -o$[outs[0]] $[ins] $(HOSTFPCFLAGS) $[fpcflags]"]

@Rule
def _pascalprogramimpl(self, name, srcs:Targets=[], fpcflags=[], toolchain=None, label="PASCAL"):
    simplerule(replaces=self,
               ins=srcs,
               outs=[f"={self.localname}"],
               commands=toolchain.PC,
               label=toolchain.PREFIX + label)

def pascalprogram(toolchain=Toolchain, **kwargs):
    return _pascalprogramimpl(**kwargs, toolchain=toolchain)

def hostpascalprogram(toolchain=HostToolchain, **kwargs):
    return _pascalprogramimpl(**kwargs, toolchain=toolchain)
