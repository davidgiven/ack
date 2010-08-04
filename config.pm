-- ======================================================================= --
--                          ACK CONFIGURATION                              --
--                      (Edit this before building)                        --
-- ======================================================================= --

-- What platform to build for by default?

DEFAULT_PLATFORM = "pc86"

-- Where should the ACK put its temporary files?

ACK_TEMP_DIR = "/tmp"

-- Where is the ACK going to be installed, eventually?

PREFIX = "/usr/local"

-- ======================================================================= --
--                       BROKEN ACK CONFIGURATION                          --
--                       (Currently not editable)                          --
-- ======================================================================= --

-- FIXME: the following two variables must be set to their Minix variants
-- due to hard-coded references in the descr files.

-- Name of the platform-independent library directory; 'share' on modern
-- systems, 'lib' on Minix-like systems.

PLATIND = "lib"

-- Name of the platform-dependent library directory; 'lib' on modern
-- systems, 'lib.bin' on Minix-like systems.

PLATDEP = "lib.bin"

-- ======================================================================= --
--                      BUILD SYSTEM CONFIGURATION                         --
--                        (Not user servicable)                            --
-- ======================================================================= --

-- Absolute path to the ACK source directory.

ROOTDIR = posix.getcwd().."/"

-- Temporary directory used during the build process.

TEMPDIR = "/tmp/ack-temp/"

-- Directory in which dynamically generated header files will go during
-- the build process.

HEADERDIR = TEMPDIR.."headers/"

-- Directory in which tools used by the build process but which not actually
-- deployed with the ACK will go.

TOOLDIR = TEMPDIR.."tools/"

-- Directory in which the libraries used to build the ACK tools but which are
-- not actually deployed with the ACK will go.

LIBDIR = TEMPDIR.."lib/"

-- Staging area where the installation will be built before actually copying
-- it.

BINDIR = TEMPDIR.."staging/"

-- Directory that the pm cache goes in.

pm.intermediate_cache_dir = TEMPDIR.."pmcache/"
