#include <iostream>
#include <algorithm>
#include <cmath>
#include <vector>
#include <stdio.h>
#include <fstream>
using namespace std;

//#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/highgui/highgui.hpp"
#include <opencv2/opencv.hpp>


using namespace cv;


double max(double a, double b)
{
	return a>b?a:b;
}

double min(double a, double b)
{
	return a<b?a:b;
}


Mat affine_global,inverse_global,tpose_global;
// Vector class to define rays. Basic computation functions for that.
class Vector{
	public:
		double x;
		double y;
		double z;
		int type;	// 0 for point, 1 for direction, -1 for None
		// Vector();
		Vector(double, double, double, int);
		double operator^(Vector);
		Vector operator&(Vector);
		Vector operator*(double);
		Vector operator+(Vector);
		Vector operator-(Vector);
		double mag(bool);
		Vector unit();
		void print();
		Vector affineTransform(Mat,int);
};

Vector::Vector(double x_set = 0, double y_set = 0, double z_set = 0, int type_set = 1)
{
	x = x_set;
	y = y_set;
	z = z_set;
	type = type_set;
}

// Dot product
double Vector::operator^(Vector b)
{
	if(type != 1 || b.type != 1) {}//printf("Point provided rather than direction in dot product, check type\n");
	return (x*b.x + y*b.y + z*b.z);
}

// Cross product
Vector Vector::operator&(Vector b)
{
	if(type != 1 || b.type != 1) printf("Point provided rather than direction in cross product, check type\n");
	Vector a_cross_b;
	a_cross_b.x = y*b.z - z*b.y;
	a_cross_b.y = z*b.x - x*b.z;
	a_cross_b.z = x*b.y - y*b.x;
	a_cross_b.type = 1;
	return a_cross_b;
}

// Scalar Multiplication 
Vector Vector::operator*(double b)
{
	Vector a_mult_b;
	a_mult_b.x = x*b;
	a_mult_b.y = y*b;
	a_mult_b.z = z*b;
	a_mult_b.type = type;
	return a_mult_b;
}

// Vector Addition
Vector Vector::operator+(Vector b)
{
	Vector a_plus_b;
	a_plus_b.x = x + b.x;
	a_plus_b.y = y + b.y;
	a_plus_b.z = z + b.z;
	a_plus_b.type = 1;
	return a_plus_b;
}

// Vector subtraction
Vector Vector::operator-(Vector b)
{
	Vector a_minus_b;
	a_minus_b.x = x - b.x;
	a_minus_b.y = y - b.y;
	a_minus_b.z = z - b.z;
	a_minus_b.type = 1;
	return a_minus_b;
}

// Vector magnitude
double Vector::mag(bool warn = 1)
{
	if(type == 0 && warn == 1) printf("Getting magnitude of point !!!\n");
	return sqrt(x*x + y*y + z*z);
}

// Unit vector creation
Vector Vector::unit()
{
	if(type == 0) printf("Creating unit vector of point !!!\n");
	Vector a_unit;
	double a_mag = mag(0);
	a_unit.x = x/a_mag;
	a_unit.y = y/a_mag;
	a_unit.z = z/a_mag;
	a_unit.type = type;
	return a_unit;
}

// Print Vector
void Vector::print()
{
	printf("(%f , %f , %f , Type = %d)\n", x , y , z, type);
}


// Affine transformation of a point
Vector Vector::affineTransform(Mat M,int point_bool)
{
	Vector changed_point = Vector(0,0,0,0);
	double add=0;


	// Iterate through row of point vector
	for(int i=0;i<3;i++)
	{	add=0;
		add += x*M.at<float>(0,i);
		add += y*M.at<float>(1,i);
		add += z*M.at<float>(2,i);
		add += (point_bool)*M.at<float>(3,i);
		changed_point.x = (i==0)?add:changed_point.x;
		changed_point.y = (i==1)?add:changed_point.y;
		changed_point.z = (i==2)?add:changed_point.z;		
	}

	changed_point.type = type;
	return changed_point;
}

