#include "new.cpp"

vector<Sphere> spheres;

vector<Polygon> polygons;

void illuminateModel(nVec camera, Screen s, nVec l_src, double i_l, double i_a){
	Mat img(s.l,s.b, CV_8UC3, Vec3b(0,0,0));
	for(int i = -1*s.l/2; i <= s.l/2; i++){
		for(int j = -1*s.b/2; j <= s.b/2; j++){
			nVec pixel = s.center + (s.up.scale(i)) + (s.
					right.scale(j));
			nVec rd = pixel - camera;
			rd = rd.norm();
			Ray r(camera, rd);
			double t=1000000;
			bool pol_intersect = false;
			bool sphere_intersect = false;
			int int_k = -1;

			for(int k=0; k<spheres.size(); k++){
				pair<double,double> int1 = spheres[k].intersect(r);
				if(int1.first !=-1){
					t = min(min(int1.first,int1.second), t);
					if(t == min(int1.first, int1.second)){
						// img.at<Vec3b>(i+s.l/2,j+s.b/2) = spheres[k].color;
						sphere_intersect = true;
						int_k = k;
					}

				}
			}
			for(int k=0;k<polygons.size(); k++){
				double int2 = polygons[k].intersect(r);
				if(int2 !=-1){
					t = min(t,int2);
					if(t = int2){
						// img.at<Vec3b>(i+s.l/2,j+s.b/2) = polygons[k].color;
						pol_intersect = true;
						int_k = k;
					} 
				}
			}
			if(!sphere_intersect && !pol_intersect) continue;

			nVec pt = camera + rd.scale(t);
			nVec L = l_src - pt;
			L = L.norm();
			nVec N;
			if(sphere_intersect && !pol_intersect){
				N = pt - spheres[int_k].c;
				N = N.norm();
			}
			else{
				N = polygons[int_k].normal();
			}
			nVec R = N.scale(2*L.dotProd(N)) - L;
			nVec V = camera - pt;
			V = V.norm();
			bool blocked = false;
			Ray light_ray(pt, L);
			for(int k1=0;k1<spheres.size();k1++){
				if(sphere_intersect && !pol_intersect && (k1 == int_k)){
					pair<double,double> pr = spheres[k1].intersect(light_ray);
					if(pr.first > 0 || pr.second > 0) {
						blocked = true;
						break;
					}
				}
				else if((k1 != int_k) || pol_intersect){
					pair<double,double> pr = spheres[k1].intersect(light_ray);
					if(pr.first >= 0 || pr.second >= 0) {
						blocked = true;
						break;
					}
				}
			}
			if(!blocked){
				for(int k1=0;k1<polygons.size();k1++){
					if(pol_intersect && (k1 == int_k)){
						pair<double,double> pr = polygons[k1].intersect(light_ray);
						if(pr.first > 0 || pr.second > 0) {
							blocked = true;
							break;
						}
					}
					else if((k1 != int_k) || (sphere_intersect && !pol_intersect)){
						pair<double,double> pr = polygons[k1].intersect(light_ray);
						if(pr.first >= 0 || pr.second >= 0) {
							blocked = true;
							break;
						}
					}
				}
			}
			double i_total = 0;
			if(sphere_intersect && !pol_intersect){
				i_total = spheres[int_k].ka*i_a;
			}
			else{
				i_total = polygons[int_k].ka*i_a;
			}
			if(!blocked){
				if(sphere_intersect && !pol_intersect){
					i_total += spheres[int_k].kd*i_l*L.dotProd(N) + spheres[int_k].ks*i_l*pow(R.dotProd(N),spheres[int_k].spec_coeff);
				}
				else{
					i_total += polygons[int_k].kd*i_l*L.dotProd(N) + polygons[int_k].ks*i_l*pow(R.dotProd(N),polygons[int_k].spec_coeff);
				}
			}
			if(sphere_intersect && !pol_intersect){
				img.at<Vec3b>(i+s.l/2,j+s.b/2)[0] = max(spheres[int_k].color[0]*i_total,255);
				img.at<Vec3b>(i+s.l/2,j+s.b/2)[1] = max(spheres[int_k].color[1]*i_total,255);
				img.at<Vec3b>(i+s.l/2,j+s.b/2)[2] = max(spheres[int_k].color[2]*i_total,255);
			}
			else{
				img.at<Vec3b>(i+s.l/2,j+s.b/2)[0] = max(polygons[int_k].color[0]*i_total,255);
				img.at<Vec3b>(i+s.l/2,j+s.b/2)[1] = max(polygons[int_k].color[1]*i_total,255);
				img.at<Vec3b>(i+s.l/2,j+s.b/2)[2] = max(polygons[int_k].color[2]*i_total,255);	
			}
		}
	}
}

void generateImage(nVec camera, Screen s){
	Mat img(s.l, s.b, CV_8UC3,Vec3b(0,0,0));
	for(int i=-1*s.l/2; i <= s.l/2;i++){
		for(int j = -1*s.b/2; j <= s.b/2;j++){
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
	// generateImage(camera, s);

	illuminateModel(camera,s, l_src, i_l, i_a);
}