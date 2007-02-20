-- $Source$
-- $State$

-- Custom rules used by the ACK build process.

preprocess = simple {
	class = "preprocess",
	outputs = {"%U%-%I%"},
	command = {
		"cpp -I%HEADERDIR% %in% > %out[1]%"
	}
}

-- Revision history
-- $Log$
-- Revision 1.2  2007-02-20 00:32:58  dtrg
-- Changed the 'preprocess' rule to use the system C preprocessor.
--
-- Revision 1.1  2006/07/22 00:49:48  dtrg
-- First version in CVS.
--