class Colour
{
	public:
		int r;
		int g;
		int b;
		
    	double ka,kd,ks;

		Colour(int , int , int);
};

Colour::Colour(int r_set = 1, int g_set = 0, int b_set = 0)
{
	r = r_set; ka=0.3; 
	g = g_set; kd=0.3;
	b = b_set; ks=0.3;
}

// Class for defining sphere
class Sphere
{
	public:
		double radius;
		Vector center;
		Colour colour;
		int affined;
		Mat affine,inverse,tpose;
		double reflectivity = 0.0, refractivity= 0.5;
		double mu_refract = 1.5;
		Sphere(double, Vector, Colour);
		pair<double, double> get_intersection(Vector, Vector);	// Get intersection point(s) with ray
		Vector get_normal(Vector);
		Vector get_reflection(Vector, Vector);
		Vector get_refraction(Vector, Vector, double, double);
};

Sphere::Sphere(double radius_set = 30, Vector center_set = Vector(0,0,0,0), Colour colour_set = Colour(200,10,10))
{
	radius = radius_set;
	center = center_set;
	colour = colour_set;
	affined=0;
	affine =  Mat::zeros(4,4,CV_32FC1);
	inverse = Mat::zeros(4,4,CV_32FC1);
	tpose = Mat::zeros(4,4,CV_32FC1);
}

pair<double, double> Sphere::get_intersection(Vector ray_pt, Vector ray_dir)
{

	// Get unit vector

	double mag_dir=1.0;

	Vector ray_udir = ray_dir.unit();

	// Check affined and change point ray and direction ray.
	if(affined)
	{	
		ray_pt = ray_pt.affineTransform(inverse,1);
		ray_udir = ray_dir.affineTransform(inverse,0);	
		mag_dir = ray_udir.mag();  // calculate mag( |ray_dir* inv(M)| )
	} 

	ray_udir = ray_udir.unit();

	//Calculate  the values t1 and t2 for R_o + R_d*t to lie on sphere
	Vector join_dir = ray_pt - center;
	double B = 2*(ray_udir^join_dir);
	double C = join_dir.mag()*join_dir.mag() - radius*radius;
	// If does not intersect return type -1

	if(B*B < 4*C)
	{
		return make_pair(100000000,100000000);
	}

	// Else calculate t1, t2 and return the 2 points
	double t_1 = (-B - sqrt(B*B - 4*C))/2.0;
	double t_2 = (-B + sqrt(B*B - 4*C))/2.0;
	Vector p_1 = ray_pt + (ray_udir * t_1);
	Vector p_2 = ray_pt + (ray_udir * t_2);

	// TO CHECK
	// if affined then t_affined * unit  =  t_org * mag_dir.
	// Thus t_org =  t_affined / mag_dir;

	return make_pair(t_1/mag_dir,t_2/mag_dir);
}

// Return normal of sphere.
Vector Sphere::get_normal(Vector point)
{

  // If affined then bring point back to non-affine form and then calculate normal.
  if(affined)
  {
  	point = point.affineTransform(inverse,1);
  }


  Vector norm;
  norm.x = (point.x-center.x);
  norm.y = (point.y-center.y);
  norm.z = (point.z-center.z);

  // Transform normal obtain to new normal for affined object.
  if(affined)
  {
  	norm = norm.affineTransform(tpose,0);
  }


  return norm.unit();
}

Vector Sphere::get_reflection(Vector point, Vector incoming)
{
	Vector normal = get_normal(point);
	return (((normal*(normal^incoming))*2) - incoming).unit(); 
}

