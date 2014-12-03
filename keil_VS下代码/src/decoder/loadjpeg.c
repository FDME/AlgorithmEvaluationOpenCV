#include "tinyjpeg.h"
#include "../header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
//#include <def.h>
#include "tinyjpeg-internal.h"
struct jdec_private decoder;
int jpg_decode(UINT8T* iPicture,UINT8T* oY,UINT8T* oU,UINT8T* oV,UINT32T size)
{
	int output_format = TINYJPEG_FMT_YUV420P;
	
  unsigned int length_of_pic;
  unsigned int width, height;
  unsigned char *buf;
  struct jdec_private *jdec;
  unsigned char *components[3];

  /*Load the Jpeg into memory */
  length_of_pic = size;
	buf = iPicture;
	
  /* Decompress it */
  jdec = &decoder;
	//uart_printf("Parse headers...\n");
  if (tinyjpeg_parse_header(jdec, buf, length_of_pic)<0)
    //exitmessage(tinyjpeg_get_errorstring(jdec));
		return 1;
  /* Get the size of the image */
  tinyjpeg_get_size(jdec, &width, &height);
	//uart_printf("filesize: %x\nwidth: %x\nheight: %x\n\n",length_of_pic, width,height);
  if (tinyjpeg_decode(jdec, output_format, oY, oU,oV) < 0)
    //exitmessage(tinyjpeg_get_errorstring(jdec));
		return 1;
	//uart_printf("Get result...\n");
  /* 
   * Get address for each plane (not only max 3 planes is supported), and
   * depending of the output mode, only some components will be filled 
   * RGB: 1 plane, YUV420P: 3 planes, GREY: 1 plane
   */
  tinyjpeg_get_components(jdec, components);

  /* Only called this if the buffers were allocated by tinyjpeg_decode() */
#ifdef WIN32
#else
  tinyjpeg_free(jdec);//VSÓÐÄÚ´æ´íÎó
#endif
  /* else called just free(jdec); */
  return 0;
}


