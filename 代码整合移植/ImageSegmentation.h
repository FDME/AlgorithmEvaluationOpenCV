#include "header.h"

#define MAXHEIGHT	640
#define MAXWIDTH	480
#define MAXLABEL	3
#define MAXPIXEL	(MAXHEIGHT*MAXWIDTH)
#define MAXNODE		(MAXPIXEL + 10)
#define INF			1000000

struct Edge{
	INT32T x, y;
	INT32T cap;
	INT32T next;
};
typedef struct Edge EDGE;

void ImageSegment(UINT8T *label);
