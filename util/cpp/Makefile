distr:
	echo '#ifndef	lint' > Version.c
	echo 'char Version[] = "ACK C preprocessor Version XXX";' | \
		sed "s/XXX/`RC -i`/" >> Version.c
	echo '#endif	lint' >> Version.c
