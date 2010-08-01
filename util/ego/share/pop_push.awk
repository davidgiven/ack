BEGIN {
	print "char *pop_push[]="
	print "{"
	print "\"\","
	s = 0
}
/aar/	{
	s = 1
}
	{
	if (s) printf("\"%s\",\n",$3)
}
END {
	print "};"
}
