#include "header.h"
/**
 * @file lens.h
 * Lens distortion parameteres
 * @author Marko Mäkelä (msmakela at nic.funet.fi)
 */

/* The following parameters are for ADS Pyro Webcam API-203.
 * For other cameras, first define LENS_SQUARE and LENS_QUAD to 0,
 * and make a picture of graphing paper or some other suitable object.
 * Then find the parameters with the Gimp "wideangle" plugin, and
 * fill them in here.
 */

/** @name These parameters correspond to the parameters of the
 * Gimp "wideangle" plugin by David Hodson.
 */
/**@{*///参数需改
/** r^2 correction factor (Main / 200).  The neutral value is 0. */
#define LENS_SQUARE	((double) 0 / 200.0)
#define LENS_QUAD	((double) 0 / 200.0)
//#define LENS_SQUARE	((double) -27.0 / 200.0)
/** r^4 correction factor (Edge / 200).  The neutral value is 0. */
//#define LENS_QUAD	((double) 8.0 / 200.0)
/**@}*/

/** @name Optional parameters.  For best performance, these should
 * be left undefined when equal to the neutral value. */
/**@{*/
/** Image scaling factor (0.5^(Zoom / 100)).
 * The neutral values are Zoom=0, or LENS_SCALE=1. */
#define LENS_SCALE	1.045
/** Image brightening factor (Brighten / 10).  The neutral value is 0.*/
/* #define LENS_BRIGHT	((double) 0.0 / -10.0) */
/**@}*/
//-------------- next part --------------
/**
 * @file image.h
 * Image dimensions
 * @author Marko Mäkelä (msmakela at nic.funet.fi)
 */

/** Image depth in pixels */
#define IMAGE_DEPTH	3
//-------------- next part --------------
/**
 * @file lenscorr.c
 * Adjust for lens distortion
 * @author Marko Mäkelä (msmakela at nic.funet.fi)
 */

/* Copyright © 2004 Marko Mäkelä
 * 
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with this program; if not, write to the Free Software
 *     Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/** Adjust for lens distortion and scale the image.
 * @param src		buffer for the source image
 *			(C * R * IMAGE_DEPTH)
 * @param dst		buffer for the destination image
 *			(C * R * IMAGE_DEPTH)
 * @param C		destination image C
 * @param R	destination image R
 */
void
lens_correct (const UINT8T* src, UINT8T* dst);
//-------------- next part --------------
/**
 * @file lenscorr.c
 * Adjust for lens distortion
 * @author Marko Mäkelä (msmakela at nic.funet.fi)
 * @author David Hodson (hodsond at acm.org)
 */

/* Copyright © 2001-2003 David Hodson
 * Copyright © 2004 Marko Mäkelä
 * 
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with this program; if not, write to the Free Software
 *     Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/* The algorithm is from the "wideangle" Gimp plugin written by David Hodson.
   This implementation of the algorithm was written by Marko Mäkelä. */

//#include <math.h>
//#include "image.h"
//#include "lens.h"

/** integer part of the x-coordinate of the current source point, minus 1 */
static int ix;
/** integer part of the y-coordinate of the current source point, minus 1 */
static int iy;
/** fractional part of the current source point; point = (d[0],d[1])+(ix,iy) */
static double fp[2];
#ifdef LENS_BRIGHT
/** brightening factor for the current pixel */
static double brighten;
#endif /* LENS_BRIGHT */

/** Get the source point corresponding to the
 * destination point (i,j), relative to the center of the destination image.
 * Assign it to (1,1)+(ix,iy)+fp, relative to the top left of the source image.
 * The destination point at distance r from the center is mapped to
 * distance LENS_SCALE * r * (1 + LENS_SQUARE * r^2 + LENS_QUAD * r^4).
 * @param off_x	x-coordinate of destination point
 * @param off_y	y-coordinate of destination point
 */
