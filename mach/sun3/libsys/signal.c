static long masks[32];
static long flags[32];
void (*
signal(sig,handler))()
	void (*handler)();
{
	struct {
		void (*sv_handler)();
		long	sv_mask;
		long	sv_flags;
	} v, ov;

	v.sv_handler = handler;
	v.sv_mask = masks[sig];
	v.sv_flags = flags[sig];
	if (_sigvec(sig,&v, &ov) < 0) return (void (*)()) -1;
	if (v.sv_mask != ov.sv_mask || v.sv_flags != ov.sv_flags) {
		v.sv_mask = ov.sv_mask;
		masks[sig] = ov.sv_mask;
		v.sv_flags = ov.sv_flags;
		flags[sig] = ov.sv_flags;
		if (_sigvec(sig,&v,(char *) 0) < 0) return (void (*)()) -1;
	}
	return ov.sv_handler;
}
