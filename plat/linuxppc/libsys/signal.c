#include <signal.h>

/*
 * Uses our bridge in sigaction.s when calling the signal handler.
 * Mimics Linux __NR_signal by using SA_NODEFER | SA_RESETHAND.
 */
sighandler_t signal(int signum, sighandler_t handler) {
	struct sigaction new, old;
	int i;

	new.sa_handler = handler;
	new.sa_mask = 0; /* empty set */
	new.sa_flags = SA_NODEFER | SA_RESETHAND;

	i = sigaction(signum, &new, &old);
	if (i < 0)
		return SIG_ERR;
	return old.sa_handler;
}
