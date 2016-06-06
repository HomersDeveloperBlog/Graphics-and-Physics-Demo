#include "stdafx.h"

#include "LibrarySnippets.h"

void ublasDemo()
{
  c_vector<double, 3> a;
  ublas::matrix<double> mat(6,6);
  ublas::vector<double> v (6);
  for (unsigned int i = 0;
   i < v.size(); ++ i)
    v (i) = i;

  ublas::vector<double> w = 3.0*v;              // multiplication by scalar
  ublas::vector<double> x = w + 2.543*w;        // addition

  double c = ublas::norm_2(x);                  // euclidean norm
  c = ublas::norm_inf(x);                       // max-norm
  c = ublas::inner_prod(x,w);                   // inner product
  for(unsigned int i = 0; i < mat.size1(); i++)
      mat(i,i) = 2.0;

  ublas::vector<double> ax = prod(mat,x);       // matrix/vector product
  ublas::matrix<double> mat2 = prod(mat,mat);    // matrix/matrix product
  x += v*2.0;                            // x <- x + 2*v

  ublas::vector<double> xx = x;                 // copy in memory
  x -= v*2.0;                            // x <- x - 2*v
  v = w + 2.0*v;
}

void lorenz(
	const ublas::c_vector<double , 3U> & x,
	ublas::c_vector<double , 3U> & dxdt,
	double t)
{
	const double sigma = 10.0;
	const double R = 28.0;
	const double b = 8.0 / 3.0;

    dxdt[0] = sigma * ( x[1] - x[0] );
    dxdt[1] = R * x[0] - x[1] - x[0] * x[2];
    dxdt[2] = -b * x[2] + x[0] * x[1];
}

void write_lorenz( const c_vector< double , 3U > &x , const double t )
{
    std::cout << t << '\t' << x[0] << '\t' << x[1] << '\t' << x[2] << std::endl;
}

void odeintDemo()
{
    c_vector< double , 3U > x;
	x[0] = 10.0; x[1] = 1.0; x[2] = 1.0; // initial conditions

    //integrate( lorenz , x , 0.0 , 25.0 , 0.1 , write_lorenz );
	runge_kutta4< c_vector< double , 3U > > stepper; //%an error controlled stepper, and a call to integrate to the current time would be best
	const double dt = 0.01;
	for( double t=0.0 ; t<10.0 ; t+= dt )
	{
		stepper.do_step( lorenz , x , t , dt);
		write_lorenz(x, t); //observer
	}
}