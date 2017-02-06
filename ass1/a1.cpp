#include "new.cpp"

vector<Sphere> spheres;

vector<Polygon> polygons;

// bool getIntersection(Ray r, Sphere sp){
// 	// double A = Xd^2 + Yd^2 + Zd^2;
// 	double B = 2*(r.rd.x*(r.r0.x - sp.c.x) + r.rd.y*(r.r0.y - sp.c.y) + r.rd.z*(r.r0.z - sp.c.z));
// 	double C = pow((r.r0.x - sp.c.x),2) + pow((r.r0.y - sp.c.y),2) + pow((r.r0.z - sp.c.z),2) - pow(sp.r,2);
// 	double t0 = (-1*B - sqrt(B*B - 4*C))/2.0;
// 	double t1 = (-1*B + sqrt(B*B - 4*C))/2.0;
// 	// cout << "t0: " << t0  << " , t1: " << t1 << ", C: " << C <<endl;
// 	if(max(t0,t1) >= 0) return true;
// 	return false;
// }

void generateImage(nVec camera, Screen s){
	Mat img(s.l, s.b, CV_8UC3,Vec3b(0,0,0));
	for(int i=-1*s.l/2; i < s.l/2;i++){
		for(int j = -1*s.b/2; j < s.b/2;j++){
			//traverse over all objects, and take the first	
			nVec pixel = s.center + (s.up.scale(i)) + (s.
					right.scale(j));
			nVec rd = pixel - camera;
			rd = rd.norm();
			Ray r(camera, rd);
			double t=1000000;

			for(int k=0; k<spheres.size(); k++){
				pair<double,double> int1 = spheres[k].intersect(r);
				if(int1.first !=-1){
					t = min(min(int1.first,int1.second), t);
					if(t == min(int1.first, int1.second)){
						img.at<Vec3b>(i+s.l/2,j+s.b/2) = spheres[k].color;
						
					}

				}

			}

			for(int k=0;k<polygons.size(); k++){
				double int2 = polygons[k].intersect(r);
				if(int2 !=-1){
					t = min(t,int2);
					if(t = int2){
						img.at<Vec3b>(i+s.l/2,j+s.b/2) = polygons[k].color;

					} 
				}
			}
			// if(t!=1000000 ){

			// 	img.at<Vec3b>(i+s.l/2,j+s.b/2) = Vec3b(255,255,255);
			// }

		}
	}
	imshow("img", img);
	waitKey(0);
	
}



// void generateImage(nVec camera, Screen s, Sphere sp){
// 	Mat img(s.l, s.b, CV_8UC3,Vec3b(0,0,0));
// 	for(int i=-1*s.l/2; i < s.l/2;i++){
// 		for(int j = -1*s.b/2; j < s.b/2;j++){
// 			nVec pixel = s.center + (s.up.scale(i)) + (s.right.scale(j));
// 			// nVec pixel = s.center.add(s.up.multiply(i)).add(s.right.multiply(j));
// 			nVec rd = pixel - camera;
// 			// nVec rd = pixel.sub(camera);
// 			rd = rd.norm();
// 			// rd = rd.divide(rd.getMag());
// 			Ray r(camera, rd);
// 			pair<int,int> intersection = sp.intersect(r);
// 			if(intersection.first == -1 ){

// 				img.at<Vec3b>(i+s.l/2,j+s.b/2) = Vec3b(255,255,255);
// 			}
// 			// if(getIntersection(r,sp)){
// 			// 	// cout << i << " , " << j << endl;
// 			// 	img.at<Vec3b>(i+s.l/2,j+s.b/2) = Vec3b(255,255,255);
// 			// }
// 		}
// 	}
// 	imshow("img", img);
// 	waitKey(0);
// }

int main(){
	nVec camera(0,0,125);
	nVec normal(0,0,1);
	nVec up(0,1,0);
	nVec right(1,0,0);
	nVec center(100,300,-425);
	nVec center1(0,0,0);
	Sphere sp(center, 100, Vec3b(255,0,0));
	spheres.push_back(sp);

	Polygon p;
	p.n=3;
	vector<nVec> nv;
	nv.push_back(nVec(50,10,-100));
	nv.push_back(nVec(10,50,-100));
	nv.push_back(nVec(0,0,-100));
	p.vertices = nv;
	p.color = Vec3b(0,255,0);
	polygons.push_back(p);
	p.normal().print();


	Screen s(500,1000,center1, normal, up, right);
	generateImage(camera, s);
}