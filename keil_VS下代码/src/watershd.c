#include "ForegroundSeperation.h"
#include "test.h"
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
extern POINT up_st,up_en,down_st,down_en;
extern UINT16T right;
void ForegroundSeperation(RGBTYPE* src,RGBTYPE* dst){
	UINT8T* markers, *m;
	UINT32T** pt;
	UINT32T i,j,pos,en = 256, qt;
	UINT8T active_queue;
	UINT8T t,dr,db,dg,lab,cnt;
	UINT16T idx;
	UINT16T ha[C],he[R],right;
	
    //WSQ q[SIZE];//储存mask和图像坐标的队列
    WSQ* q;
	#ifdef WIN32
	UINT8T pRGB[SIZE * 3]; 
	CvPoint start_pt;
	CvPoint end_pt;
	IplImage *img;
#endif
    q = (WSQ*)malloc(sizeof(WSQ)*SIZE);
    markers = (UINT8T*)malloc(sizeof(UINT8T)*SIZE);
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
			else if( i < R/20 || j > C/2 || (j>30)&&(i>(-(j*0.4)+660)))
				//
				markers[i*C+j] = BGND;
			else
				markers[i*C+j] = 0;
		}
		//showImage_1ch(markers,"marker");
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
			else if(markers[pos] == WSHED)
			{
				dst[pos].r = 255;
				dst[pos].g = 0;
				dst[pos].b = 0;
			}
			else dst[pos] = src[pos];
		}
	for( i = 0; i < C; i++ ) ha[i] = 0;
	for( i = 0; i < R; i++ ) he[i] = 0;
	for( i = 1; i < C-1; i++)
		for( j = 1; j < R-1; j++ )
			if(markers[j*C+i] == WSHED)
			{ha[i]++;he[j]++;}
	//for( i = 0; i < C; i++) printf("%d   ",ha[i]);
	//printf("\n");
	//for( i = 0; i < R; i++) printf("%d   ",he[i]);

	//现在检测右边界！
	for( i = 1; i < C-1; i++)
	{
		if(ha[i]<10)
		{
			cnt = 0;
			continue;
		}
		else{
			if(++cnt > 3)
			{right = i-2;break;}
		}
	}

#ifdef WIN32
	start_pt = cvPoint(right,1);
	end_pt = cvPoint(right,R-1);
	memset(pRGB, 0, SIZE * 3); //初始化
	for (i = 0; i < R; i++)  //int不够，必须二重循环
	{
		for (j = 0; j < C; j++)
		{
			pRGB[i*C * 3 + j * 3] = dst[i*C + j].b;
			pRGB[i*C * 3 + j * 3 + 1] = dst[i*C + j].g;
			pRGB[i*C * 3 + j * 3 + 2] = dst[i*C + j].r;
		}
	}
	img = cvCreateImageHeader(cvSize(C,R),IPL_DEPTH_8U,3);
	cvSetData(img,pRGB,3*C);
	cvLine(img, start_pt, end_pt, CV_RGB(0, 0, 255), 1, CV_AA, 0);
#endif
	//现在检测上边界！！
	for( i = 0; he[i] == 0;i++);
	up_st.y = i;
	up_st.x = 1;
	cnt = 0;
	for( i = up_st.y; i < R-1; i++)
	{
		if(he[i] != 1)
		{cnt = 0; continue;}
		else {
			cnt++;
			if( cnt > 3 )
				break;
		}
	}
	up_en.y = i-2;
	for( i = 1; i < C-1; i++ )
		if( markers[up_en.y*C+i] == WSHED)
			break;
	up_en.x = i;
#ifdef WIN32
	start_pt = cvPoint(1,up_st.y);
	end_pt = cvPoint(i,up_en.y);
	cvLine(img, start_pt, end_pt, CV_RGB(0, 0, 255), 1, CV_AA, 0);
	cvNamedWindow("hahaha", 0);
	cvShowImage("hahaha", img);
	cvWaitKey(0);
#endif
	//不准的下边界
/*	for( i = up_en.y; i < R-1; i++){
		if(he[i] == 1)
		{cnt = 0; continue;}
		else{
			cnt++;
			if(cnt == 3)
				break;
		}
	}
	down_st.y = i - 3;
	for( i = 1; i < C-1; i++)
		if(markers[i+down_st.y*C] == WSHED) break;
	down_st.x = i;
	for( i = down_st.y; he[i]; i++);
	down_en.y = i-1;
	for( i = 1; i < C-1; i++)
		if(markers[i+down_en.y*C] == WSHED) break;
	down_en.x = i;
#ifdef WIN32
	start_pt = cvPoint(down_st.x,down_st.y);
	end_pt = cvPoint(down_en.x,down_en.y);
	printf("%d %d %d %d",down_st.x,down_st.y,down_en.x,down_en.y);
	cvLine(img, start_pt, end_pt, CV_RGB(0, 0, 255), 1, CV_AA, 0);
	cvNamedWindow("hahaha", 0);
	cvShowImage("hahaha", img);
	cvWaitKey(0);
#endif*/
	free(markers);
	free(q);
}
