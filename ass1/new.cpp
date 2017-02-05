#include <stdio.h>
#include <iostream>
#include <vector>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/utility.hpp>

using namespace cv; // all the new API is put into "cv" namespace. Export its content
using namespace std;

class Vec{
	double x;
	double y;
	double z;
}

Vec::Vec(double _x, double _y, double _z){
	x = _x;
	y = _y;
	z = _z;
}

double Vec::dotProd(Vec v2){
	return (x*v2.x + y*v2.y + z*v2.z);
}

Vec Vec::crossProd(Vec v2){
	Vec v3 = new Vec();
	v3.x = y*v2.z - z*v2.y;
	v3.y = z*v2.x - x*v2.z;
	v3.z = x*v2.y - y*v2.x;
	return v3;
}

class Ray
{
	Vec r0;
	Vec rd;

}

Ray::Ray(Vec _r0,Vec _rd ){
	r0 = _r0;
	rd = _rd;
}

class Sphere
{
	Vec c;
	int r;
}

Sphere::Sphere(Vec _c,int _r ){
	c = _c;
	r = _r;
}

class Polygon
{
	int n;
	vector<Vec> vertices;
}

Polygon::Polygon(int _c,vector<Vec> ){
	c = _c;
	r = _r;
}

int main{


}