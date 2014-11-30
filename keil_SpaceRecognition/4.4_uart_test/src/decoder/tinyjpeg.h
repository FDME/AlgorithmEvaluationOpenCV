#ifndef __JPEGDEC_H__
#define __JPEGDEC_H__

#ifdef __cplusplus
extern "C" {
#endif

struct jdec_private;

/* Flags that can be set by any applications */
#define TINYJPEG_FLAGS_MJPEG_TABLE	(1<<1)

/* Format accepted in outout */
enum tinyjpeg_fmt {
   TINYJPEG_FMT_GREY = 1,
   TINYJPEG_FMT_BGR24,
   TINYJPEG_FMT_RGB24,
   TINYJPEG_FMT_YUV420P,
};

//struct jdec_private *tinyjpeg_init(void);
void tinyjpeg_free(struct jdec_private *priv);

int tinyjpeg_parse_header(struct jdec_private *priv, const unsigned char *buf, unsigned int size);
int tinyjpeg_decode(struct jdec_private *priv, int pixel_format,unsigned char *pY,unsigned char *pU,unsigned char *pV);
const char *tinyjpeg_get_errorstring(struct jdec_private *priv);
void tinyjpeg_get_size(struct jdec_private *priv, unsigned int *width, unsigned int *height);
int tinyjpeg_get_components(struct jdec_private *priv, unsigned char **components);
int tinyjpeg_set_components(struct jdec_private *priv, unsigned char **components, unsigned int ncomponents);
int tinyjpeg_set_flags(struct jdec_private *priv, int flags);

#ifdef __cplusplus
}
#endif

#endif



