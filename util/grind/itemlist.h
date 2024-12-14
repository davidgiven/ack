#ifndef ITEMLIST_H
#define ITEMLIST_H

extern void handle_displays(void);
extern void remove_from_item_list(int n);
extern void perform_items(void);
extern int item_addr_actions(t_addr a, int mess_type, int may_stop);

#endif