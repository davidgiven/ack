BEGIN {
	print "char *pop_push[]="
	print "{"
	print "\"\","
	switch = 0
}
/aar/	{
	switch = 1
}
	{
	if (switch) printf("\"%s\",\n",$3)
}
END {
	print "};"
}
