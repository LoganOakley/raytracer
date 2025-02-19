#include "ray.h"
#include "../ImageSpecReader/ImageSpecReader.h"
#include "../Vector/vector.h"
#include <math.h>
#include <stdlib.h>

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
	int intersectedSphereIndex;
	//check if the ray intersects any of the spheres, tracking the closest intersection point and what sphere it is.
	for(int i = 0; i < spec->sphereCount; i++){
		sphere sphere = spec->spheres[i];
		double *t = intersect(ray, sphere);
		if( t == NULL){
			continue;
		}
		if( *t < closestIntersection || closestIntersection < 0 ){
			closestIntersection = *t;
			intersectedSphereIndex = i;
		}
	}
	// if we have intersected a sphere use it to shade the ray, otherwise return the background color.
	if(closestIntersection >= 0){

		return ShadeRay(spec, intersectedSphereIndex, &ray, closestIntersection);
	}
	return spec->bkgcolor;
}

color ShadeRay(ImageSpec *spec, int sphereIndex, ray *r, double intersectionDistance){	
	sphere s = spec->spheres[sphereIndex];
	material mat = spec->materials[s.matIndex];
	// c = ka*Od + kd * (N.L) * Od + ks *(H.V)^n * Os
	point intersectionPoint = sumPoints(2, scale(intersectionDistance, r->dir), r->origin);
	point normal = scale(1/s.radius, sumPoints(2, intersectionPoint, scale(-1, s.center)));
	point view = normalize(sumPoints(2, r->origin ,scale(-1,intersectionPoint)));	
	
	color c = scaleColor(mat.ambientStrength, mat.matColor);
	for(int i = 0; i< spec->lightCount; i++){
		light l = spec->lights[i];

		point lightDir;

		if(l.type==1){
			lightDir = normalize(sumPoints(2, l.loc, scale(-1, intersectionPoint)));
		} else {
			lightDir = normalize(scale(-1, l.loc));
		}

		point half = normalize(sumPoints(2, lightDir, view));
		float diffuseECon = dot(normal, lightDir);
		if(diffuseECon < 0){
			diffuseECon = 0;
		} 
	
		float diffuseIntensity = mat.diffuseStrength * diffuseECon;

		color diffuseComponent = scaleColor(diffuseIntensity, mat.matColor);

		float specECon = dot(half, normal);
		if(specECon < 0){
			specECon = 0;
		}

		float specularIntensity = mat.specularStrength * pow(specECon, mat.specularFallOff);
		color specularComponent = scaleColor(specularIntensity, mat.specularColor);

		unsigned char S = 1;
		ray shadowRay = {intersectionPoint, lightDir};
		double shadowIntersection = -1;

		for(int i = 0; i < spec->sphereCount; i++){
			if( i == sphereIndex){
				continue;
			}
			sphere sphere = spec->spheres[i];
			double *t = intersect(shadowRay, sphere);
			if( t == NULL){
				continue;
			}
			if( *t < shadowIntersection || shadowIntersection < 0 ){
				shadowIntersection = *t;
			}
		}

		if( shadowIntersection > 0 && (l.type = 0 || shadowIntersection < length(sumPoints(2,l.loc, scale(-1,intersectionPoint))))){
			S=0;
		}
		color illuminatedComponent = scaleColor(l.intensity*S, sumColors(2, diffuseComponent, specularComponent));
		c = sumColors(2, c, illuminatedComponent); 
	}
	if(c.r > 1){
		c.r = 1;
	}
	if(c.g > 1){
		c.g = 1;
	}
	if(c.b > 1){
		c.b = 1;
	}
	return c; 
}
