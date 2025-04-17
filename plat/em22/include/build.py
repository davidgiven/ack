from build.ab import export
from build.ack import clibrary

clibrary(name="include", deps=["plat/em/include"])

export(name="all", deps=["plat/em/include+all"])
