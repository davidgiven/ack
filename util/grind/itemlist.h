#ifndef ITEMLIST_H
#define ITEMLIST_H

struct tree;
extern int item_addr_actions(t_addr a, int mess_type, int may_stop);
extern p_tree get_from_item_list(int n);
extern void able_item(int n, int kind);
extern void add_to_item_list(struct tree* p);
extern void handle_displays(void);
extern void perform_items(void);
extern void remove_from_item_list(int n);

#endif