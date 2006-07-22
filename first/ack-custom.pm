-- $Source$
-- $State$

-- Custom rules used by the ACK build process.

preprocess = simple {
	class = "preprocess",
	outputs = {"%U%-%I%"},
	command = {
		"%BINDIR%bin/cpp -I%HEADERDIR% %in% > %out[1]%"
	}
}

-- Revision history
-- $Log$
-- Revision 1.1  2006-07-22 00:49:48  dtrg
-- First version in CVS.
--
