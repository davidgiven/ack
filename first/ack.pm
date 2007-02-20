-- $Source$
-- $State$

-- Provides rules for building things with the half-built ACK itself.

ACKBUILDFLAGS = {"-m%PLATFORM%", "%OPTIMISATION%"}
ACKDEFINES = EMPTY
ACKINCLUDES = EMPTY

ackfile = simple_with_clike_dependencies {
	class = "ackfile",
	CINCLUDES = {REDIRECT, "ACKINCLUDES"},
	command = {
		"%BINDIR%bin/ack %ACKBUILDFLAGS% %ACKINCLUDES:cincludes% %ACKDEFINES:cdefines% -c -o %out% %in%"
	},
	outputs = {"%U%-%I%.o"},
}

ackprogram = simple {
	class = "ackprogram",
	command = {
		"%BINDIR%bin/ack %ACKBUILDFLAGS% -o %out% %in%"
	},
	outputs = {"%U%-%I%"},
}

acklibrary = simple {
	class = "acklibrary",
	command = {
		"%RM% %out% && %BINDIR%bin/aal cr %out% %in%"
	},
	outputs = {"%U%-%I%.a"},
}
