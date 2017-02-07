#include "new.cpp"

vector<Sphere> spheres;

vector<Polygon> polygons;

vector<nVec> lights_srcs;

void antiAlias(Mat* img){

}

void illuminateModel(nVec camera, Screen s,  double i_l, double i_a){
	Mat img(s.l,s.b, CV_8UC3, Vec3b(0,0,0));
	// nVec l_src;
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
					if(t == int2){
						// img.at<Vec3b>(i+s.l/2,j+s.b/2) = polygons[k].color;
						pol_intersect = true;
						int_k = k;
					} 
				}
			}
			// if(!sphere_intersect && !pol_intersect) continue;

			// nVec pt = camera + rd.scale(t);

			double i_total = 0;
			if (sphere_intersect || pol_intersect){

				for(int ll=0; ll<lights_srcs.size(); ll++){
					
					nVec l_src = lights_srcs[ll];
					// l_src.print();
					nVec pt = r.Point(t);
					nVec L = l_src - pt;
					L = L.norm();
					nVec N;
					if(pol_intersect){
						N = polygons[int_k].normal().scale(-1);
						// N.print();
					}
					else{
						N = pt - spheres[int_k].c;
						N = N.norm();
					}
					nVec R = N.scale(2*L.dotProd(N)) - L;
					nVec V = camera - pt;
					V = V.norm();
					bool blocked = false;
					Ray light_ray(pt, L);
					for(int k1=0;k1<spheres.size();k1++){
						pair<double,double> pr = spheres[k1].intersect(light_ray);
						if(pr.first > pow(10,-3)) {
							blocked = true;
							break;
						}
					}
					if(!blocked){
						for(int k1=0;k1<polygons.size();k1++){
							double pr = polygons[k1].intersect(light_ray);
							if(pr > pow(10,-3)) {
								blocked = true;
								break;
							}
						}
					}
					if(pol_intersect){
						i_total += polygons[int_k].ka*i_a;
					}
					else{
						i_total += spheres[int_k].ka*i_a;
					}
					if(!blocked ){
						if(pol_intersect){
							i_total += max(polygons[int_k].kd*i_l*(L.dotProd(N)),0.0) + polygons[int_k].ks*i_l*pow(max(0.0,R.dotProd(V)),polygons[int_k].spec_coeff);
							// cout << "1: " << (L.dotProd(N)) << ", 2: " << R.dotProd(V) << endl;
						}
						else{
							i_total += max(spheres[int_k].kd*i_l*(L.dotProd(N)),0.0) + spheres[int_k].ks*i_l*pow(max(0.0,R.dotProd(V)),spheres[int_k].spec_coeff);
						}
					}

				}

				if(pol_intersect){
					// cout << img.at<Vec3b>(i+s.l/2,j+s.b/2) <<  " , "  << blocked << ", pixel: " << pixel.x << "," << pixel.y << ","<<pixel.z << endl;
					img.at<Vec3b>(i+s.l/2,j+s.b/2)[0] = min(polygons[int_k].color[0]*i_total,255.0);
					img.at<Vec3b>(i+s.l/2,j+s.b/2)[1] = min(polygons[int_k].color[1]*i_total,255.0);
					img.at<Vec3b>(i+s.l/2,j+s.b/2)[2] = min(polygons[int_k].color[2]*i_total,255.0);	
				}
				else{
					img.at<Vec3b>(i+s.l/2,j+s.b/2)[0] = min(spheres[int_k].color[0]*i_total,255.0);
					img.at<Vec3b>(i+s.l/2,j+s.b/2)[1] = min(spheres[int_k].color[1]*i_total,255.0);
					img.at<Vec3b>(i+s.l/2,j+s.b/2)[2] = min(spheres[int_k].color[2]*i_total,255.0);
				}
				
			}

		}
	}
	GaussianBlur(img,img,Size(5,5),0.5);
	imshow("image", img);
	waitKey(0);
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
					if(t == int2){
						// cout << t << endl;
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
	nVec camera(0,0,-160);
	nVec normal(0,0,1);
	nVec up(0,1,0);
	nVec right(1,0,0);
	nVec center(0,0,40);
	nVec center1(0,0,-40);
	Sphere sp(center, 100, Vec3b(0,0,255));
	spheres.push_back(sp);

	// p.n=3;
	vector<nVec> nv;
	nv.push_back(nVec(-300,-300,150));
	nv.push_back(nVec(300,-300,150));
	nv.push_back(nVec(300,300,150));
	nv.push_back(nVec(-300,300,150));
	// p.vertices = nv;
	// p.color = Vec3b(0,255,0);
	Polygon p(3,nv,Vec3b(200,0,0));

	polygons.push_back(p);
	p.normal().print();


	Screen s(800,800,center1, normal, up, right);
	// generateImage(camera, s);
	nVec l_src(300,200,-200);
	nVec l_src2(-300,0,-200);
	// nVec l_src(250,0,0);
	lights_srcs.push_back(l_src);
	// lights_srcs.push_back(l_src2);

	double i_l = 1;
	double i_a = 1;

	illuminateModel(camera,s,  i_l, i_a);
}