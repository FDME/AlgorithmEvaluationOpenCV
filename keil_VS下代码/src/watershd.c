#include "ForegroundSeperation.h"
void ForegroundSeperation(RGBTYPE* img){
	UINT8T markers[SIZE], *m;
	UINT32T** pt;
	UINT32T i,j,pos,en = 256, qt;
	UINT8T active_queue;
	UINT8T t,dr,db,dg,lab;
	UINT16T idx;

	for(i = 0; i != SIZE; i++)//初始化队列
	{if(i < 256) q[i].next = i; else q[i].next = 0; q[i].flag = 0;}
	//for( i = 0; i < 256; i++ )//初始化subs_tabs，以计算梯度
 //       subs_tab[i] = 0;
 //   for( i = 256; i <= 512; i++ )
 //       subs_tab[i] = i - 256;
	//这里改marker
	for( i = 0; i != R; i++)
		for( j = 0; j != C; j++)
		{
			if(i == 0 || j == 0 || i == R-1 || j == C-1)
				markers[i*C+j] = WSHED;
			else if( i > R/2 && j < C/3 )
				markers[i*C+j] = FGND;
			else if( i < R/8 || j > C*3/4 )
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
					c_diff(img[pos],img[pos-1],idx);
				if( m[1] < 3 && m[1] > 0 )
				{
					c_diff(img[pos],img[pos+1],t);
					idx = ws_min(idx,t);
				}
				if( m[-C] < 3 && m[-C] > 0 )
				{
					c_diff(img[pos],img[pos-C],t);
					idx = ws_min(idx,t);
				}
				if( m[C] < 3 && m[C] > 0 )
				{
					c_diff(img[pos],img[pos+C],t);
					idx = ws_min(idx,t);
				}
				if(idx == 33)
					idx = 33;
				ws_push(idx, pos);

				m[0] = IN_QUEUE;
			}
		}
	}
// find the first non-empty queue
 	for( i = 0; i < NQ; i++)
		if(q[i].next < 256)
			break;
	if( i == NQ )
		return;

	active_queue = i;
	m = markers;
	j = 0; 
	while(1)
	{
		j++;
		lab = 0;
		if(q[active_queue].next < NQ)
		{
			for( i = active_queue+1; i<NQ; i++)
				if( q[i].next > NQ)
					break;
			if( i == NQ)
				break;
			active_queue = i;
		}
		if(active_queue == 33)
			active_queue = 33;
		ws_pop(active_queue,pos);

		m = markers + pos;
		if(pos >= 307200)
			pos = pos;
		if(pos == 0)
			pos = pos;
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
			c_diff( img[pos], img[pos-1],t);
			pos--;
			ws_push(t,pos); 
			pos++;
			active_queue = ws_min(active_queue,t);
			m[-1] = IN_QUEUE;
		}
		if( m[1] == 0 )
		{
			c_diff( img[pos], img[pos+1],t);
			pos++;
			ws_push(t,pos);
			pos--;
			active_queue = ws_min(active_queue,t);
			m[1] = IN_QUEUE;
		}
		if( m[-C] == 0 )
		{
			c_diff( img[pos], img[pos-C],t);
			pos-=C;
			ws_push(t,pos);
			pos+=C;
			active_queue = ws_min(active_queue,t);
			m[-C] = IN_QUEUE;
		}
		if( m[C] == 0 )
		{
			c_diff( img[pos], img[pos+C],t);
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
				img[pos].r = 0;
				img[pos].g = 0;
				img[pos].b = 0;
			}
		}
}