Vector Sphere::get_refraction(Vector incoming, Vector point,double mu_incoming,double mu_outgoing)
{
	// Get normal at point p.
    Vector normal = get_normal(point);

    normal = normal.unit();
    incoming  = incoming.unit();    

    //Calculate  absolute sine  and cosine of incident theta.
    double cos_inc_theta = abs((normal.unit()^incoming.unit()));

    double sin_inc_theta  = sqrt(1.0 - pow(cos_inc_theta,2.0)); 


    // Calculate  absolute sine of refracted theta.
    double sin_ref_theta = (mu_incoming*sin_inc_theta)/(mu_outgoing);


    //For total inter reflection when sin_theta_t > 1.
    if(sin_ref_theta > 1.0)
    {	
    	cout<<"catch sphere\n";
    	// Return any vector not of unit length.
    	return Vector(1,1,1);
    }

    //Calculate cosine of refracted theta.
    double cos_ref_theta  = sqrt(1.0 - pow(sin_ref_theta,2.0));

    //Seen from slides.
    //TO CHECK if incoming and normal should be in unit vector.
    Vector refracted  = (incoming + normal*(cos_inc_theta))*(mu_incoming/mu_outgoing) - normal*(cos_ref_theta);

    return refracted.unit();

}



// Polygon class
vector<Vector> points;
class Polygon
{
	public:
		vector<Vector> vertices;
		Colour colour;
		int affined;
		Mat affine,tpose,inverse;
		double reflectivity = 0.5, refractivity= 0.0;
		double mu_refract = 1.0;
		Polygon(vector<Vector>, Colour);
		double get_intersection(Vector, Vector);	// Get intersection point of ray with plane
		Vector get_normal(Vector);
		Vector get_reflection(Vector, Vector);
		Vector get_refraction(Vector, Vector, double, double);
};

Polygon::Polygon(vector<Vector> vertices_set = points, Colour colour_set = Colour(200,200,200))
{
	colour = colour_set;
	vertices = vertices_set;
	affined=0;
	affine =  Mat::zeros(4,4,CV_32FC1);
	inverse = Mat::zeros(4,4,CV_32FC1);
	tpose = Mat::zeros(4,4,CV_32FC1);
}

double Polygon::get_intersection(Vector ray_pt, Vector ray_dir)
{
	// Construct plane from points // Doubt in direction of normal
	Vector normal = ((vertices[1]-vertices[0])&(vertices[vertices.size()-1] - vertices[0])).unit();
	double D = -1*((vertices[0] - Vector(0,0,0,0))^normal);

	// Calculate intersection with plane
	Vector ray_udir = ray_dir.unit();

	double mag_dir=1.0;

	if(affined)
	{
		ray_pt = ray_pt.affineTransform(inverse,1);
		ray_udir = ray_dir.affineTransform(inverse,0);
		mag_dir = ray_udir.mag();
	}

	ray_udir = ray_udir.unit();

	double numerator = -((normal^ray_pt) + D);
	double denominator = (normal^ray_udir);
	double t = numerator/denominator;
	if(t<0) 
	{
		// printf("Plane before screen\n");
		return 100000000;
	}
	Vector p = ray_pt + (ray_udir*t);

	// Check if point lies inside polygon (use 2 rays for more confidence)
	int count1 = 0, count2 =0;
	Vector ray1 = (vertices[1] - vertices[0]);
	Vector ray2 = (vertices[2] - vertices[0]);
	
	Vector n1 = (ray1&normal).unit();
	for(int i=0; i<vertices.size();i++)
	{
		Vector A; 
		if(i == 0) A = vertices[vertices.size()-1];
		else A = vertices[i-1];
		Vector B = vertices[i];
		double numerator2 = (A - p)^n1;
		double denominator2 = (A - B)^n1;

		if(abs(denominator2)> 0)
		{
			double t2 = numerator2/denominator2; 
			double direction_sign = ((A + ((B-A)*t2)) - p)^ray1;
			if(t2 >= 0.001 && t2 <= 0.999 && direction_sign > 0) count1++;
		} 

	}

	Vector n2 = (ray2&normal).unit();
	for(int i=0; i<vertices.size();i++)
	{
		Vector A; 
		if(i == 0) A = vertices[vertices.size()-1];
		else A = vertices[i-1];
		Vector B = vertices[i];
		double numerator2 = (A - p)^n2;
		double denominator2 = (A - B)^n2;

		if(abs(denominator2)> 0)
		{
			double t2 = numerator2/denominator2; 
			double direction_sign = ((A + ((B-A)*t2)) - p)^ray2;
			if(t2 >= 0.001 && t2 <= 0.999 && direction_sign > 0) count2++;
		} 
	}
	if(count1%2 == 1 || count2%2 == 1) return t/mag_dir;
	return 100000000;
}


