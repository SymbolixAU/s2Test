
#include <Rcpp.h>
using namespace Rcpp;

#include "s2/s2point.h"
#include "s2/s2pointutil.h"

// [[Rcpp::export]]
List rcpp_hello_world() {

    CharacterVector x = CharacterVector::create( "foo", "bar" )  ;
    NumericVector y   = NumericVector::create( 0.0, 1.0 ) ;
    List z            = List::create( x, y ) ;

    return z ;
}


// [[Rcpp::export]]
void pointTest() {
	
	S2Point a(1, 1, 1);
	S2Point b(1, 2, 1);
	S2Point c(1, 1, 2);
	
	bool test;
	
	test = S2::SimpleCCW(a, b, c);
	
	Rcpp::Rcout << "test: " << test << std::endl;
	
}


