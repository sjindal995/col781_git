#include "new.cpp"
#include <fstream>
vector<Sphere> spheres;

vector<Polygon> polygons;

vector<nVec> lights_srcs;
vector<double> i_l;
double i_a;
int image_length,image_height;

void illuminateModel(nVec camera, Screen s){
	Mat img(s.l,s.b, CV_8UC3, Vec3b(0,0,0));
	// nVec l_src;
	for(int i = -1*s.l/2; i <= s.l/2; i+= s.l/image_height){
		for(int j = -1*s.b/2; j <= s.b/2; j+= s.b/image_length){
			nVec pixel = s.center + (s.up.scale(i)) + (s.
					right.scale(j));
			nVec rd = pixel - camera;
			rd = rd.norm();
			Ray r(camera, rd);
			double t=1000000;
			bool pol_intersect = false;
			bool sphere_intersect = false;
			int int_k = -1, aff=0;
			Mat transm;

			for(int k=0; k<spheres.size(); k++){
				// if(sphere.affine==0){
				pair<double,double> int1 = spheres[k].intersect(r);
				// }
				// else{
				// 	pair<double,double> int1 = spheres[k].intersect()
				// }
				if(int1.first !=-1){
					t = min(min(int1.first,int1.second), t);
					if(t == min(int1.first, int1.second)){
						// img.at<Vec3b>(i+s.l/2,j+s.b/2) = spheres[k].color;
						if(spheres[k].affine==1) {
							aff=1;
							transm = spheres[k].m;
						}
						else aff=0;
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
						if(polygons[k].affine==1) {
							aff=1;
							transm = polygons[k].m;
						}
						else aff=0;
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
					if(aff==1){
						pt = (r.r0.transform(transm.inv()) + r.rd.transform(transm.inv()).scale(t) ).transform(transm);	
					}
					nVec L = l_src - pt;
					L = L.norm();
					nVec N;
					if(pol_intersect){
						N = polygons[int_k].normal().scale(-1);
						// N.print();
					}
					else{
						// N = pt - spheres[int_k].c;
						// N = N.norm();
						N = spheres[int_k].normal(pt);
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
							i_total += max(polygons[int_k].kd*i_l[ll]*(L.dotProd(N)),0.0) + polygons[int_k].ks*i_l[ll]*pow(max(0.0,R.dotProd(V)),polygons[int_k].spec_coeff);
							// cout << "1: " << (L.dotProd(N)) << ", 2: " << R.dotProd(V) << endl;
						}
						else{
							i_total += max(spheres[int_k].kd*i_l[ll]*(L.dotProd(N)),0.0) + spheres[int_k].ks*i_l[ll]*pow(max(0.0,R.dotProd(V)),spheres[int_k].spec_coeff);
						}
					}

				}

				if(pol_intersect){
					// cout << img.at<Vec3b>(i+s.l/2,j+s.b/2) <<  " , "  << blocked << ", pixel: " << pixel.x << "," << pixel.y << ","<<pixel.z << endl;
					for(int i1 = 0;i1 < s.l/image_height;i1++){
						for(int j1=0;j1 < s.b/image_length;j1++){
							img.at<Vec3b>(i+i1+s.l/2,j+j1+s.b/2)[0] = min(polygons[int_k].color[0]*i_total,255.0);
							img.at<Vec3b>(i+i1+s.l/2,j+j1+s.b/2)[1] = min(polygons[int_k].color[1]*i_total,255.0);
							img.at<Vec3b>(i+i1+s.l/2,j+j1+s.b/2)[2] = min(polygons[int_k].color[2]*i_total,255.0);
						}
					}
				}
				else{
					for(int i1 = 0;i1 < s.l/image_height;i1++){
						for(int j1=0;j1 < s.b/image_length;j1++){
							img.at<Vec3b>(i+i1+s.l/2,j+j1+s.b/2)[0] = min(spheres[int_k].color[0]*i_total,255.0);
							img.at<Vec3b>(i+i1+s.l/2,j+j1+s.b/2)[1] = min(spheres[int_k].color[1]*i_total,255.0);
							img.at<Vec3b>(i+i1+s.l/2,j+j1+s.b/2)[2] = min(spheres[int_k].color[2]*i_total,255.0);
						}
					}
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

int main(int argc, char** argv){
	ifstream inp(argv[1]);
	// ifstream inp("input.txt");
	string tag;
	inp >> tag;
	double x,y,z;
	inp>>x>>y>>z;
	nVec camera(x,y,z);

	inp>>tag>>x>>y>>z;
	nVec sc_center(x,y,z);
	inp>>tag>>x>>y>>z;
	nVec sc_normal(x,y,z);
	inp>>tag>>x>>y>>z;
	nVec sc_up(x,y,z);
	inp>>tag>>x>>y>>z;
	nVec sc_right(x,y,z);
	inp>>tag>>x;
	double sc_length = x;
	inp>>tag>>x;
	double sc_height = x;

	Screen sc(sc_height,sc_length,sc_center,sc_normal,sc_up,sc_right);

	int n_spheres;
	inp>>tag>>n_spheres;
	for(int i=0;i<n_spheres;i++){
		inp>>tag>>x>>y>>z;
		nVec sp_center(x,y,z);
		double sp_radius;
		inp>>tag>>sp_radius;
		inp>>tag>>x>>y>>z;
		Vec3b sp_color = Vec3b(0,0,255);
		double ka,kd,ks,spec_coeff;
		inp>>tag>>ka>>kd>>ks>>spec_coeff;
		Sphere sp(sp_center,sp_radius,sp_color,ka,kd,ks,spec_coeff);
		spheres.push_back(sp);
	}


	int n_pol;
	inp>>tag>>n_pol;
	for(int i=0;i<n_spheres;i++){
		int n_vertices;
		inp>>tag>>n_vertices;
		vector<nVec> vertices;
		for(int j=0;j<n_vertices;j++){
			inp>>tag>>x>>y>>z;
			vertices.push_back(nVec(x,y,z));
		}
		inp>>tag>>x>>y>>z;
		Vec3b p_color = Vec3b(x,y,z);
		double ka,kd,ks,spec_coeff;
		inp>>tag>>ka>>kd>>ks>>spec_coeff;
		Polygon p(n_vertices,vertices,p_color,ka,kd,ks,spec_coeff);
		// polygons.push_back(p);
	}

	spheres[0].affine=1;
	Mat mat(4,4, CV_32FC1, float(0));
	mat.at<float>(0,0)=1;
	mat.at<float>(1,0)=1;
	mat.at<float>(1,1)=2;
	mat.at<float>(2,2)=3;
	mat.at<float>(3,3)=1;
	spheres[0].m = mat;

	int n_lights;
	inp>>tag>>n_lights;
	for(int i=0;i<n_lights;i++){
		inp>>tag>>x>>y>>z;
		lights_srcs.push_back(nVec(x,y,z));
		double l;
		inp>>tag>>l;
		i_l.push_back(l);
	}
	inp>>tag>>i_a;
	inp>>tag>>image_length>>image_height;
	illuminateModel(camera,sc);
	// Mat matrix(4,4,CV_32FC1)
}