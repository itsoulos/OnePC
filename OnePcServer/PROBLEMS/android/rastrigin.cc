# include <math.h>

extern "C"
{

int	getdimension()
{
	return 2;
}

void	getleftmargin(double *x)
{
	x[0]=-1;
	x[1]=-1;
}

void	getrightmargin(double *x)
{
	x[0]=1;
	x[1]=1;
}

double	funmin(double *x)
{
   return x[0]*x[0]+x[1]*x[1]-cos(18.0*x[0])-cos(18.0*x[1]);
}

void    granal(double *x,double *g)
{
      g[0]=2.0*x[0]+18.0*sin(18.0*x[0]);
      g[1]=2.0*x[1]+18.0*sin(18.0*x[1]);
}

}