Vector Polygon::get_normal(Vector point)
{
	Vector normal = ((vertices[1]-vertices[0])&(vertices[2] - vertices[0])).unit();
	
	// Check if affined and change normal accordingly.
	if(affined)
	{
		normal = normal.affineTransform(tpose,0);
	}

	return normal;	
}


Vector Polygon::get_reflection(Vector point, Vector incoming)
{
	Vector normal = get_normal(point);
	return (((normal*(normal^incoming))*2)- incoming).unit(); 
}

Vector Polygon::get_refraction(Vector incoming, Vector point,double mu_incoming,double mu_outgoing)
{
	// Get normal at point p.
    Vector normal = get_normal(point);

    normal = normal.unit();
    incoming  = incoming.unit();



    //Calculate  absolute sine  and cosine of incident theta.
    double cos_inc_theta = abs((normal.unit()^incoming.unit()));

    double sin_inc_theta  = sqrt(1.0 - pow(cos_inc_theta,2.0)); 


    // Calculate  absolute sine of refracted theta.
    double sin_ref_theta = (mu_incoming*sin_inc_theta)/(mu_outgoing);


    //For total inter reflection when sin_theta_t > 1.
    if(sin_ref_theta > 1.0)
    {	
    	// Return any vector not of unit length.
    	return Vector(1,1,1);
    }

    //Calculate cosine of refracted theta.
    double cos_ref_theta  = sqrt(1.0 - pow(sin_ref_theta,2.0));

    //Seen from slides.
    //TO CHECK if incoming and normal should be in unit vector.
    Vector refracted  = (incoming + normal*(cos_inc_theta))*(mu_incoming/mu_outgoing) - normal*(cos_ref_theta);

    return refracted.unit();

}

// Class for screen on which image will be generated
class Screen
{
	public:			
		Vector camera;
		Vector normal;
		double focal_length;
		double scale;
		pair<int, int> pixel_dim;
		vector<Sphere> spheres;
		vector<Vector> lights;
		vector<Polygon> polygons;

		Screen(Vector, Vector, double, double, pair<int,int>);
		pair<Vector,Vector> get_plane_vectors();	// Get 2 perpendicular unit vectors on image plane
		Vec3b calculate_intensity(Vector, Vector, Vector, Colour , vector<Vector>);
		Mat gen_image();
		Mat anti_aliasing(int,int);
		Vec3b recursive_intersection(Vector, Vector, double, double);
};

Screen::Screen(Vector camera_set = Vector(0,0,50,0), Vector normal_set = Vector(0,0,-1,1),
				 double focal_length_set = 8, double scale_set = 0.05, pair<int,int> pixel_dim_set = make_pair(1000,1000))
{
	camera = camera_set;
	normal = normal_set.unit();
	focal_length = focal_length_set;
	scale = scale_set;
	pixel_dim = pixel_dim_set;
}

pair<Vector,Vector> Screen::get_plane_vectors()
{
	Vector u(1,0,0,1);

	if(abs((normal^u) - 1) <= 0.01) u = Vector(0,1,0,1);
	// Get first vector
	u = (u - (normal*(u^normal))).unit();
	// Get second vector
	Vector v = (normal&u).unit();
	return make_pair(u,v);
}

