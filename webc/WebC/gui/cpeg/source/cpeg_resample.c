/*
 *		Filtered Image Resample
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include "cpeg_resample.h"

#if 0 
static char	_Program[] = "fzoom";
static char	_Version[] = "0.20";
static char	_Copyright[] = "Public Domain 1991 by Dale Schumacher";
#endif

#define M_PI 3.1415926535897932384626433832795

/* clamp the input to the specified range */
#define CLAMP(v,l,h)    ((v)<(l) ? (l) : (v) > (h) ? (h) : v)

#ifndef EXIT_SUCCESS
#define	EXIT_SUCCESS	(0)
#define	EXIT_FAILURE	(1)
#endif

#define	WHITE_PIXEL	(255)
#define	BLACK_PIXEL	(0)

/*
 *	generic image access and i/o support routines
 */

static char *
next_token(f)
FILE *f;
{
	static char delim[] = " \t\r\n";
	static char *t = NULL;
	static char lnbuf[256];
	char *p;

	while(t == NULL) {			/* nothing in the buffer */
		if(fgets(lnbuf, sizeof(lnbuf), f)) {	/* read a line */
			if(p = strchr(lnbuf, '#')) {	/* clip any comment */
				*p = '\0';
			}
			t = strtok(lnbuf, delim);	/* get first token */
		} else {
			return(NULL);
		}
	}
	p = t;
	t = strtok((char *)NULL, delim);		/* get next token */
	return(p);
}

rszImage *
new_image(xsize, ysize, channels)	/* create a blank image */
int xsize, ysize, channels;
{
	rszImage *image;

	if((image = (rszImage *)malloc(sizeof(rszImage)))
	&& (image->data = (Pixel *)calloc(ysize, xsize * channels))) {
		image->xsize = xsize;
		image->ysize = ysize;
		image->span = xsize * channels;
		image->channels = channels;
	}
	return(image);
}

void
free_image(image)
rszImage *image;
{
	free(image->data);
	free(image);
}

rszImage *
load_image(f)		/* read image from file */
FILE *f;
{
	char *p;
	int width, height;
	rszImage *image;

	if(((p = next_token(f)) && (strcmp(p, "Bm") == 0))
	&& ((p = next_token(f)) && ((width = atoi(p)) > 0))
	&& ((p = next_token(f)) && ((height = atoi(p)) > 0))
	&& ((p = next_token(f)) && (strcmp(p, "8") == 0))
	&& (image = new_image(width, height, 1))
	&& (fread(image->data, (size_t) width, (size_t) height, f) == (size_t)height)) {
		return(image);		/* load successful */
	} else {
		return(NULL);		/* load failed */
	}
}

int
save_image(f, image)	/* write image to file */
FILE *f;
rszImage *image;
{
	fprintf(f, "Bm # PXM 8-bit greyscale image\n");
	fprintf(f, "%d %d 8 # width height depth\n",
		image->xsize, image->ysize);
	if(fwrite(image->data, image->xsize, image->ysize, f) == (size_t) (image->ysize)) {
		return(0);		/* save successful */
	} else {
		return(-1);		/* save failed */
	}
}

Pixel
get_pixel(image, x, y)
rszImage *image;
int x, y;
{
	static rszImage *im = NULL;
	static int yy = -1;
	static Pixel *p = NULL;

	if((x < 0) || (x >= image->xsize) || (y < 0) || (y >= image->ysize)) {
		return(0);
	}
	if((im != image) || (yy != y)) {
		im = image;
		yy = y;
		p = image->data + (y * image->span);
	}
	return(p[x]);
}

void
get_row(row, image, y)
Pixel *row;
rszImage *image;
int y;
{
	if((y < 0) || (y >= image->ysize)) {
		return;
	}
	memcpy(row,
		image->data + (y * image->span),
		(sizeof(Pixel) * image->xsize * image->channels));
}

void
get_column(column, image, x, channel)
Pixel *column;
rszImage *image;
int x;
int channel;
{
	int i, d;
	Pixel *p;

	if((x < 0) || (x >= image->xsize)) {
		return;
	}
	d = image->span;
	for(i = image->ysize, p = image->data + (image->channels * x) + channel; i-- > 0; p += d) {
		*column++ = *p;
	}
}

