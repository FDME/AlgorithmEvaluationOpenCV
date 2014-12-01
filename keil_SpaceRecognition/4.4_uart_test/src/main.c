/*********************************************************************************************
* File£∫	main.c
* Author:	embest
* Desc£∫	c main entry
* History:	
*********************************************************************************************/

/*------------------------------------------------------------------------------------------*/
/*                                     includes files	                                    */
/*------------------------------------------------------------------------------------------*/
#include "header.h"
UINT16T camera(void);
int jpg_decode(UINT8T* iPicture,UINT8T* oY,UINT8T* oU,UINT8T* oV,UINT16T size);
int yuv2rgb(UINT8T* py, UINT8T* pu, UINT8T* pv, RGBTYPE* image_RGB);
UINT8T buffer[65535]; //¥Ê¥¢≈ƒ…„jpeg
UINT8T py[Y_SIZE];   
UINT8T pu[Y_SIZE/4];
UINT8T pv[Y_SIZE/4];
RGBTYPE image_RGB[R*C];

//≤‚ ‘À˘”√Õº∆¨RGB–≈œ¢
extern UINT8T imageOringinal_R[R][C];
extern UINT8T imageOringinal_G[R][C];
extern UINT8T imageOringinal_B[R][C];
/*********************************************************************************************
* name:		main
*********************************************************************************************/
int main(int argc,char **argv)
{
	  UINT8T* picture = buffer;
		UINT32T k = 0;
		UINT16T size;

		sys_init();        /* Initial s3c2410's Clock, MMU, Interrupt,Port and UART */
		uart_select(UART0);
		uart_printf("Take a picture\n");
		size = camera();
		uart_select(UART0);
		uart_printf("Finish transmitting\n");
	/*	while(k<size){
			uart_sendbyte(buffer[k]);
			k++;
		}
		*/
		if(jpg_decode(picture,py,pu,pv,size)<0)
			uart_printf("Decoding error!\n");
		else
			uart_printf("Finish decoding\n"); 
		
		if(yuv2rgb(py, pu, pv, image_RGB) == FALSE)
			uart_printf("RGB error!\n");
		else
			uart_printf("Finish RGB\n"); 
	/*	//print the result of decoding
		k = 0;
		while(k<150000){
			uart_sendbyte(py[k]);
			k++;
		}
		while(k<Y_SIZE){
			uart_sendbyte(py[k]);
			k++;
		}
		*/
		while(1){}
}

