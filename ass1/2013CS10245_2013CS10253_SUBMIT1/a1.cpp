#include "new.cpp"
#include <fstream>
vector<Sphere> spheres;

vector<Polygon> polygons;

vector<nVec> lights_srcs;
vector<double> i_l;
double i_a;
int image_length,image_height;
double pixel_d;
int alias_factor = 3;

//ri-> refreactive index of the medium of the ray
Vec3b traceRay(Ray r, double factor, double ri_in, int rray){
	// cout<<"ri "<<ri_in<<endl;
	if(factor < 0.001) return 0;
	// cout<<"working"<<endl;
	double t=1000000, tempt=-1;
	bool pol_intersect = false;
	bool sphere_intersect = false;
	int int_k = -1, aff=0;
	Mat transm;

	for(int k=0; k<spheres.size(); k++){
		pair<double,double> int1 = spheres[k].intersect(r);
		if(int1.first > 0){
			tempt = min(int1.first,int1.second);
			if(tempt<0 || abs(tempt) < 0.001) t = min(int1.first,t);
			else t = tempt;
			// t = min(min(int1.first,int1.second), t);
			// if(t < 0) t = min(int1.first, );
			if(t == int1.first || t == int1.second){
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
		if(int2 > 0){
			t = min(t,int2);
			if(t == int2){
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
	Vec3b color = Vec3b(0,0,0);
	double i_total = 0;
	if (sphere_intersect || pol_intersect){
		if(pol_intersect){
			i_total += polygons[int_k].ka*i_a;
		}
		else{
			i_total += spheres[int_k].ka*i_a;
		}

		nVec pt = r.Point(t);
		if(aff==1){
			pt = (r.r0.transform(transm.inv(),0) + r.rd.transform(transm.inv(),1).scale(t) ).transform(transm,0);	
		}
		nVec N;
		if(pol_intersect){
			N = polygons[int_k].normal().scale(-1);
		}
		else{
			N = spheres[int_k].normal(pt);
		}
		for(int ll=0; ll<lights_srcs.size(); ll++){
			
			nVec l_src = lights_srcs[ll];
			nVec L = l_src - pt;
			L = L.norm();
			nVec R = N.scale(2*L.dotProd(N)) - L;
			nVec V = r.r0 - pt;
			V = V.norm();
			R = R.norm();
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
			if(!blocked ){
				if(pol_intersect){
					i_total += max(polygons[int_k].kd*i_l[ll]*(L.dotProd(N)),0.0) + polygons[int_k].ks*i_l[ll]*pow(max(0.0,R.dotProd(V)),polygons[int_k].spec_coeff);
				}
				else{
					i_total += max(spheres[int_k].kd*i_l[ll]*(L.dotProd(N)),0.0) + spheres[int_k].ks*i_l[ll]*pow(max(0.0,R.dotProd(V)),spheres[int_k].spec_coeff);
				}
			}
			
		}
		if(pol_intersect){
			nVec ref = N.scale(2*(r.rd).dotProd(N)) - r.rd;
			ref = ref.norm();
			ref = ref.scale(-1);
			Ray reflected_ray(pt, ref);
			Vec3b newColor = traceRay(reflected_ray,factor*polygons[int_k].reflection,1.0,0);
			if(rray==1) cout<<"here"<<endl;
			color[0] = min((newColor[0] + polygons[int_k].absorption*factor*polygons[int_k].color[0])*i_total,255.0);
			color[1] = min((newColor[1] + polygons[int_k].absorption*factor*polygons[int_k].color[1])*i_total,255.0);
			color[2] = min((newColor[2] + polygons[int_k].absorption*factor*polygons[int_k].color[2])*i_total,255.0);
		}
		else{
			nVec ref = N.scale(2*(r.rd).dotProd(N)) - r.rd;
			ref = ref.norm();
			ref = ref.scale(-1);
			Ray reflected_ray(pt, ref);
			double ri_out;
			if(N.dotProd(r.rd)>0){
				ri_out = 1.0;
				N = N.scale(-1);
			}
			else{
				ri_out = spheres[int_k].ref_ind;
			}
			double nr = ri_in/ri_out;
			double c1 = (-1)*r.rd.dotProd(N);
			double c2 = sqrt(1 - nr*nr*(1 - c1*c1));
			nVec rref = r.rd.scale(nr) + N.scale(nr*c1 - c2);
			rref = rref.norm();
			Ray refracted_ray(pt,rref);

			// cout<<"sphere ref ind "<<ri_out<<" refractivity "<<spheres[int_k].refraction<<endl;
			Vec3b newColor = traceRay(reflected_ray,factor*spheres[int_k].reflection,1.0,0);
			Vec3b newColor2 = traceRay(refracted_ray,factor*spheres[int_k].refraction,ri_out,1);
			color[0] = min(newColor[0] + newColor2[0] + factor*spheres[int_k].absorption*spheres[int_k].color[0]*i_total,255.0);
			color[1] = min(newColor[1] + newColor2[1] + factor*spheres[int_k].absorption*spheres[int_k].color[1]*i_total,255.0);
			color[2] = min(newColor[2] + newColor2[2] + factor*spheres[int_k].absorption*spheres[int_k].color[2]*i_total,255.0);
			cout<<newColor2<<" "<<color<<endl;
			
		}

	}
	return color;
}

Mat illuminateModel(nVec camera, Screen s){
	Mat tmp_img(image_height,image_length, CV_8UC3, Vec3b(0,0,0));
	double i=0,j=0;
	for(double it1 = 0; it1 < image_height; it1++){
		for(double it2 = 0; it2 < image_length; it2++){
			i = (it1 - image_height/2.0)*pixel_d;
			j = (it2 - image_length/2.0)*pixel_d;
			nVec pixel = s.center + (s.up.scale(i)) + (s.
					right.scale(j));
			nVec rd = pixel - camera;
			rd = rd.norm();
			Ray r(camera, rd);
			tmp_img.at<Vec3b>(it1,it2) = traceRay(r,1,1.0,0);
		}
	}
	return tmp_img;
}

void antiAlias(nVec camera, Screen s){
	Mat img(image_height, image_length, CV_8UC3, Vec3b(0,0,0));
	image_height = alias_factor*image_height;
	image_length = alias_factor*image_length;
	pixel_d = pixel_d/float(alias_factor);
	Mat tmp_img = illuminateModel(camera,s);
	image_height = image_height/alias_factor;
	image_length = image_length/alias_factor;
	for(int i=0;i<image_height;i++){
		for(int j=0;j<image_length;j++){
			double x_c=0,y_c=0,z_c=0;
			for(int i0=0;i0<alias_factor;i0++){
				for(int j0=0;j0<alias_factor;j0++){
					x_c += tmp_img.at<Vec3b>(i*alias_factor + i0, j*alias_factor + j0)[0];
					y_c += tmp_img.at<Vec3b>(i*alias_factor + i0, j*alias_factor + j0)[1];
					z_c += tmp_img.at<Vec3b>(i*alias_factor + i0, j*alias_factor + j0)[2];
				}
			}
			img.at<Vec3b>(i,j) = Vec3b(x_c/(alias_factor*alias_factor), y_c/(alias_factor*alias_factor), z_c/(alias_factor*alias_factor));
		}
	}
	imshow("image", img);
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
	inp>>tag>>pixel_d;
	inp>>tag>>image_length>>image_height;

	Screen sc(sc_center,sc_normal,sc_up,sc_right);

	int n_spheres;
	inp>>tag>>n_spheres;
	for(int i=0;i<n_spheres;i++){
		inp>>tag>>x>>y>>z;
		nVec sp_center(x,y,z);
		double sp_radius;
		inp>>tag>>sp_radius;
		inp>>tag>>x>>y>>z;
		// Vec3b sp_color = Vec3b(0,0,255);
		Vec3b sp_color = Vec3b(x,y,z);
		double ka,kd,ks,spec_coeff;
		inp>>tag>>ka>>kd>>ks>>spec_coeff;
		double reflection, refraction, absorption, ref_ind;
		inp>>tag>>reflection>>refraction>>absorption>>ref_ind;
		int affine;
		inp>>tag>>affine;
		Mat affine_mat = Mat(4,4, CV_32FC1, float(0));
		if(affine==1){
			for(int i0=0;i0<4;i0++){
				for(int i1=0;i1<4;i1++){
					inp>>affine_mat.at<float>(i0,i1);
				}
			}
		}
		Sphere sp(sp_center,sp_radius,sp_color,ka,kd,ks,spec_coeff,affine,affine_mat,reflection,refraction,absorption,ref_ind);
		spheres.push_back(sp);
	}


	int n_pol;
	inp>>tag>>n_pol;
	for(int i=0;i<n_pol;i++){
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
		double reflection,refraction, absorption;
		inp>>tag>>reflection>>refraction>>absorption;
		int affine;
		inp>>tag>>affine;
		Mat affine_mat = Mat(4,4, CV_32FC1, float(0));
		if(affine==1){
			for(int i0=0;i0<4;i0++){
				for(int i1=0;i1<4;i1++){
					inp>>affine_mat.at<float>(i0,i1);
				}
			}
		}
		Polygon p(n_vertices,vertices,p_color,ka,kd,ks,spec_coeff,affine,affine_mat ,reflection,refraction,absorption);
		polygons.push_back(p);
	}
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
	// illuminateModel(camera,sc);

	// Mat matrix(4,4,CV_32FC1)
	antiAlias(camera,sc);
}