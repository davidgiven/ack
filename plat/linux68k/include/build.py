from build.ab import export
from build.ack import clibrary

clibrary(name="include", deps=["plat/linux/include"])

export(name="all", deps=["plat/linux/include+all"])
