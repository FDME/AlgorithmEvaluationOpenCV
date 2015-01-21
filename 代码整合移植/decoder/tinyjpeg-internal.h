#include "../header.h"

#ifndef __TINYJPEG_INTERNAL_H_
#define __TINYJPEG_INTERNAL_H_

#include <setjmp.h>

#define SANITY_CHECK 1

struct jdec_private;

#define HUFFMAN_BITS_SIZE  256
#define HUFFMAN_HASH_NBITS 9
#define HUFFMAN_HASH_SIZE  (1UL<<HUFFMAN_HASH_NBITS)
#define HUFFMAN_HASH_MASK  (HUFFMAN_HASH_SIZE-1)

#define HUFFMAN_TABLES	   4
#define COMPONENTS	   3
#define JPEG_MAX_WIDTH	   2048
#define JPEG_MAX_HEIGHT	   2048

struct huffman_table
{
  /* Fast look up table, using HUFFMAN_HASH_NBITS bits we can have directly the symbol,
   * if the symbol is <0, then we need to look into the tree table */
  short int lookup[HUFFMAN_HASH_SIZE];
  /* code size: give the number of bits of a symbol is encoded */
  unsigned char code_size[HUFFMAN_HASH_SIZE];
  /* some place to store value that is not encoded in the lookup table 
   * FIXME: Calculate if 256 value is enough to store all values
   */
  UINT16T slowtable[16-HUFFMAN_HASH_NBITS][256];
};

struct component 
{
  unsigned int Hfactor;
  unsigned int Vfactor;
  float *Q_table;		/* Pointer to the quantisation table to use */
  struct huffman_table *AC_table;
  struct huffman_table *DC_table;
  short int previous_DC;	/* Previous DC coefficient */
  short int DCT[64];		/* DCT coef */
#if SANITY_CHECK
  unsigned int cid;
#endif
};


typedef void (*decode_MCU_fct) (struct jdec_private *priv);
typedef void (*convert_colorspace_fct) (struct jdec_private *priv);

struct jdec_private
{
  /* Public variables */
  UINT8T *components[COMPONENTS];
  unsigned int width, height;	/* Size of the image */
  unsigned int flags;

  /* Private variables */
  const unsigned char *stream_begin, *stream_end;
  unsigned int stream_length;

  const unsigned char *stream;	/* Pointer to the current stream */
  unsigned int reservoir, nbits_in_reservoir;

  struct component component_infos[COMPONENTS];
  float Q_tables[COMPONENTS][64];		/* quantization tables */
  struct huffman_table HTDC[HUFFMAN_TABLES];	/* DC huffman tables   */
  struct huffman_table HTAC[HUFFMAN_TABLES];	/* AC huffman tables   */
  int default_huffman_table_initialized;
  int restart_interval;
  int restarts_to_go;				/* MCUs left in this restart interval */
  int last_rst_marker_seen;			/* Rst marker is incremented each time */

  /* Temp space used after the IDCT to store each components */
  UINT8T Y[64*4], Cr[64], Cb[64];

  jmp_buf jump_state;
  /* Internal Pointer use for colorspace conversion, do not modify it !!! */
  UINT8T *plane[COMPONENTS];

};

#if defined(__GNUC__) && (__GNUC__ > 3) && defined(__OPTIMIZE__)
#define __likely(x)       __builtin_expect(!!(x), 1)
#define __unlikely(x)     __builtin_expect(!!(x), 0)
#else
#define __likely(x)       (x)
#define __unlikely(x)     (x)
#endif

#define IDCT tinyjpeg_idct_float
void tinyjpeg_idct_float (struct component *compptr, UINT8T *output_buf, int stride);

#endif

