-- $Source$
-- $State$

-- This is unpleasant. LLgen can generate an arbitrary number of output files,
-- which means we need our own output filename generator.

LLgen = simple {
	class = "LLgen",
	command = {
		"rm -f %out%",
		"cd %out[1]:dirname% && %TOOLDIR%LLgen %in%"
	},
	
	outputs = {"%U%/" },
	__outputs = function(self, inputs)
		local o = simple.__outputs(self, inputs)[1]
		
		local outputs = {o.."Lpars.h", o.."Lpars.c"}
		
		for _, i in ipairs(inputs) do
			i = string.gsub(i, "^.*/", "")
			i = string.gsub(i, "%.g$", ".c")
			table.insert(outputs, o..i)
		end
		
		return outputs
	end
}

-- Revision history
-- $Log$
-- Revision 1.2  2006-11-11 22:59:01  dtrg
-- Now uses the version of LLgen included with the ACK instead of the standalone version.
--
-- Revision 1.1  2006/07/20 23:18:18  dtrg
-- First version in CVS.
--