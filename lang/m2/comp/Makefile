distr:
	echo '#ifndef	lint' > Version.c
	echo 'char Version[] = "ACK Modula-2 compiler Version XXX";' | \
		sed "s/XXX/`RC -i`/" >> Version.c
	echo '#endif' >> Version.c
