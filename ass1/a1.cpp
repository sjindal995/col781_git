#include <iostream>

using namespace std;

class vector{
	double x;
	double y;
	double z;
	double dotProd(vector v2);
	vector crossProd(vector v2);
	double getMag();
};

vector::vector(double _x, double _y, double _z){
	x = _x;
	y = _y;
	z = _z;
}

double vector::dotProd(vector v2){
	return (x*v2.x + y*v2.y + z*v2.z);
}

vector vector::crossProd(vector v2){
	vector v3 = new vector();
	v3.x = y*v2.z - z*v2.y;
	v3.y = z*v2.x - x*v2.z;
	v3.z = x*v2.y - y*v2.x;
	return v3;
}