# include <math.h>
# include <stdio.h>

static double dmax(double a,double b){return a>b?a:b;}
typedef double(*FUNCTION)(double,void*);

extern "C"
{

# define NODE_COUNT	3
# define DIMENSION	1
# define KAPPA		1
# define ALPHA		0.0
# define BETA		40.0
# define INTEGRATION	1000.0

/*	Integration of the function f using the Simpson rule.
 *	The integration is performed from point a to point b.
 * */
double	SimpsonIntegrate(FUNCTION f,void *param,double a,double b)
{
	const int NSIMPSON = 1000;
	const double eps = 1e-7;
	double sum = 0.0;
	for(int i=0;i<NSIMPSON;i++)
	{
		double x=a+(b-a)*i/(NSIMPSON-1.0);
		double factor = 1.0;
		if(i==0 || i==NSIMPSON-1) factor=1.0;
		else
		if(i%2==1) factor=4.0;
		else
		factor = 2.0;
		sum+=factor*f(x,param);
	}
	return eps/3.0 * sum;
}

/*	The sigmoidal function used in the neural networks.
 * */
double	sig(double x)
{
	return 1.0/(1.0+exp(-x));
}

/*	Evaluate the neural network at point x.
 * */
double	getValue(double *node,double *x)
{
	double arg=0.0;
	double per=0.0;
	int nodes=NODE_COUNT;
	for(int i=1;i<=nodes;i++)
	{
		arg=0.0;
		for(int j=1;j<=DIMENSION;j++)
		{
			int pos=(DIMENSION+2)*i-(DIMENSION+1)+j-1;
			arg+=node[pos]*x[j-1];
		}
		arg+=node[(DIMENSION+2)*i-1];
		per+=node[(DIMENSION+2)*i-(DIMENSION+1)-1]*sig(arg);
	}
	return per;
}

/*	Evaluate the first derivative with respect to index,
 *	of the neural network at point x
 * */
double	getDValue(double *node,double *x,int index)
{
	double arg=0.0;
	double per=0.0;
	int nodes=NODE_COUNT;
	for(int i=1;i<=nodes;i++)
	{
		arg=0.0;
		for(int j=1;j<=DIMENSION;j++)
		{
			int pos=(DIMENSION+2)*i-(DIMENSION+1)+j-1;
			arg+=node[pos]*x[j-1];
		}
		arg+=node[(DIMENSION+2)*i-1];
		double s = sig(arg);
		per+=node[(DIMENSION+2)*i-(DIMENSION+1)-1]*s*(1.0 - s)*
			node[(DIMENSION+2)*i-(DIMENSION+1)+index-1];
	}
	return per;
}

/*	Evaluate the second derivative with respect to index,
 *	of the neural network at point x
*/
double	getD2Value(double *node,double *x,int index)
{
	double arg=0.0;
	double per=0.0;
	int nodes=NODE_COUNT;
	for(int i=1;i<=nodes;i++)
	{
		arg=0.0;
		for(int j=1;j<=DIMENSION;j++)
		{
			int pos=(DIMENSION+2)*i-(DIMENSION+1)+j-1;
			arg+=node[pos]*x[j-1];
		}
		arg+=node[(DIMENSION+2)*i-1];
		double s = sig(arg);
		per+=node[(DIMENSION+2)*i-(DIMENSION+1)-1]*s*(1.0 - s)*(1.0-2*s)*
			pow(node[(DIMENSION+2)*i-(DIMENSION+1)+index-1],2.0);
	}
	return per;
}

/*	Return the dimension of the problem to be solved.
 * */
int	getdimension()
{
	return (DIMENSION+2) * NODE_COUNT;
}

void	getleftmargin(double *x)
{
	for(int i=0;i<(DIMENSION+2)*NODE_COUNT;i++)
		x[i]=-1e+2;
}

void	getrightmargin(double *x)
{
	for(int i=0;i<(DIMENSION+2)*NODE_COUNT;i++)
		x[i]= 1e+2;
}

/* Eq 27
 * */
double	u(double r,double *x)
{
	return r * exp(-KAPPA * r) * getValue(x,&r);
}

/*	The first derivative of equation 27 with respect to x.
 * */
double	du(double r,double *x)
{
	return r * exp(-KAPPA *r) *getDValue(x,&r,1)+
		getValue(x,&r)*(exp(-KAPPA*r)+r*(-KAPPA)*exp(-KAPPA*r));
}

/*	The second derivative of equation 27 with respect to x.
 * */
double	d2u(double r,double *x)
{
	return (exp(-KAPPA*r)+r*(-KAPPA)*exp(-KAPPA*r))*getDValue(x,&r,1)+
	       r*exp(-KAPPA*r)*getD2Value(x,&r,1)+
	getDValue(x,&r,1)*(exp(-KAPPA*r)+r*(-KAPPA)*exp(-KAPPA)*r)+
	getValue(x,&r)*(-KAPPA*exp(-KAPPA*r)+(-KAPPA)*exp(-KAPPA * r)+r*(-KAPPA)*(-KAPPA)*exp(-KAPPA*r));
}

/*	The function to be integrated for equation 22.
 * */
double	f1_integral(double r_prime,void *param)
{
	double *xparam=(double*)param;
	double e_q = xparam[0];
	double rho = xparam[1];
	double r   = xparam[2];
	double f =  rho /fabs(r -r_prime);
	if(isnan(f) || isinf(f)) return rho/1e-15;//1e-20;
	return f;
}

/*	The function to be integrated for equation 24.
 * */
double	f2_integral(double r,void *param)
{
	double *xparam = (double *)param;
	double V = xparam[0];
	double *node = &xparam[1];
	double u1 = u(r,node);
	double u2 = du(r,node);
	double f1=u2 * u2;
	double f2=V*u1*u1;
	return f1+f2;
}

/*	The function to be integrated for the normalization factor N0.
 * */
double	f3_integral(double r,void *param)
{
	double *xparam = (double *)param;
	double u1= u(r,xparam);
	return u1 * u1;
}

/*	The objective function (equation 25).
 * */
double	funmin(double *x)
{
	double h_bar = 1.0546*1.0e-34;
	double e_q   = 1.6*1.0e-19;
	double mass_mi = 1.0*1.6*1e-27; //grammars
	double scale_factor = h_bar * h_bar / (2.0 * mass_mi);
	double rho_0 = 1.0;
	double c_p = 1.0;
	double z_p = 1.0;
	int    n = 100;
	double  funmin_0 = 0.0;
	double *f2_params=new double[1+getdimension()];			
	for(int j=0;j<getdimension();j++) f2_params[j+1]=x[j];
	for(int i=0;i<n;i++)
	{
		double r = ALPHA +(BETA-ALPHA)*i*1.0/(n-1.0);
		
		double rho = 1.0 +exp((r-c_p)/z_p);	
		rho = rho_0/rho;
		
/*		Compute the potential
 * */
		double f1_params[3];
		f1_params[0]=e_q;
		f1_params[1]=rho;
		f1_params[2]=r;
		double v_potential,energy,N0,error;
		v_potential = SimpsonIntegrate(f1_integral,f1_params,0.0,INTEGRATION);
		v_potential = -e_q * e_q * v_potential;

/*		Compute the energy
 * */
		f2_params[0]=v_potential;
		energy = SimpsonIntegrate(f2_integral,f2_params,ALPHA,(i==0)?ALPHA+(BETA-ALPHA)/10000:r);
		N0=SimpsonIntegrate(f3_integral,&f2_params[1],ALPHA,(i==0)?ALPHA+(BETA-ALPHA)/10000:r);
		N0 = 1.0/ N0;
		energy = N0 *energy;
		double u1 = u(r,x);
		double u2 = d2u(r,x);

/*		Compute the equation 25
 * */
		double value = -u2+v_potential*u1/scale_factor-energy*u1;
		funmin_0+=pow(value,2.0);
	}
	delete[] f2_params;
	return funmin_0;
}

/*	The first derivative of the objective function.
 * */
void    granal(double *x,double *g)
{
	for(int i=0;i<getdimension();i++)
	{
		double eps=pow(1e-18,1.0/3.0)*dmax(1.0,fabs(x[i]));
		x[i]+=eps;
		double v1=funmin(x);
		x[i]-=2.0 *eps;
		double v2=funmin(x);
		g[i]=(v1-v2)/(2.0 * eps);
		x[i]+=eps;
	}
}

}