Pixel
put_pixel(image, x, y, channel, data)
rszImage *image;
int x, y, channel;
Pixel data;
{
	static rszImage *im = NULL;
	static int yy = -1;
	static Pixel *p = NULL;

	if((x < 0) || (x >= image->xsize) || (y < 0) || (y >= image->ysize)) {
		return(0);
	}
	if((im != image) || (yy != y)) {
		im = image;
		yy = y;
		p = image->data + (y * image->span);
	}
	return(p[(x * image->channels) + channel] = data);
}


/*
 *	filter function definitions
 */

#define	filter_support		(1.0)

double
filter(t)
double t;
{
	/* f(t) = 2|t|^3 - 3|t|^2 + 1, -1 <= t <= 1 */
	if(t < 0.0) t = -t;
	if(t < 1.0) return((2.0 * t - 3.0) * t * t + 1.0);
	return(0.0);
}

#define	box_support		(0.5)

double
box_filter(t)
double t;
{
	if((t > -0.5) && (t <= 0.5)) return(1.0);
	return(0.0);
}

#define	triangle_support	(1.0)

double
triangle_filter(t)
double t;
{
	if(t < 0.0) t = -t;
	if(t < 1.0) return(1.0 - t);
	return(0.0);
}

#define	bell_support		(1.5)

double
bell_filter(t)		/* box (*) box (*) box */
double t;
{
	if(t < 0) t = -t;
	if(t < .5) return(.75 - (t * t));
	if(t < 1.5) {
		t = (t - 1.5);
		return(.5 * (t * t));
	}
	return(0.0);
}

#define	B_spline_support	(2.0)

double
B_spline_filter(t)	/* box (*) box (*) box (*) box */
double t;
{
	double tt;

	if(t < 0) t = -t;
	if(t < 1) {
		tt = t * t;
		return((.5 * tt * t) - tt + (2.0 / 3.0));
	} else if(t < 2) {
		t = 2 - t;
		return((1.0 / 6.0) * (t * t * t));
	}
	return(0.0);
}

double
sinc(x)
double x;
{
	x *= M_PI;
	if(x != 0) return(sin(x) / x);
	return(1.0);
}

#define	Lanczos3_support	(3.0)

double
Lanczos3_filter(t)
double t;
{
	if(t < 0) t = -t;
	if(t < 3.0) return(sinc(t) * sinc(t/3.0));
	return(0.0);
}

#define	Mitchell_support	(2.0)

#define	B	(1.0 / 3.0)
#define	C	(1.0 / 3.0)

double
Mitchell_filter(t)
double t;
{
	double tt;

	tt = t * t;
	if(t < 0) t = -t;
	if(t < 1.0) {
		t = (((12.0 - 9.0 * B - 6.0 * C) * (t * tt))
		   + ((-18.0 + 12.0 * B + 6.0 * C) * tt)
		   + (6.0 - 2 * B));
		return(t / 6.0);
	} else if(t < 2.0) {
		t = (((-1.0 * B - 6.0 * C) * (t * tt))
		   + ((6.0 * B + 30.0 * C) * tt)
		   + ((-12.0 * B - 48.0 * C) * t)
		   + (8.0 * B + 24 * C));
		return(t / 6.0);
	}
	return(0.0);
}

/*
 *	image rescaling routine
 */

typedef struct {
	int	pixel;
	double	weight;
} CONTRIB;

typedef struct {
	int	n;		/* number of contributors */
	CONTRIB	*p;		/* pointer to list of contributions */
} CLIST;

CLIST	*contrib;		/* array of contribution lists */

