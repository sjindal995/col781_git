#include <stdio.h>
#include <iostream>
#include <vector>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/utility.hpp>

using namespace cv; // all the new API is put into "cv" namespace. Export its content
using namespace std;

class nVec{
public:
	double x;
	double y;
	double z;
	nVec();
	nVec(double _x,double _y, double _z);
	double dotProd(nVec v2);
	nVec crossProd(nVec v2);
};

nVec::nVec(){
	x=0;
	y=0;
	z=0;
}

nVec::nVec(double _x, double _y, double _z){
	x = _x;
	y = _y;
	z = _z;
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

class Ray
{
public:
	nVec r0;
	nVec rd;
	Ray(nVec _r0, nVec _rd);
};

Ray::Ray(nVec _r0,nVec _rd ){
	r0 = _r0;
	rd = _rd;
}

class Sphere
{
public:
	nVec c;
	int r;
	Sphere(nVec _c, int _r);
};

Sphere::Sphere(nVec _c,int _r ){
	c = _c;
	r = _r;
}

class Polygon
{
public:
	int n;
	vector<nVec> vertices;
	Polygon(int _n, vector<nVec> _vertices);
};

Polygon::Polygon(int _n,vector<nVec> _vertices){
	n = _n;
	vertices = _vertices;
}

class Screen{
public:
	int l;
	int b;
	int center;
	nVec normal;
	Screen(int _l, int _b, int _center, nVec _normal);
}

Screen::Screen(int _l, int _b, int _center, nVec _normal){
	l = _l;
	b = _b;
	center = _center;
	normal = _normal;
}

// int main(){


// }