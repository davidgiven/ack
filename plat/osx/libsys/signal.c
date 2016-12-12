#include <signal.h>

sig_t signal(int sig, sig_t func)
{
	struct sigaction newsa, oldsa;
	int i;

	newsa.sa_handler = func;
	newsa.sa_mask = 0; /* empty set */
	newsa.sa_flags = SA_RESTART;

	i = sigaction(sig, &newsa, &oldsa);
	if (i < 0)
		return SIG_ERR;
	return oldsa.sa_handler;
}