void
resample(dst, src, filterf, fwidth)
rszImage *dst;				/* destination image structure */
rszImage *src;				/* source image structure */
double (*filterf)(double);			/* filter function */
double fwidth;				/* filter width (support) */
{
	rszImage *tmp;			/* intermediate image */
	double xscale, yscale;		/* zoom scale factors */
	int i, j, k, channel;			/* loop variables */
	int n;				/* pixel number */
	double center, left, right;	/* filter calculation variables */
	double width, fscale, weight;	/* filter calculation variables */
	Pixel *raster;			/* a row or column of pixels */

	/* create intermediate image to hold horizontal zoom */
	tmp = new_image(dst->xsize, src->ysize, src->channels);
	xscale = (double) dst->xsize / (double) src->xsize;
	yscale = (double) dst->ysize / (double) src->ysize;

	/* pre-calculate filter contributions for a row */
	contrib = (CLIST *)calloc(dst->xsize, sizeof(CLIST));
	if(xscale < 1.0) {
		width = fwidth / xscale;
		fscale = 1.0 / xscale;
		for(i = 0; i < dst->xsize; ++i) {
			contrib[i].n = 0;
			contrib[i].p = (CONTRIB *)calloc((int) (width * 2 + 1),
					sizeof(CONTRIB));
			center = (double) i / xscale;
			left = ceil(center - width);
			right = floor(center + width);
			for(j = (int) left; j <= right; ++j) {
				weight = center - (double) j;
				weight = (*filterf)(weight / fscale) / fscale;
				if(j < 0) {
					n = -j;
				} else if(j >= src->xsize) {
					n = (src->xsize - j) + src->xsize - 1;
				} else {
					n = j;
				}
				k = contrib[i].n++;
				contrib[i].p[k].pixel = n;
				contrib[i].p[k].weight = weight;
			}
		}
	} else {
		for(i = 0; i < dst->xsize; ++i) {
			contrib[i].n = 0;
			contrib[i].p = (CONTRIB *)calloc((int) (fwidth * 2 + 1),
					sizeof(CONTRIB));
			center = (double) i / xscale;
			left = ceil(center - fwidth);
			right = floor(center + fwidth);
			for(j = (int)left; j <= right; ++j) {
				weight = center - (double) j;
				weight = (*filterf)(weight);
				if(j < 0) {
					n = -j;
				} else if(j >= src->xsize) {
					n = (src->xsize - j) + src->xsize - 1;
				} else {
					n = j;
				}
				k = contrib[i].n++;
				contrib[i].p[k].pixel = n;
				contrib[i].p[k].weight = weight;
			}
		}
	}

	/* apply filter to zoom horizontally from src to tmp */
	raster = (Pixel *)calloc(src->xsize, sizeof(Pixel) * src->channels);
	for(k = 0; k < tmp->ysize; ++k) {
		get_row(raster, src, k);
		for(channel=0; channel < src->channels; channel++) {
			for(i = 0; i < tmp->xsize; ++i) {
				weight = 0.0;
				for(j = 0; j < contrib[i].n; ++j) {
					weight += raster[(contrib[i].p[j].pixel * src->channels) + channel]
						* contrib[i].p[j].weight;
				}
				put_pixel(tmp, i, k, channel,
					(Pixel)CLAMP(weight, BLACK_PIXEL, WHITE_PIXEL));
			}
		}
	}
	free(raster);

	/* free the memory allocated for horizontal filter weights */
	for(i = 0; i < tmp->xsize; ++i) {
		free(contrib[i].p);
	}
	free(contrib);

	/* pre-calculate filter contributions for a column */
	contrib = (CLIST *)calloc(dst->ysize, sizeof(CLIST));
	if(yscale < 1.0) {
		width = fwidth / yscale;
		fscale = 1.0 / yscale;
		for(i = 0; i < dst->ysize; ++i) {
			contrib[i].n = 0;
			contrib[i].p = (CONTRIB *)calloc((int) (width * 2 + 1),
					sizeof(CONTRIB));
			center = (double) i / yscale;
			left = ceil(center - width);
			right = floor(center + width);
			for(j = (int) left; j <= right; ++j) {
				weight = center - (double) j;
				weight = (*filterf)(weight / fscale) / fscale;
				if(j < 0) {
					n = -j;
				} else if(j >= tmp->ysize) {
					n = (tmp->ysize - j) + tmp->ysize - 1;
				} else {
					n = j;
				}
				k = contrib[i].n++;
				contrib[i].p[k].pixel = n;
				contrib[i].p[k].weight = weight;
			}
		}
	} else {
		for(i = 0; i < dst->ysize; ++i) {
			contrib[i].n = 0;
			contrib[i].p = (CONTRIB *)calloc((int) (fwidth * 2 + 1),
					sizeof(CONTRIB));
			center = (double) i / yscale;
			left = ceil(center - fwidth);
			right = floor(center + fwidth);
			for(j = (int)left; j <= right; ++j) {
				weight = center - (double) j;
				weight = (*filterf)(weight);
				if(j < 0) {
					n = -j;
				} else if(j >= tmp->ysize) {
					n = (tmp->ysize - j) + tmp->ysize - 1;
				} else {
					n = j;
				}
				k = contrib[i].n++;
				contrib[i].p[k].pixel = n;
				contrib[i].p[k].weight = weight;
			}
		}
	}

	/* apply filter to zoom vertically from tmp to dst */
	raster = (Pixel *)calloc(tmp->ysize, sizeof(Pixel));
	for(k = 0; k < dst->xsize; ++k) {
		for(channel=0; channel < src->channels; channel++) {
			get_column(raster, tmp, k, channel);
			for(i = 0; i < dst->ysize; ++i) {
				weight = 0.0;
				for(j = 0; j < contrib[i].n; ++j) {
					weight += raster[contrib[i].p[j].pixel]
						* contrib[i].p[j].weight;
				}
				put_pixel(dst, k, i, channel,
					(Pixel)CLAMP(weight, BLACK_PIXEL, WHITE_PIXEL));
			}
		}
	}
	free(raster);

	/* free the memory allocated for vertical filter weights */
	for(i = 0; i < dst->ysize; ++i) {
		free(contrib[i].p);
	}
	free(contrib);

	free_image(tmp);
}

