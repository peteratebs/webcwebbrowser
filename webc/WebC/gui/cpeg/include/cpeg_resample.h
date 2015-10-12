/*
 *		Filtered Image Resample Interface
 *
 */
#ifndef _RESIZE_H
#define _RESIZE_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


typedef	unsigned char	Pixel;

typedef struct {
	int	xsize;		/* horizontal size of the image in Pixels */
	int	ysize;		/* vertical size of the image in Pixels */
	Pixel *	data;		/* pointer to first scanline of image */
	int	span;		/* byte offset between two scanlines */
	int channels;   /* number of color channels interleaved in data */
} rszImage;


void resample(
rszImage *dst,				/* destination image structure */
rszImage *src,				/* source image structure */
double (*filterf)(double),			/* filter function */
double fwidth);				/* filter width (support) */


void resample_palettized(
rszImage *dst,				/* destination image structure */
rszImage *src,				/* source image structure */
double (*filterf)(double),			/* filter function */
double fwidth,				/* filter width (support) */
int *palettePtr,				/* pointer to pallete entries */
int paletteEntries				/* number of pallete entries */
);


// supported filters
double filter(double t);
double box_filter(double t);
double triangle_filter(double t);
double bell_filter(double t);
double B_spline_filter(double t);
double sinc(double t);
double Lanczos3_filter(double t);
double Mitchell_filter(double t);
// or define one yourself in "C" 

#ifdef __cplusplus
}
#endif

#endif