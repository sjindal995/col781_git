#include "new.cpp"

bool getIntersection(Ray r, Sphere sp){
	// double A = Xd^2 + Yd^2 + Zd^2;
	double B = 2*(r.rd.x*(r.r0.x - sp.c.x) + r.rd.y*(r.r0.y - sp.c.y) + r.rd.z*(r.r0.z - sp.c.z));
	double C = pow((r.r0.x - sp.c.x),2) + pow((r.r0.y - sp.c.y),2) + pow((r.r0.z - sp.c.z),2) - pow(sp.r,2);
	double t0 = (-1*B - sqrt(B*B - 4*C))/2.0;
	double t1 = (-1*B + sqrt(B*B - 4*C))/2.0;
	// cout << "t0: " << t0  << " , t1: " << t1 << ", C: " << C <<endl;
	if(max(t0,t1) >= 0) return true;
	return false;
}

void generateImage(nVec camera, Screen s, Sphere sp){
	Mat img(s.l, s.b, CV_8UC3,Vec3b(0,0,0));
	for(int i=-1*s.l/2; i < s.l/2;i++){
		for(int j = -1*s.b/2; j < s.b/2;j++){
			nVec pixel = s.center.add(s.up.multiply(i)).add(s.right.multiply(j));
			nVec rd = pixel.sub(camera);
			rd = rd.divide(rd.getMag());
			Ray r(camera, rd);
			if(getIntersection(r,sp)){
				// cout << i << " , " << j << endl;
				img.at<Vec3b>(i+s.l/2,j+s.b/2) = Vec3b(255,255,255);
			}
		}
	}
	imshow("img", img);
	waitKey(0);
}

int main(){
	nVec camera(0,0,125);
	nVec normal(0,0,1);
	nVec up(0,1,0);
	nVec right(1,0,0);
	nVec center(0,0,-425);
	nVec center1(0,0,0);
	Sphere sp(center, 500);
	Screen s(500,1000,center1, normal, up, right);
	generateImage(camera, s, sp);
}