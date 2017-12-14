
#include <Rcpp.h>
using namespace Rcpp;

#include "s2/s2point.h"
#include "s2/s2pointutil.h"
#include "s2/s2latlng.h"

// [[Rcpp::export]]
List rcpp_hello_world() {

    CharacterVector x = CharacterVector::create( "foo", "bar" )  ;
    NumericVector y   = NumericVector::create( 0.0, 1.0 ) ;
    List z            = List::create( x, y ) ;

    return z ;
}


// [[Rcpp::export]]
void pointTest() {
	
	S2Point a(-1, 1, 1);
	S2Point b(1, 2, 1);
	S2Point c(1, 1, 2);
	
	bool test;
	
	test = S2::SimpleCCW(a, b, c);
	
	Rcpp::Rcout << "test: " << test << std::endl;
	
	Rcpp::Rcout << a.Abs() << std::endl;
	
}

// [[Rcpp::export]]
void angleTest() {
	
	S2Point a(-1, 1, 1);
	S2Point b(1, 2, 1);
	
	S1Angle ang(a, b);
	
	Rcpp::Rcout << ang << std::endl;
	
}

// [[Rcpp::export]]
void latLonTest() {
	
	Rcpp::Rcout << "S2PointObject Object: " << std::endl;
	S1Angle lat;
	S1Angle lng;
	S2LatLng ll = S2LatLng(lat, lng);
	
	std::vector<S2Point> pt(1);
	
	pt[0] = S2Point(0.0, 0.0, 0.0);
	Rcpp::Rcout << pt[0] << std::endl;
	
	std::string str;
	//	Rcpp::String str;
	S2LatLng ll2;
	
//	ll2 = S2LatLng::FromDegrees(-30.043800, -51.140220);
	ll2.FromDegrees(-30.0, -51.14);
	
	ll2.ToStringInDegrees(&str);
	
	//ll2.GetDistance(ll);
	
	//ll2.ToPoint();
	
}



