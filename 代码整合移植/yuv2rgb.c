#include "yuv2rgb.h"

//解码后的YUV420转为RGB
int yuv2rgb(UINT8T* py, UINT8T* pu, UINT8T* pv, UINT8T*r, UINT8T*g, UINT8T*b)
{
		int baseSize = C * R;
    int rgbSize = baseSize * 3;

    UINT8T rgbData[R*C*3];
	
    int temp = 0;

    UINT8T* rData = rgbData;                  //r分量地址
    UINT8T* gData = rgbData + baseSize;       //g分量地址
    UINT8T* bData = gData   + baseSize;       //b分量地址

    int uvIndex =0, yIndex =0;
		int x;
		int y;
	
	  if (!py || !pu || !pv || !r||!g||!b)
    {
        return -1;
    }
    
    memset(rgbData, 0, rgbSize);

    //YUV->RGB 的转换矩阵
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

            /* r分量 */
            temp          = (int)(py[yIndex] + (pv[uvIndex] - 128) * 1.4022);
            rData[yIndex] = temp<0 ? 0 : (temp > 255 ? 255 : temp);

            /* g分量 */
            temp          = (int)(py[yIndex] + (pu[uvIndex] - 128) * (-0.3456) +
                (pv[uvIndex] - 128) * (-0.7145));
            gData[yIndex] = temp < 0 ? 0 : (temp > 255 ? 255 : temp);

            /* b分量 */
            temp          = (int)(py[yIndex] + (pu[uvIndex] - 128) * 1.771);
            bData[yIndex] = temp < 0 ? 0 : (temp > 255 ? 255 : temp);
        }
    }

    //将R,G,B分别赋给image_RGB
    
    for (y = 0; y < R; y++)
    {
        for (x = 0; x < C; x++)
        {
			r[y * C + x ] = rData[y * C + x];   //R
            g[y * C + x ] = gData[y * C + x];   //G
            b[y * C + x ] = bData[y * C + x];   //B
        }
    }

    if (!r||!g||!b)
    {
        return -1;
    }
		return 0;
}
