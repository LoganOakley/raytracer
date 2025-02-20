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
	//get the sphere and material of the sphere
	sphere s = spec->spheres[sphereIndex];
	material mat = spec->materials[s.matIndex];
	// c = ka*Od + kd * (N.L) * Od + ks *(H.V)^n * Os
	
	point intersectionPoint = sumPoints(2, scale(intersectionDistance, r->dir), r->origin);
	// normal vector from the surface at our intersection point
	point normal = scale(1/s.radius, sumPoints(2, intersectionPoint, scale(-1, s.center)));
	// direction from surface to the base of the view ray
	point view = normalize(sumPoints(2, r->origin ,scale(-1,intersectionPoint)));	
	
	// set the color to be the ambient component before adding light input
	color c = scaleColor(mat.ambientStrength, mat.matColor);
	//check each light in the scene for how it modifies the color at the intersection
	for(int i = 0; i< spec->lightCount; i++){
		light l = spec->lights[i];

		point lightDir;

		// light type 1 is a point light otherwise it is a directional light source
		if(l.type==1){
			lightDir = normalize(sumPoints(2, l.loc, scale(-1, intersectionPoint)));
		} else {
			lightDir = normalize(scale(-1, l.loc));
		}

		// the director halfway between the light and view, when this is close to the normal the specular highlight will be maximized
		point half = normalize(sumPoints(2, lightDir, view));

		// Diffuse energy, cannot be negative
		float diffuseECon = dot(normal, lightDir);
		if(diffuseECon < 0){
			diffuseECon = 0;
		} 
	
		// set the intensity of the color based off of the energy 
		float diffuseIntensity = mat.diffuseStrength * diffuseECon;

		// use intensity to scale the material color for the component based off of diffuse light
		color diffuseComponent = scaleColor(diffuseIntensity, mat.matColor);

		// specular energy, also cannot be negative
		float specECon = dot(half, normal);
		if(specECon < 0){
			specECon = 0;
		}

		//set the intensity of the specular light based off of the fall off and the materials specular property
		float specularIntensity = mat.specularStrength * pow(specECon, mat.specularFallOff);

		//scale specular color by specular intensity
		color specularComponent = scaleColor(specularIntensity, mat.specularColor);

		// flag to determine if pixel is in shadow 1 no shadow, 0 in shadow
		unsigned char S = 1;

		// the shadow ray points from the intersection point towards the light
		ray shadowRay = {intersectionPoint, lightDir};

		// negative value to check if we find an intersection
		double shadowIntersection = -1;

		//find any spheres that intersect the ray
		for(int i = 0; i < spec->sphereCount; i++){
			//skip the sphere if it is the one the pixel is on
			if( i == sphereIndex){
				continue;
			}
			sphere sphere = spec->spheres[i];
			double *t = intersect(shadowRay, sphere);
			// if there is no intersection infront of the shadow ray skip
			if( t == NULL){
				continue;
			}
			// update closest intersection point of the shadow ray
			if( *t < shadowIntersection || shadowIntersection < 0 ){
				shadowIntersection = *t;
			}
		}

		// if the shadow intersection is in front of the shadow ray, and before the lightsource, the point is shadowed (directional type lights are infinite distance so all intersections will happen before
		if( shadowIntersection > 0 && (l.type = 0 || shadowIntersection < length(sumPoints(2,l.loc, scale(-1,intersectionPoint))))){
			S=0;
		}

		// combine the 3 components, scaling the light based components by the light intnsity and shadow flag
		color illuminatedComponent = scaleColor(l.intensity*S, sumColors(2, diffuseComponent, specularComponent));
		c = sumColors(2, c, illuminatedComponent); 
	}

	// after checking all lights, ensure no color is over max strength (1)
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
