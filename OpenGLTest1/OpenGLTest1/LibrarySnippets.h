//ublas references.
//http://www.boost.org/doc/libs/1_60_0/libs/numeric/ublas/doc/index.html
//http://www.crystalclearsoftware.com/cgi-bin/boost_wiki/wiki.pl?Effective_UBLAS
//http://www.boost.org/doc/libs/1_60_0/libs/numeric/odeint/doc/html/index.html
//http://na-wiki.csc.kth.se/mediawiki/index.php/Using_uBLAS#Expressions_at_work
//http://www.crystalclearsoftware.com/cgi-bin/boost_wiki/wiki.pl?Effective_UBLAS

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>

using namespace boost::numeric;
using namespace boost::numeric::ublas;

void ublasDemo();

#include <boost/numeric/odeint.hpp>

using namespace boost::numeric::odeint;

void lorenz(
	const ublas::c_vector<double , 3U> & x,
	ublas::c_vector<double, 3U> & dxdt,
	double t);

void write_lorenz(
	const ublas::c_vector<double, 3U> & x,
	const double t);

void odeintDemo();
