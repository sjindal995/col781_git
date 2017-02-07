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
	nVec operator+(const nVec& nv2);
	nVec operator-(const nVec& nv2);
	nVec norm();
	nVec scale(double d);
	nVec transform(Mat);
	void print();

	// nVec sub(nVec v2);
	// nVec add(nVec v2);
	// nVec multiply(double d);
	// nVec divide(double d);
	double getMag();
};

nVec::nVec(double _x, double _y, double _z){
	x = _x;
	y = _y;
	z = _z;
}

nVec nVec::scale(double d){
	nVec v;
	v.x = x*d;
	v.y = y*d;
	v.z = z*d;
	return v;
}

nVec nVec::norm(){
	double d = sqrt(x*x+y*y+z*z);
	return scale(1/d);
}

void nVec::print(){
	cout<<x<<" "<<y<<" "<<z<<endl;
}


nVec nVec::operator+(const nVec& nv) {

	nVec nv2;
	nv2.x = this->x+nv.x;
	nv2.y = this->y+nv.y;
	nv2.z = this->z+nv.z;

	return nv2;
}

nVec nVec::operator-(const nVec& nv) {

	nVec nv2;
	nv2.x = this->x-nv.x;
	nv2.y = this->y-nv.y;
	nv2.z = this->z-nv.z;

	return nv2;
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

nVec nVec::transform(Mat mat){
	nVec v;
	v.x = mat.at<float>(0,0)*x + mat.at<float>(1,0)*y + mat.at<float>(2,0)*z + mat.at<float>(3,0); 
	v.y = mat.at<float>(0,1)*x + mat.at<float>(1,1)*y + mat.at<float>(2,1)*z + mat.at<float>(3,1); 
	v.z = mat.at<float>(0,2)*x + mat.at<float>(1,2)*y + mat.at<float>(2,2)*z + mat.at<float>(3,2);
	return v; 
}

class Ray
{
public:
	nVec r0;
	nVec rd;
	Ray();
	Ray(nVec _r0, nVec _rd);
	nVec Point(double t);	
};

Ray::Ray(){

}

Ray::Ray(nVec _r0,nVec _rd ){
	r0 = _r0;
	rd = _rd;
}

nVec Ray::Point(double t){
	nVec v = rd.scale(t);
	return v+r0;
}

class Sphere
{
public:
	nVec c;
	int r;
	double ka;
	double kd;
	double ks;
	double spec_coeff;
	Vec3b color;
	int affine;
	Mat m;
	// Sphere();
	Sphere(nVec _c, int _r = 0, Vec3b _color = Vec3b(255,255,255), double _ka = 0.3, double _kd = 0.3, double _ks = 0.3, double _spec_coeff = 2, int _affine =0, Mat _m= Mat(4,4, CV_32FC1, float(0)));
	pair <double,double> intersect(Ray R);
	nVec normal(nVec);

};

// Sphere::Sphere(){}

Sphere::Sphere(nVec _c,int _r , Vec3b _color, double _ka, double _kd, double _ks, double _spec_coeff , int _affine, Mat _m){
	c = _c;
	r = _r;
	ka = _ka;
	kd = _kd;
	ks = _ks;
	spec_coeff = _spec_coeff;
	color = _color;
	affine = _affine;
	m = _m;

}

pair <double,double> Sphere::intersect(Ray R){
	if(affine==1){
		R.r0 = R.r0.transform(m.inv());
		R.rd = R.rd.transform(m.inv());
	}
	double A = 1;
	double B = 2*(R.rd.x*(R.r0.x-c.x) + R.rd.y*(R.r0.y-c.y) + R.rd.z*(R.r0.z-c.z) );
	double C = (R.r0.x - c.x)*(R.r0.x - c.x) + (R.r0.y - c.y)*(R.r0.y - c.y) + (R.r0.z - c.z)*(R.r0.z - c.z) - r*r;

	double D = B*B -4*A*C; 
	if(D<0){
		return make_pair(-1,-1);
	}

	double t1 = ((-1)*B + sqrt(D))/(2*A);
	double t2 = ((-1)*B - sqrt(D))/(2*A);
	return make_pair(t1,t2);
}

nVec Sphere::normal(nVec point){
	nVec n = point - c;
	if(affine==0) return n.norm();
	else{
		n = n.transform(m.inv().t());
		return n.norm();
	}
}

class Polygon
{
public:
	int n;
	vector<nVec> vertices;
	Vec3b color;
	double ka;
	double kd;
	double ks;
	double spec_coeff;
	int affine;
	Mat m;
	// Polygon();
	Polygon(int _n, vector<nVec> _vertices, Vec3b, double _ka, double _kd, double _ks, double _spec_coeff, int,Mat);
	nVec normal();
	double intersect(Ray);

};

// Polygon::Polygon(){}

Polygon::Polygon(int _n,vector<nVec> _vertices,Vec3b _color = Vec3b(255,255,255), double _ka = 0.3, double _kd = 0.3, double _ks = 0.3, double _spec_coeff = 2,  int _affine =0, Mat _m= Mat(4,4, CV_32FC1, float(0))){
	n = _n;
	vertices = _vertices;
	color = _color;
	ka = _ka;
	kd = _kd;
	ks = _ks;
	spec_coeff = _spec_coeff;
	affine = _affine;
	m =_m;
}

nVec Polygon::normal(){
	nVec v1 = vertices[1]-vertices[0];
	nVec v2 = vertices[2]-vertices[1];
	nVec v3 = v1.crossProd(v2);
	if(affine==0)	return (v3.norm());
	else {
		return v3.transform(m.inv().t()).norm();
	}
}

double Polygon::intersect(Ray R){
	if(affine==1){
		R.r0 = R.r0.transform(m.inv());
		R.rd = R.rd.transform(m.inv());
	}
	nVec n = normal();
	if(n.dotProd(R.rd)==0) return -1; //parallel case
	double D = (-1)*n.dotProd(vertices[0]);
	double t = (-1)*(n.dotProd(R.r0)+D)/(n.dotProd(R.rd));
	nVec p = R.Point(t);
	// cout<<"Point ";
	// p.print();
	//check if inside
	Ray R1;	//ray passing thru intersection point, in the plane
	R1.r0 = p;
	R1.rd = (vertices[1]-vertices[0]);	//edge X normal to plane
	// R1.rd.print();
	nVec r1norm = R1.rd.crossProd(n); 
	int nint = 0, n0=0;

	for(int i=0; i< vertices.size(); i++){

		Ray R2;
		R2.r0 = vertices[i];
		R2.rd = vertices[(i+1)%(vertices.size())]-vertices[i];
		// nVec raynorm = n.crossProd(r.rd);
		// if(R1.rd.dotProd(raynorm)>0){
		// 	double trls = ((R1.r0-R2.r0).dotProd(raynorm))/ (R1.rd.dotProd(raynorm));
		// 	nVec pt = R1.point(trls); // intersection pt
		// 	if( (R2.rd).dotProd(pt - R2.r0) < (R2.rd).dotProd(R2.rd) )  
		// }

		double trls = (-1)*((R2.r0 - p).dotProd(r1norm))/(R2.rd).dotProd(r1norm);
		nVec p2 = R2.Point(trls);
		double t2 = (p2.x-R1.r0.x)/(R1.r0.x-R1.rd.x);
		// R2.Point(trls).print();
		// cout<<"ind "<<i<<" "<<trls<<" "<<t2<<endl; 
		if(t2>0){

			if(trls >0.0001 && trls < 0.9999 ){
				nint++;
			}
			if(abs(trls)<0.0001 && abs(trls-1)<0.0001 ){
				n0++;
				cout<<"here"<<endl;
			}
		}


	}
	nint+=n0/2;
	if(nint % 2 ==1){
		return t;
	}

	return -1;

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
// 	// nVec v()
// 	cout<<0<<endl;
// 	nVec v(1,2,3);
// 	Mat matrix(4,4,CV_32FC1);
// 	int k=0;
// 	for(int i=0; i<4; i++){
// 		for(int j=0; j<4; j++){
// 			if(i==j)
// 			matrix.at<float>(i,j) = 1;
// 			else matrix.at<float>(i,j)=0;
// 		}

// 	}
// 	matrix.at<float>(1,1) =2; 
// 	matrix.at<float>(2,2) =3; 
// 	v.transform(matrix).print();
// }