#include "ray.h"
#include <math.h>
#include <stdlib.h>
void printPointP(point p){
	printf("x: %.2f, y: %.2f, z: %.2f\n", p.x, p.y, p.z);
};

double *intersect(ray r, sphere s){
	double *intersectionDistance = malloc(sizeof(double));
	double t1;
	double t2;

	// A B and C refers to the coeficients of the quadratic formula formed by plugging in the ray equation into the sphere formula
	double A = r.dir.x*r.dir.x + r.dir.y*r.dir.y + r.dir.z*r.dir.z; 
	double B = 2*(r.dir.x*(r.origin.x - s.center.x) + r.dir.y*(r.origin.y - s.center.y) + r.dir.z*(r.origin.z - s.center.z));
	double C = (r.origin.x - s.center.x)*(r.origin.x - s.center.x) + (r.origin.y - s.center.y)*(r.origin.y - s.center.y) + (r.origin.z - s.center.z)*(r.origin.z - s.center.z) - (s.radius*s.radius);
	
	// check the descriminent to see if any real zeros exists
	double dis = B*B - 4*A*C;
	if(dis < 0){
		return NULL;
	}

	// get the two possible solutions and return the closest the ray origin in the positive direction.
	t1 = (-B + sqrt(dis))/(2*A);
	t2 = (-B - sqrt(dis))/(2*A);
	if( t1 < 0 && t2 < 0){
		return NULL;
	}
	if( t1 >= 0 && t2 >=0){
		*intersectionDistance = t1 <= t2 ? t1 : t2;
	} else if( t1 >= 0 ){
		*intersectionDistance = t1;
	} else{
		*intersectionDistance = t2;
	}
	return intersectionDistance;
}

color TraceRay(ImageSpec *spec, ray ray){
	double closestIntersection = -1;
	sphere intersectedSphere;
	//check if the ray intersects any of the spheres, tracking the closest intersection point and what speher it is.
	for(int i = 0; i < spec->sphereCount; i++){
		sphere sphere = spec->spheres[i];
		double *t = intersect(ray, sphere);
		if( t == NULL){
			continue;
		}
		if( *t < closestIntersection || closestIntersection < 0 ){
			closestIntersection = *t;
			intersectedSphere = sphere;
		}
	}
	// if we have intersected a asphere use it to shade the ray, otherwise return the background color.
	if(closestIntersection >= 0){
		return ShadeRay(spec, &intersectedSphere);
	}
	return spec->bkgcolor;
}
color ShadeRay(ImageSpec *spec, sphere *s){	

	return spec->materials[s->matIndex].matColor;
}
