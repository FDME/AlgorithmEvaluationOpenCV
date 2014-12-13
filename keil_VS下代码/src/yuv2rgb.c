#include "yuv2rgb.h"

//������YUV420תΪRGB
int yuv2rgb(UINT8T* py, UINT8T* pu, UINT8T* pv, RGBTYPE*image_RGB)
{
		int baseSize = C * R;
    int rgbSize = baseSize * 3;

    UINT8T rgbData[R*C*3];
	
    int temp = 0;

    UINT8T* rData = rgbData;                  //r������ַ
    UINT8T* gData = rgbData + baseSize;       //g������ַ
    UINT8T* bData = gData   + baseSize;       //b������ַ

    int uvIndex =0, yIndex =0;
		int x;
		int y;
	
	  if (!py || !pu || !pv || !image_RGB)
    {
        return FALSE;
    }
    
    memset(rgbData, 0, rgbSize);

    //YUV->RGB ��ת������
    //double  Yuv2Rgb[3][3] = 
		//	{1, 0, 1.4022,
    //    1, -0.3456, -0.7145,
    //    1, 1.771,   0};

    for(y=0; y < R; y++)
    {
        for(x=0; x < C; x++)
        {
            uvIndex        = (y>>1) * (C>>1) + (x>>1);
            yIndex         = y * C + x;

            /* r���� */
            temp          = (int)(py[yIndex] + (pv[uvIndex] - 128) * 1.4022);
            rData[yIndex] = temp<0 ? 0 : (temp > 255 ? 255 : temp);

            /* g���� */
            temp          = (int)(py[yIndex] + (pu[uvIndex] - 128) * (-0.3456) +
                (pv[uvIndex] - 128) * (-0.7145));
            gData[yIndex] = temp < 0 ? 0 : (temp > 255 ? 255 : temp);

            /* b���� */
            temp          = (int)(py[yIndex] + (pu[uvIndex] - 128) * 1.771);
            bData[yIndex] = temp < 0 ? 0 : (temp > 255 ? 255 : temp);
        }
    }

    //��R,G,B�ֱ𸳸�image_RGB
    
    for (y = 0; y < R; y++)
    {
        for (x = 0; x < C; x++)
        {
			image_RGB[y * C + x ].r = rData[y * C + x];   //R
            image_RGB[y * C + x ].g = gData[y * C + x];   //G
            image_RGB[y * C + x ].b = bData[y * C + x];   //B
        }
    }

    if (!image_RGB)
    {
        return FALSE;
    }
		return TRUE;
}
