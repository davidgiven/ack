#ifndef RUN_H
#define RUN_H

extern void signal_child(int signum);
extern void list_position( p_position pos);
extern int send_cont(int stop_message);
extern t_addr* get_EM_regs(int level);
extern int singlestep(int type, long count);

#endif