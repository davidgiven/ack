from build import PLATS
from build.ab import Rule, simplerule
from build.ack import ackclibrary


@Rule
def generate(self, name, body, offset):
    simplerule(
        replaces=self,
        ins=["./generate.sh", f"./{body}"],
        outs=[f"={name}.s"],
        commands=[f"$[ins[0]] {body} {offset} > $[outs]"],
    )


generated = [
    generate(name=f"{k}{i}", body=f"{k}.h", offset=i)
    for i in range(1, 129)
    for k in [
        "fload",
        "floadn",
        "fstore",
        "fstoren",
        "faddr",
        "faddrn",
    ]
]

for plat in PLATS:
    ackclibrary(
        name=f"lib_{plat}",
        srcs=(
            generated
            + [
                "./aar2.s",
                "./adi4.s",
                "./and.s",
                "./blm.s",
                "./cii.s",
                "./cmi4.s",
                "./cmps_mag.s",
                "./cmpu_mag.s",
                "./cms.s",
                "./com.s",
                "./csa.s",
                "./csb.s",
                "./dup.s",
                "./dvi2.s",
                "./dvi4.s",
                "./exg.s",
                "./flp.s",
                "./inn.s",
                "./ior.s",
                "./lar2.s",
                "./loi.s",
                "./mli2.s",
                "./mli4.s",
                "./mlu2.s",
                "./ngi4.s",
                "./nop.s",
                "./pchl.s",
                "./pro.s",
                "./rck.s",
                "./ret.s",
                "./rol4.s",
                "./ror4.s",
                "./rst.s",
                "./sar2.s",
                "./sbi4.s",
                "./set2.s",
                "./set.s",
                "./sli2.s",
                "./sli4.s",
                "./sri2.s",
                "./sri4.s",
                "./sti.s",
                "./xor.s",
                "./fload.h",
                "./floadn.h",
                "./fstore.h",
                "./fstoren.h",
                "./faddr.h",
                "./faddrn.h",
            ]
        ),
        plat=plat,
    )
