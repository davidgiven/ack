/* $Header$ */

#define BIS(sp,n) (sp)[(n)>>4] |= 1<<((n)&0xF)
#define BIC(sp,n) (sp)[(n)>>4] &= ~(1<<((n)&0xF))
#define BIT(sp,n) (((sp)[(n)>>4]&(1<<((n)&0xF)))!=0)