Vec3b Screen::calculate_intensity(Vector point, Vector camera, Vector norm, Colour colour, vector<Vector> lights)
{
	double Ia = 1, Id = 1, Is = 1, n = 2;
	double intensity = colour.ka*Ia;
	for(int i=0;i<lights.size();i++)
	{
		// Ray L at point
		Vector incident = (lights[i]-point).unit();

		int shadow = 0;
		for(int j=0; j<spheres.size(); j++)
		{
			pair<double, double> t = spheres[j].get_intersection(point,incident);
			if((((t.first) > 0.05) && (abs(t.first - 100000000) > 0.01)) || (((t.second) > 0.05) && (abs(t.second - 100000000) > 0.01)) )
			{
				shadow = 1;
				break;
			}
		}
		for(int j=0; j<polygons.size(); j++)
		{
			double t = polygons[j].get_intersection(point,incident);
			if((t > 0.05) && (abs(t - 100000000) > 0.01) )
			{
				shadow = 1;
				break;
			}
		}

		// Ray R at point
		Vector reflected = ((norm*(2*(incident^norm))) - incident).unit();
		Vector v_point = (camera - point).unit();
		// I += kd*Id*max(0,[L.N]) + ks*Is*max(0,[R.V]^n)
		if(shadow == 0)
			intensity += (colour.kd*Id*max(0,(incident^norm))) + (colour.ks*Is*pow(max(0,(reflected^v_point)),n));
	}
	Vec3b final_colour =  Vec3b(min(intensity*colour.b,255), min(intensity*colour.g,255), min(intensity*colour.r,255));
	return final_colour;

}  


Vec3b Screen::recursive_intersection(Vector ray_pt, Vector ray_dir, double factor,double mu_incoming)
{
	if(factor < 0.001) 
	{
		return Vec3b(0,0,0);
	}
	else
	{
		Vector ray_udir = ray_dir.unit();
		double t_min = 10000000;
		pair<int,int> index = make_pair(-1,-1);
		// Check all intersections
		for(int k=0; k<spheres.size(); k++)
		{
			pair<double, double> t_s = spheres[k].get_intersection(ray_pt, ray_udir);
			double t_tmp = t_min;
			if(t_s.first > 0.01) t_min = min(t_min, t_s.first);
			else if(t_s.second > 0.01) t_min = min(t_min, t_s.second);
			if(abs(t_min - t_tmp) > 0.001) index = make_pair(0,k);
		}

		for(int k=0; k<polygons.size(); k++)
		{
			double t = polygons[k].get_intersection(ray_pt, ray_udir);
			double t_tmp = t_min;
			if(t > 0.01) t_min = min(t_min, t);
			if(abs(t_min - t_tmp) > 0.001) index = make_pair(2,k);
		}


		if(index.first != -1)
		{
			Vector intersect = ray_pt + (ray_udir*t_min);
			
			Vector norm, refracted, reflected;
			Colour colour;
			double refractivity, reflectivity;
			// Refractive index of medium in which light is going.
			double mu_outgoing;

			switch(index.first)
			{
				case 0:
					norm =spheres[index.second].get_normal(intersect);
					colour= spheres[index.second].colour;
					refractivity = spheres[index.second].refractivity;
					reflectivity = spheres[index.second].reflectivity;
					reflected = spheres[index.second].get_reflection(intersect, ray_udir*(-1));
					mu_outgoing = (norm.unit()^ray_udir.unit())>0?1.0:spheres[index.second].mu_refract;
					refracted =  spheres[index.second].get_refraction(ray_udir,intersect,mu_incoming,mu_outgoing);
					break;		
				case 2:
					norm = polygons[index.second].get_normal(intersect);
					colour= polygons[index.second].colour;		
					refractivity = polygons[index.second].refractivity;
					reflectivity = polygons[index.second].reflectivity;
					reflected = polygons[index.second].get_reflection(intersect,ray_udir*(-1));
					mu_outgoing = (norm.unit()^ray_udir.unit())>0?1.0:spheres[index.second].mu_refract;
					refracted = polygons[index.second].get_refraction(ray_udir,intersect,mu_incoming,mu_outgoing);
			}
			Vec3b total_intensity = Vec3b(0,0,0);
			double absorb = (1 - reflectivity - refractivity);
			//If refracted vector not of unit length no refraction, internal reflection.
			if(refracted.mag()>1.001)
			{	
				reflectivity += refractivity;
			}
			Vec3b basic = calculate_intensity(intersect,camera,norm,colour, lights);
			// This rays goes back in same medium so no change of refractive index.
			Vec3b reflect = recursive_intersection(intersect, reflected, reflectivity*factor,mu_incoming);
			// Adding refracted ray contribution.
			Vec3b refract = Vec3b(0,0,0);
			if(refracted.mag()<1.001) 
			{	// Refracted ray goes into new medium.
				//if(refractivity!=0)cout<<mu_outgoing<<" "<<mu_incoming<<" "<<refractivity<<endl;
				refract =  recursive_intersection(intersect,refracted, refractivity*factor,mu_outgoing);
			}
			for(int l = 0; l< 3; l++)
			{
				total_intensity[l] += factor*absorb*basic[l]; 
				total_intensity[l] += reflect[l];
				total_intensity[l] += refract[l];
			}
			
			return total_intensity;
		}
		else
		{
			return Vec3b(0,0,0);
		}
	}
}


