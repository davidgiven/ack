#ifndef RUN_H
#define RUN_H

extern int send_cont(int stop_message);
extern int set_or_clear_breakpoint(t_addr a, int type);
extern int set_or_clear_trace(t_addr start, t_addr end, int type);
extern int set_pc(t_addr PC);
extern int singlestep(int type, long count);
extern t_addr* get_EM_regs(int level);
extern void list_position(p_position pos);
extern t_addr get_dump(char** globbuf, char** stackbuf);
extern int put_dump(char* globbuf, char* stackbuf);
extern void set_bytes(long size, char* from, t_addr to);
extern void signal_child(int signum);

#endif