static void
map_point (double off_x, double off_y)
{
  double d, radius_mult;

  /* compute the distance from the center of the image, squared */
  d = (off_x * off_x) + (off_y * off_y);
  d *= 4.0 / (C * C + R * R);

  /* map distance r from center to radius_mult * (r + 1); below d = r^2 */
  radius_mult = d * LENS_SQUARE + d * d * LENS_QUAD;
#ifdef LENS_BRIGHT
  brighten = 1.0 + radius_mult * LENS_BRIGHT;
#endif /* LENS_BRIGHT */
  radius_mult += 1.0;
#ifdef LENS_SCALE
  radius_mult *= LENS_SCALE;
#endif /* LENS_SCALE */

  /* (src_x,src_y) = (center_x,center_y) + radius_mult * (off_x,off_y) */
  /* (ix,iy) = integer part of (src_x,src_y)-(1,1) */
  /* (fp[0],fp[1]) = fractional part of (src_x,src_y) */
  d = floor (fp[0] = off_x * radius_mult);
  fp[0] -= d, ix = (int) d + (C / 2 - 1);
  d = floor (fp[1] = off_y * radius_mult);
  fp[1] -= d, iy = (int) d + (R / 2 - 1);
}

/**
 * Catmull-Rom cubic interpolation
 *
 * equally spaced points p0, p1, p2, p3
 * interpolate 0 <= u < 1 between p1 and p2
 *
 * (1 u u^2 u^3) (  0.0  1.0  0.0  0.0 ) (p0)
 *               ( -0.5  0.0  0.5  0.0 ) (p1)
 *               (  1.0 -2.5  2.0 -0.5 ) (p2)
 *               ( -0.5  1.5 -1.5  0.5 ) (p3)
 *
 * @param src	source image
 * @param dst	pixel to plot in destination image
 */
static void
cubic_interpolate (const char* src, char* dst)
{
  /** counter */
  int i, j;
  /** vertically interpolated pixel values */
  float verts[4 * IMAGE_DEPTH];
  /** interpolated fractional coordinates */
  float u[2][4];

  for (i = 2; i--; ) {
    register float f = fp[i];
    u[i][0] = /* p - 1 */	((-0.5 * f + 1.0) * f - 0.5) * f;
    u[i][1] = /* p */		(1.5 * f - 2.5) * f * f + 1.0;
    u[i][2] = /* p + 1 */	((-1.5 * f + 2.0) * f + 0.5) * f;
    u[i][3] = /* p + 2 */	(0.5 * f - 0.5) * f * f;
  }

  /* loop over x coordinates and depth */
  for (i = 0; i < 4 * IMAGE_DEPTH; i++) {
    /* map pixels outside the source image to value 0 */
    verts[i] = 0;
    if (IMAGE_DEPTH * ix + i >= 0 &&
	IMAGE_DEPTH * ix + i < IMAGE_DEPTH * C) {
      const char* const s = src + (IMAGE_DEPTH * ix + i);
      /* loop over y coordinates */
      for (j = 4; j--; )
	if (iy + j >= 0 && iy + j < R)
	  verts[i] += u[1][j] * (unsigned char)
	    s[(iy + j) * IMAGE_DEPTH * C];
    }
  }

  for (i = 0; i < IMAGE_DEPTH; i++) {
    float result = 0;
    for (j = 0; j < 4; j++)
      result += u[0][j] * verts[i + IMAGE_DEPTH * j];
#ifdef LENS_BRIGHT
    result *= brighten;
#endif /* LENS_BRIGHT */
    if (result < 0.0)
      *dst++ = 0;
    else if (result > 255.0)
      *dst++ = (char) 255;
    else
      *dst++ = result;
  }
}

/** Adjust for lens distortion and scale the image.
 * @param src		buffer for the source image
 *			(C * R * IMAGE_DEPTH)
 * @param dst		buffer for the destination image
 *			(C * R * IMAGE_DEPTH)
 * @param C		destination image C
 * @param R	destination image R
 */
void
lens_correct (UINT8T* src, UINT8T* dst)
{
  /** horizontal step in source image coordinates */
  const double step_x = (double) C / C;
  /** vertical step in source image coordinates */
  const double step_y = (double) R / R;
  /** point in destination image, relative to origin,
   * scaled to (C, R)
   */
  double off_x, off_y;
  /** counters */
  unsigned i, j;

  for (j = R, off_y = step_y * R / -2; j--; off_y += step_y) {
    for (i = C, off_x = step_x * C / -2; i--; off_x += step_x) {
      map_point (off_x, off_y);
      cubic_interpolate (src, dst);
      dst += IMAGE_DEPTH;
    }
  }
}