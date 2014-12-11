//实现在watershd.c
#include "header.h"
#define WSHED 3
#define IN_QUEUE 4
#define NQ 256
#define AAA 30000 //同一梯度的最大点数
#define FGND 1
#define BGND 2
    // MAX(a,b) = b + MAX(a-b,0)
    //#define ws_max(a,b) ((b) + subs_tab[(a)-(b)+NQ])
#define ws_max(a,b) MAX(a,b)
    // MIN(a,b) = a - MAX(a-b,0)
    //#define ws_min(a,b) ((a) - subs_tab[(a)-(b)+NQ])
#define ws_min(a,b) MIN(a,b)
#define ws_push(idx,pos)	\
{								\
	if(q[idx].next > 255)		\
	q[en].pos = pos;			\
	else \
	q[idx].pos = pos; \
	q[q[idx].en].next = en;	\
	q[idx].en = en++;		\
}
#define ws_pop(idx,pos)		\
{							\
	pos = q[idx].pos;		\
	qt = q[idx].next;	\
	q[idx] = q[q[idx].next];	\
	if(q[idx].next==0) q[idx].next = idx;\
	q[qt].next = 0; \
}
#define c_diff(ptr1,ptr2,diff)	\
{								\
	dr = ptr1.r>ptr2.r?(ptr1.r-ptr2.r):(ptr2.r-ptr1.r);\
	dg = ptr1.g>ptr2.g?(ptr1.g-ptr2.g):(ptr2.g-ptr1.g);\
	db = ptr1.b>ptr2.b?(ptr1.b-ptr2.b):(ptr2.b-ptr1.b);\
	diff = MAX(MAX(dr,dg),db);						   \
}
struct ws_Queue{
	UINT32T pos;
	UINT32T next;
	UINT32T en;
	UINT8T flag;
};
typedef struct ws_Queue WSQ;
WSQ q[SIZE];//储存mask和图像坐标的队列
void ForegroundSeperation(RGBTYPE* img);