Mat Screen::gen_image()
{
	Vector center = camera + normal*focal_length;
	pair<Vector, Vector> vecs = get_plane_vectors();
	Vector u = vecs.first;
	Vector v = vecs.second;
	normal.print();
	u.print();
	v.print();
	center.print();
	Vector x_unit = u*scale;
	Vector y_unit = v*scale;
	Vector bottom_left = (center - (x_unit*(pixel_dim.first/2))) - (y_unit*(pixel_dim.second/2));
	bottom_left.print();
	// printf("Pixel dimension =  (%d , %d)\n", pixel_dim.first, pixel_dim.second);
	int count = 0;

	bottom_left.print();

	Mat_<Vec3b> img(pixel_dim.first, pixel_dim.second, Vec3b(0,0,0));


	for(int i=0; i<pixel_dim.first; i++)
	{
		for(int j=0; j<pixel_dim.second; j++)
		{
			//printf("Pixel : (%d , %d) out of (%d , %d)\n", i , j, pixel_dim.first, pixel_dim.second);
			// Create ray
			// Check intersection recursively
			Vector ray_pt = camera;
			Vector ray_udir = ((bottom_left + ((x_unit*i) + (y_unit*j)))- camera).unit(); 
			img(j,i) = recursive_intersection(ray_pt, ray_udir, 1, 1.0);
			//printf("%f\n , (Type %d , index %d)\n",t_min, index.first, index.second);

		}
	}
	return img;
	
}

Mat Screen::anti_aliasing(int m, int n)
{
	int ratio = 3;
	pixel_dim.first = ratio*m;
	pixel_dim.second = ratio*n;
	scale = scale/ratio;
	Mat big_mat = gen_image();
	Mat_<Vec3b> small_mat(m, n, Vec3b(0,0,0));
	for(int i=0; i<m; i++)
	{
		for(int j=0; j<n; j++)
		{
			double col_avg[3] = {0};
			for(int k=0; k<ratio ; k++)
			{
				for(int l=0; l<ratio; l++)
				{
					for(int h=0 ; h<3 ;h++)
						col_avg[h] += int(big_mat.at<Vec3b>(ratio*i + k, ratio*j+l)[h]);
				}
			}
			small_mat(i,j) = Vec3b(col_avg[0]/(ratio*ratio), col_avg[1]/(ratio*ratio), col_avg[2]/(ratio*ratio));
		}
	}
	return small_mat;

}


