yacc = simple {
	class = "yacc",
	outputs = {"%U%/%I%.c"},
	
	command = {
		"yacc -t -b %out[1]:dirname%/y -d %in%",
		"mv %out[1]:dirname%/y.tab.c %out[1]%"
	}
}

flex = simple {
	class = "flex",
	outputs = {"%U%/%I%.c"},
	
	command = {
		"flex -s -t %in% > %out%"
	}
}
