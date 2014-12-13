#include "ForegroundSeperation.h"
#define WSHED 3
#define IN_QUEUE 4
#define NQ 256
#define AAA 30000 //同一梯度的最大点数
#define FGND 1
#define BGND 2
    // MAX(a,b) = b + MAX(a-b,0)
    //#define ws_max(a,b) ((b) + subs_tab[(a)-(b)+NQ])
#define ws_max(a,b) max(a,b)
    // MIN(a,b) = a - MAX(a-b,0)
    //#define ws_min(a,b) ((a) - subs_tab[(a)-(b)+NQ])
#define ws_min(a,b) min(a,b)
#define ws_push(idx,pos)	\
{								\
	if(q[idx].flag){q[en].pos = pos;q[q[idx].en].next = en;q[en].flag = 1;q[idx].en = en++;}			\
	else {q[idx].pos = pos; q[idx].en = idx;q[idx].flag = 1;}\
}
#define ws_pop(idx,pos)		\
{							\
	pos = q[idx].pos;		\
	if(!q[q[idx].next].flag) q[idx].flag = 0; \
	else {if(q[q[idx].next].next == q[idx].next) q[idx].en = idx; q[q[idx].next].flag = 0; q[idx].pos = q[q[idx].next].pos; q[idx].next = q[q[idx].next].next;}	\
}
#define c_diff(ptr1,ptr2,diff)	\
{								\
	dr = ptr1.r>ptr2.r?(ptr1.r-ptr2.r):(ptr2.r-ptr1.r);\
	dg = ptr1.g>ptr2.g?(ptr1.g-ptr2.g):(ptr2.g-ptr1.g);\
	db = ptr1.b>ptr2.b?(ptr1.b-ptr2.b):(ptr2.b-ptr1.b);\
	diff = max(max(dr,dg),db);						   \
}
struct ws_Queue{
	UINT32T pos;
	UINT32T next;
	UINT32T en;
	UINT8T flag;
};
typedef struct ws_Queue WSQ;

void ForegroundSeperation(RGBTYPE* src,RGBTYPE* dst){
	UINT8T markers[SIZE], *m;
	UINT32T** pt;
	UINT32T i,j,pos,en = 256, qt;
	UINT8T active_queue;
	UINT8T t,dr,db,dg,lab;
	UINT16T idx;
    WSQ q[SIZE];//储存mask和图像坐标的队列
	for(i = 0; i != SIZE; i++)//初始化队列
	{q[i].next = i; q[i].flag = 0;q[i].en = 0; q[i].pos = 0;}
	//这里改marker
	for( i = 0; i != R; i++)
		for( j = 0; j != C; j++)
		{
			if(i == 0 || j == 0 || i == R-1 || j == C-1)
				markers[i*C+j] = WSHED;
			else if( i > R/2 && j < C/3 )
				markers[i*C+j] = FGND;
			else if( i < R/10 || j > C*3/4 )
				markers[i*C+j] = BGND;
			else
				markers[i*C+j] = 0;
		}
	//draw a pixel-wide border of dummy "watershed" (i.e. boundary) pixels
	//for( j = 0; j < C; j++)
		//markers[j] = markers[j+(R-1)*C] = WSHED;
    // initial phase: put all the neighbor pixels of each marker to the ordered queue -
    // determine the initial boundaries of the basins
	for( i = 1; i < R-1; i++)
	{
		//markers[i*C] = markers[i*C+C-1] = WSHED;
		for( j = 1; j < C-1; j++)
		{
			m = markers + i*C+j;
			if( m[0]  > 2 ) m[0] = 0;
			if( m[0] == 0 && ( (m[-1] < 3 && m[-1] > 0) || (m[1] < 3 && m[1] > 0) || (m[-C] < 3 && m[-C] > 0) || (m[C] < 3 && m[C] > 0) ))
			{
				pos = i*C + j;
				idx = 256;
				if( m[-1] < 3 && m[-1] > 0 )
					c_diff(src[pos],src[pos-1],idx);
				if( m[1] < 3 && m[1] > 0 )
				{
					c_diff(src[pos],src[pos+1],t);
					idx = ws_min(idx,t);
				}
				if( m[-C] < 3 && m[-C] > 0 )
				{
					c_diff(src[pos],src[pos-C],t);
					idx = ws_min(idx,t);
				}
				if( m[C] < 3 && m[C] > 0 )
				{
					c_diff(src[pos],src[pos+C],t);
					idx = ws_min(idx,t);
				}
				ws_push(idx, pos);

				m[0] = IN_QUEUE;
			}
		}
	}
// find the first non-empty queue
  	for( i = 0; i < NQ; i++)
		if(q[i].flag)
			break;
	if( i == NQ )
		return;

	active_queue = i;
	m = markers;
	while(1)
	{
		lab = 0;
		if(!q[active_queue].flag)
		{
			for( i = active_queue+1; i<NQ; i++)
				if( q[i].flag)
					break;
			if( i == NQ)
				break;
			active_queue = i;
		}
		ws_pop(active_queue,pos);

		m = markers + pos;
		t = m[-1];
		if( t < 3 && t > 0 ) lab = t;
		t = m[1];
        if( t < 3 && t > 0 )
        {
            if( lab == 0 ) lab = t;
            else if( t != lab ) lab = WSHED;
        }
        t = m[-C];
        if( t < 3 && t > 0 )
        {
            if( lab == 0 ) lab = t;
            else if( t != lab ) lab = WSHED;
        }
        t = m[C];
        if( t < 3 && t > 0 )
        {
            if( lab == 0 ) lab = t;
            else if( t != lab ) lab = WSHED;
        }
		if(lab == 0)
			lab = 0;
		m[0] = lab;
		if( !(lab == FGND || lab == BGND))
			continue;
		if( m[-1] == 0 )
		{
			c_diff( src[pos], src[pos-1],t);
			pos--;
			ws_push(t,pos); 
			pos++;
			active_queue = ws_min(active_queue,t);
			m[-1] = IN_QUEUE;
		}
		if( m[1] == 0 )
		{
			c_diff( src[pos], src[pos+1],t);
			pos++;
			ws_push(t,pos);
			pos--;
			active_queue = ws_min(active_queue,t);
			m[1] = IN_QUEUE;
		}
		if( m[-C] == 0 )
		{
			c_diff( src[pos], src[pos-C],t);
			pos-=C;
			ws_push(t,pos);
			pos+=C;
			active_queue = ws_min(active_queue,t);
			m[-C] = IN_QUEUE;
		}
		if( m[C] == 0 )
		{
			c_diff( src[pos], src[pos+C],t);
			pos+=C;
			ws_push(t,pos);
			pos-=C;
			active_queue = ws_min(active_queue,t);
			m[C] = IN_QUEUE;
		}
	}
	for( i = 0; i < R; i++)
		for( j = 0; j < C; j++)
		{
			pos = i*C+j;
			if(markers[pos] == BGND)
			{
				dst[pos].r = 0;
				dst[pos].g = 0;
				dst[pos].b = 0;
			}
			else dst[pos] = src[pos];
		}
}