
#define M_PI 3.1415926535897932384626433832795
#define	filter_support		(1.0)
#define CLAMP(v,l,h)    ((v)<(l) ? (l) : (v) > (h) ? (h) : v)
typedef struct {
	int	pixel;
	double	weight;
} CONTRIB;

typedef struct {
	int	n;		/* number of contributors */
	CONTRIB	*p;		/* pointer to list of contributions */
} CLIST;

double filter(double t);

#define	box_support		(0.5)

double box_filter(double t);

#define	triangle_support	(1.0)

double triangle_filter(double t);

#define	bell_support		(1.5)

double bell_filter(double t);		/* box (*) box (*) box */

#define	B_spline_support	(2.0)

double B_spline_filter(double t);	/* box (*) box (*) box (*) box */

double sinc(double x);

#define	Lanczos3_support	(3.0)

double Lanczos3_filter(double t);

#define	Mitchell_support	(2.0)

#define	B	(1.0 / 3.0)
#define	C	(1.0 / 3.0)

double Mitchell_filter(double t);
