static char m[] = "Warning: FOR-loop control variable was changed in the body";
_forloopchk(a,b)
	unsigned a, b;
{
	if (a != b) {
		Traps_Message(m , 0, sizeof(m) - 2, 1);
	}
}
