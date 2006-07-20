yacc = simple {
	class = "yacc",
	outputs = {"%U%/%I%.c"},
	
	command = {
		"yacc -t -b %{return posix.dirname(self.out[1])}%/y -d %in%",
		"mv %{return posix.dirname(self.out[1])}%/y.tab.c %out%"
	}
}

flex = simple {
	class = "flex",
	outputs = {"%U%/%I%.c"},
	
	command = {
		"flex -s -t %in% > %out%"
	}
}