void resample_palettized(
rszImage *dst,				/* destination image structure */
rszImage *src,				/* source image structure */
double (*filterf)(double),			/* filter function */
double fwidth,				/* filter width (support) */
int *palettePtr,				/* pointer to pallete entries */
int paletteEntries				/* number of pallete entries */
)
{
}


#if 0

/*
 *	command line interface
 */

void
usage()
{
	fprintf(stderr, "usage: %s [-options] input.bm output.bm\n", _Program);
	fprintf(stderr, "\
options:\n\
	-x xsize		output x size\n\
	-y ysize		output y size\n\
	-f filter		filter type\n\
{b=box, t=triangle, q=bell, B=B-spline, h=hermite, l=Lanczos3, m=Mitchell}\n\
");
	exit(1);
}

void
banner()
{
	printf("%s v%s -- %s\n", _Program, _Version, _Copyright);
}


main(argc, argv)
int argc;
char *argv[];
{
	register int c;
	extern int optind;
	extern char *optarg;
	int xsize = 0, ysize = 0;
	double (*f)() = filter;
	double s = filter_support;
	char *dstfile, *srcfile;
	rszImage *dst, *src;
	FILE *fp;

	while((c = getopt(argc, argv, "x:y:f:V")) != EOF) {
		switch(c) {
		case 'x': xsize = atoi(optarg); break;
		case 'y': ysize = atoi(optarg); break;
		case 'f':
			switch(*optarg) {
			case 'b': f=box_filter; s=box_support; break;
			case 't': f=triangle_filter; s=triangle_support; break;
			case 'q': f=bell_filter; s=bell_support; break;
			case 'B': f=B_spline_filter; s=B_spline_support; break;
			case 'h': f=filter; s=filter_support; break;
			case 'l': f=Lanczos3_filter; s=Lanczos3_support; break;
			case 'm': f=Mitchell_filter; s=Mitchell_support; break;
			default: usage();
			}
			break;
		case 'V': banner(); exit(EXIT_SUCCESS);
		case '?': usage();
		default:  usage();
		}
	}
	if((argc - optind) != 2) usage();
	srcfile = argv[optind];
	dstfile = argv[optind + 1];
	if(((fp = fopen(srcfile, "r")) == NULL)
	|| ((src = load_image(fp)) == NULL)) {
		fprintf(stderr, "%s: can't load source image '%s'\n",
			_Program, srcfile);
		exit(EXIT_FAILURE);
	}
	fclose(fp);
	if(xsize <= 0) xsize = src->xsize;
	if(ysize <= 0) ysize = src->ysize;
	dst = new_image(xsize, ysize, src->channels);
	resample(dst, src, f, s);
	if(((fp = fopen(dstfile, "w")) == NULL)
	|| (save_image(fp, dst) != 0)) {
		fprintf(stderr, "%s: can't save destination image '%s'\n",
			_Program, dstfile);
		exit(EXIT_FAILURE);
	}
	fclose(fp);
	exit(EXIT_SUCCESS);
}

#endif