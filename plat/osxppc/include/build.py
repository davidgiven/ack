from build.ab import export
from build.ack import clibrary

clibrary(name="include", deps=["plat/osx/include"])

export(name="all", deps=["plat/osx/include+all"])
