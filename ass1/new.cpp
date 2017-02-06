#include <stdio.h>
#include <iostream>
#include <vector>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/utility.hpp>
#include <math.h>
#include <algorithm>

using namespace cv; // all the new API is put into "cv" namespace. Export its content
using namespace std;

class nVec{
public:
	double x;
	double y;
	double z;
	nVec(double _x = 0,double _y = 0, double _z = 0);
	double dotProd(nVec v2);
	nVec crossProd(nVec v2);
	nVec sub(nVec v2);
	nVec add(nVec v2);
	nVec multiply(double d);
	nVec divide(double d);
	double getMag();
};

nVec::nVec(double _x, double _y, double _z){
	x = _x;
	y = _y;
	z = _z;
}

nVec nVec::sub(nVec v2){
	nVec v3;
	v3.x = x - v2.x;
	v3.y = y - v2.y;
	v3.z = z - v2.z;
	return v3;
}

nVec nVec::add(nVec v2){
	nVec v3;
	v3.x = x + v2.x;
	v3.y = y + v2.y;
	v3.z = z + v2.z;
	return v3;
}

nVec nVec::multiply(double d){
	nVec v3;
	v3.x = x*d;
	v3.y = y*d;
	v3.z = z*d;
	return v3;
}

nVec nVec::divide(double d){
	nVec v3;
	v3.x = x/float(d);
	v3.y = y/float(d);
	v3.z = z/float(d);
	return v3;
}


double nVec::dotProd(nVec v2){
	return (x*v2.x + y*v2.y + z*v2.z);
}

nVec nVec::crossProd(nVec v2){
	nVec v3;
	v3.x = y*v2.z - z*v2.y;
	v3.y = z*v2.x - x*v2.z;
	v3.z = x*v2.y - y*v2.x;
	return v3;
}

double nVec::getMag(){
	return sqrt(x*x + y*y + z*z);
}

class Ray
{
public:
	nVec r0;
	nVec rd;
	Ray();
	Ray(nVec _r0, nVec _rd);
};

Ray::Ray(){

}

Ray::Ray(nVec _r0,nVec _rd ){
	r0 = _r0;
	rd = _rd;
}

class Sphere
{
public:
	nVec c;
	int r;
	Sphere();
	Sphere(nVec _c, int _r = 0);
};

Sphere::Sphere(){}

Sphere::Sphere(nVec _c,int _r ){
	c = _c;
	r = _r;
}

class Polygon
{
public:
	int n;
	vector<nVec> vertices;
	Polygon();
	Polygon(int _n, vector<nVec> _vertices);
};

Polygon::Polygon(){}

Polygon::Polygon(int _n,vector<nVec> _vertices){
	n = _n;
	vertices = _vertices;
}

class Screen{
public:
	int l;
	int b;
	nVec center;
	nVec normal;
	nVec up;
	nVec right;
	Screen();
	Screen(int _l, int _b, nVec _center, nVec _normal, nVec _up, nVec _right);
};

Screen::Screen(){}

Screen::Screen(int _l, int _b, nVec _center, nVec _normal, nVec _up, nVec _right){
	l = _l;
	b = _b;
	center = _center;
	normal = _normal;
	up = _up;
	right = _right;
}

// int main(){

// }