class coefficient{
	double r,g,b;
	int type;
	coefficient(double,double,double,int);
};

coefficient::coefficient(double r_=0.1,double g_=0.1,double b_=0.1,int type_=-1)
{	r=r_;g=g_;b=b_;
	type = type_;
}




// Define here the affine matrix for sphere object.
void input_sphere()
{
	affine_global = Mat::zeros(4,4,CV_32FC1);

	for(int i=0;i<4;i++)affine_global.at<float>(i,i)=1.0;


	affine_global.at<float>(3,3)=1.0;
	
	// Features

/*	affine_global.at<float>(3,3)=1.0;
	affine_global.at<float>(0,0)=1.0;
	affine_global.at<float>(0,2)=1.0;
	affine_global.at<float>(1,0)=1.0;
	affine_global.at<float>(1,1)=0.5;
	affine_global.at<float>(2,1)=0.5;
	affine_global.at<float>(2,2)=1.0;
*/

}


// Define here the affine matrix for triangle object.
void input_triangle()
{

	affine_global = Mat::zeros(4,4,CV_32FC1);

	for(int i=0;i<4;i++)
	{
		affine_global.at<float>(i,i)=1.0;
	}

	affine_global.at<float>(0,0)=1.0/sqrt(2.0);
	affine_global.at<float>(0,1)=1.0/sqrt(2.0);
	affine_global.at<float>(1,0)=-1.0/sqrt(2.0);	
	affine_global.at<float>(1,1)=1.0/sqrt(2.0);
}


// Calculate the matrix inverse and transpose matrix.
void affine_matrix_feature()
{

	if(determinant(affine_global)==0)
	{
		cout<<"Singluar Matrix\n";
		exit(0);
	}

	inverse_global = Mat::zeros(4,4,CV_32FC1);
	inverse_global =  affine_global.inv();


	tpose_global = Mat::zeros(4,4,CV_32FC1);
	tpose_global = inverse_global.t();


}


void take_input(Screen &screen, char* filename)
{
	string x;
	ifstream iff(filename);
	iff>>x;
	//Take screen params
	iff>>x;
	iff>>screen.camera.z;
	iff>>x>>screen.pixel_dim.first>>screen.pixel_dim.second;
	iff>> x>>x>>screen.focal_length >>x>> screen.scale;

	int num_spheres, num_polygon;
	
	iff>>x;

	iff>>num_spheres;
	for(int i=0; i< num_spheres; i++)
	{
		Sphere S;
		iff>>S.colour.r >> S.colour.g >> S.colour.b;
		iff>>S.reflectivity>>S.refractivity>>S.mu_refract;
		iff>>S.radius>> S.center.x >> S.center.y >> S.center.z;
		screen.spheres.push_back(S);
	}

	iff>>x;
	iff >> num_polygon;
	for(int i =0; i<num_polygon; i++)
	{
		Polygon P;
		int num_vertices;
		iff>>P.colour.r >> P.colour.g >> P.colour.b;
		iff>>P.reflectivity>>P.refractivity>>P.mu_refract;
		iff>>num_vertices;
		for(int j=0 ; j<num_vertices; j++)
		{
			Vector v;
			iff >> v.x >> v.y >> v.z;
			v.type =0;
			P.vertices.push_back(v);
		}
		screen.polygons.push_back(P);
	}
	iff>>x;
	int num_lights;
	iff>>num_lights;
	for(int i=0;i<num_lights;i++)
	{
		Vector v;
		iff >> v.x >> v.y >> v.z;
		v.type = 0;
		screen.lights.push_back(v);
	}



}

int main(int argc, char** argv)
{
	Screen screen;
	if(argc >= 2)
		take_input(screen,argv[1]);
	Mat img = screen.gen_image();
	imshow("window",img);
	waitKey(0);

	// T.get_normal(Vector(0,0,0,0)).print();

}




