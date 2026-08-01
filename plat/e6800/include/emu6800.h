/*
 *	emu6800.h
 */

#include <stdint.h>

#define	emu6800_ramtop	((uint8_t *)0xefff)

/*
 * boot.s minitourine
 */
/* extern	void	putchar(int ch); */
/* extern	int	getchar(void); */
extern	void	print(int x);
extern	void	cpu_counter(void);
extern	void	puthexc(int ch);
extern	void	puthexi(int x);
extern	void	putstr(char *s);

uint8_t	*emu6800_ram;

extern	int emu6800_conin(void);
extern	void emu6800_conout(int chx);

