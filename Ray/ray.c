#include "ray.h"
#include "../ImageSpecReader/ImageSpecReader.h"
#include "../Vector/vector.h"
#include "../Color/color.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

intersectionData *intersectSphere(ray r, object o){
	if(o.shapeType != 0){
		exit(1);
	};
	intersectionData *intersection = malloc(sizeof(intersectionData));
	double t1;
	double t2;

	
	// A B and C refers to the coeficients of the quadratic formula formed by plugging in the ray equation into the sphere formula
	double A = r.dir.x*r.dir.x + r.dir.y*r.dir.y + r.dir.z*r.dir.z; 
	double B = 2*(r.dir.x*(r.origin.x - o.shape.s.center.x) + r.dir.y*(r.origin.y - o.shape.s.center.y) + r.dir.z*(r.origin.z - o.shape.s.center.z));
	double C = (r.origin.x - o.shape.s.center.x)*(r.origin.x - o.shape.s.center.x) + (r.origin.y - o.shape.s.center.y)*(r.origin.y - o.shape.s.center.y) + (r.origin.z - o.shape.s.center.z)*(r.origin.z - o.shape.s.center.z) - (o.shape.s.radius*o.shape.s.radius);
	
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
		intersection->distance = t1 <= t2 ? t1 : t2;
	} else if( t1 >= 0 ){
		intersection->distance = t1;
	} else{
		intersection->distance= t2;
	}
	intersection->iPoint = sumPoints(2, scale(intersection->distance, r.dir), r.origin);
	return intersection;
}

intersectionData *intersectPlane(point n, point p, ray r){
	intersectionData *intersection= malloc(sizeof(intersectionData));
	// simplifed plane intersect formula solved for intersection distance
	double t = (dot(n, p) - dot(n, r.origin) )/dot(n, r.dir);
	if(t > 0){
		intersection->distance = t;
		intersection->iPoint = sumPoints(2, scale(t, r.dir), r.origin);
	}else {
		intersection = NULL;
	}
	return intersection;
};

intersectionData *intersectTriangle(ImageSpec *spec, object *o, ray r){
	triangle t = o->shape.t;
	point p1 = spec->vertices[t.points[0]];
	if(o->shapeType != 3){
		printf("Not triangle");
		return NULL;
	}

	intersectionData *planeIntersection= intersectPlane(t.norm, p1, r);
	if(planeIntersection == NULL){
		return planeIntersection;
	}


	//get data for barycentric coords and return the data
	double d1p = dot(t.basisI, sumPoints(2, planeIntersection->iPoint, scale(-1, p1)));
	double d2p = dot(t.basisJ, sumPoints(2, planeIntersection->iPoint, scale(-1, p1)));

	// Barycentric Coordinates
	double b = (t.d22*d1p - t.d12*d2p)/t.det;
	double g = (t.d11*d2p - t.d12*d1p)/t.det;
	double a = 1-(b + g);

	if( a > 1 || a < 0 || b > 1 || b < 0 || g > 1 || g < 0  ){
		return  NULL;
	}

	planeIntersection->barycentCoords = (point){a, b, g};
	return planeIntersection;
}

intersectionData *intersect(ImageSpec *spec, ray r, object o){
	switch (o.shapeType) {
		case 0: {
			return intersectSphere(r, o);
		}
		case 3: {
			return intersectTriangle(spec,&o, r);
		}
		default:{
			printf("Unsuported shapeType");
			exit(1);
		}
	}
}

color calculateDiffuseComp(point normal, point lightDir, double diffuseStrength, color matColor){
	// Diffuse energy, cannot be negative
	double diffuseECon = dot(normal, lightDir);
	if(diffuseECon < 0){
		diffuseECon = 0;
	} 
	
	// set the intensity of the color based off of the energy 
	double diffuseIntensity = diffuseStrength * diffuseECon;

	// use intensity to scale the material color for the component based off of diffuse light
	color diffuseComponent = scaleColor(diffuseIntensity, matColor);
	return diffuseComponent;
};


color calculateSpecularComponent(point normal, point half, double specularStrength, int specularFallOff, color specularColor ){
	// specular energy, also cannot be negative
	double specECon = dot(half, normal);
	if(specECon < 0){
		specECon = 0;
	}

	//set the intensity of the specular light based off of the fall off and the materials specular property
	double specularIntensity = specularStrength * pow(specECon, specularFallOff);

	//scale specular color by specular intensity
	color specularComponent = scaleColor(specularIntensity, specularColor);
	return  specularComponent;
};

double calculateShadows(ray shadowRay, double distanceToLight,  int objectIndex, ImageSpec *spec){
	// flag to determine if pixel is in shadow 1 no shadow, 0 in shadow
	unsigned char S = 1;

	// the shadow ray points from the intersection point towards the light

	// negative value to check if we find an intersection
	intersectionData *shadowIntersection = malloc(sizeof(intersectionData));
	shadowIntersection->distance = -1;

	//find any spheres that intersect the ray
	for(int i = 0; i < spec->objectCount; i++){
		//skip the sphere if it is the one the pixel is on
		if( i == objectIndex){
			continue;
		}
		object object = spec->objects[i];
		intersectionData *intersection = intersect(spec, shadowRay, object);
		// if there is no intersection infront of the shadow ray skip
		if( intersection == NULL){
			continue;
		}
		// update closest intersection point of the shadow ray
		if( intersection->distance < shadowIntersection->distance || shadowIntersection->distance < 0 ){
			shadowIntersection = intersection;
		}
	}

	// if the shadow intersection is in front of the shadow ray, and before the lightsource, the point is shadowed (directional type lights are infinite distance so all intersections will happen before
	if( shadowIntersection->distance > 0 && shadowIntersection->distance < distanceToLight){
		S=0;
	}

	free(shadowIntersection);
	return S;

};

double calculateAttenuation(double distanceToLight, light *l){
	double attenuation = 1/(l->c1 + l->c2*distanceToLight + l->c3 * distanceToLight * distanceToLight);

	// clamp attenuation so it is not negative
	if (attenuation < 0){
		attenuation = 0;
	}
	if (attenuation > 1){
		attenuation = 1;
	}
	
	return attenuation;
};

color TraceRay(ImageSpec *spec, ray ray){
	intersectionData *closestIntersection = malloc(sizeof(intersectionData));
	closestIntersection->distance = -1;
	//check if the ray intersects any of the spheres, tracking the closest intersection point and what sphere it is.
	for(int i = 0; i < spec->objectCount; i++){
		object object = spec->objects[i];
		intersectionData *intersection = intersect(spec, ray, object);
		if( intersection == NULL){
			continue;
		}
		if( intersection->distance < closestIntersection->distance || closestIntersection->distance < 0 ){
			closestIntersection = intersection;
			closestIntersection->objIndex = i;
		}
	}
	// if we have intersected a sphere use it to shade the ray, otherwise return the background color.
	if(closestIntersection->distance >= 0){
		return ShadeRay(spec, &ray, closestIntersection);
	}
	free(closestIntersection);
	return spec->bkgcolor;
}

color ShadeRay(ImageSpec *spec, ray *r, intersectionData *intersection){
	// c = ka*Od + kd * (N.L) * Od + ks *(H.N)^n * Os
	
	//get the sphere and material of the sphere
	object o = spec->objects[intersection->objIndex];
	material mat = spec->materials[o.matIndex];
	color ambientColor = mat.matColor;
	point normal;
	if(o.shapeType == 0){
	// normal vector from the surface at our intersection point
		normal = normalize(sumPoints(2, intersection->iPoint, scale(-1, o.shape.s.center)));
		// if the object is textured get the texture coord and retrieve color
		if(o.textureIndex != -1){
			texture *t = spec->textures[o.textureIndex];
			//get texture color
			double phi = acos(normal.z);
			double theta = atan2(normal.y, normal.x);
			double v = phi/M_PI;
			double u;
			if(theta >=0){
				u = theta/(2*M_PI);
			}else{
				u = theta/(2*M_PI) + 1;
			}
			double integral;
			int i = round(modf(u, &integral)*(t->width-1));
			int j = round(modf(v, &integral)*(t->height-1));
			color textureColor = t->colors[j*t->width + i];
			ambientColor = (color){textureColor.r/255, textureColor.g/255, textureColor.b/255};
		}
	}
	else{
		point baryCoords = intersection->barycentCoords;
		if(o.shape.t.normals[0] >= 0){
			point n1 = spec->norms[o.shape.t.normals[0]];
			point n2 = spec->norms[o.shape.t.normals[1]];
			point n3 = spec->norms[o.shape.t.normals[2]];
			normal = normalize(sumPoints(3, scale(baryCoords.x,n1), scale(baryCoords.y,n2), scale(baryCoords.z,n3)));
		}else{
			normal = o.shape.t.norm;
		}
		// if the object is textured get the texture coord and retrieve color
		if(o.textureIndex != -1){
			texture *t = spec->textures[o.textureIndex];
			point tCoord1 = spec->textureCoords[o.shape.t.textures[0]];
			point tCoord2 = spec->textureCoords[o.shape.t.textures[1]];
			point tCoord3 = spec->textureCoords[o.shape.t.textures[2]];
			double u = baryCoords.x * tCoord1.x + baryCoords.y *tCoord2.x + baryCoords.z * tCoord3.x;	
			double v = baryCoords.x * tCoord1.y + baryCoords.y *tCoord2.y + baryCoords.z * tCoord3.y;	
			double integral;
			int i = round(modf(u, &integral)*(t->width-1));
			int j = round(modf(v, &integral)*(t->height-1));
			color textureColor = t->colors[j*t->width + i];
			ambientColor = (color){textureColor.r/255, textureColor.g/255, textureColor.b/255};
		}
	}

	// direction from surface to the base of the view ray
	point view = normalize(sumPoints(2, r->origin ,scale(-1,intersection->iPoint)));	
	
	// set the color to be the ambient component before adding light input
	color c = scaleColor(mat.ambientStrength, ambientColor);
	//check each light in the scene for how it modifies the color at the intersection
	for(int i = 0; i< spec->lightCount; i++){

		light l = spec->lights[i];
		point lightDir;
		
		// light type 1 is a point light otherwise it is a directional light source
		if(l.type==1){
			lightDir = normalize(sumPoints(2, l.loc, scale(-1, intersection->iPoint)));
		} else {
			lightDir = normalize(scale(-1, l.loc));
		}



		color diffuseComponent = calculateDiffuseComp(normal, lightDir, mat.diffuseStrength, ambientColor);

		// the directon halfway between the light and view, when this is close to the normal the specular highlight will be maximized
		point half = normalize(sumPoints(2, lightDir, view));

		color specularComponent = calculateSpecularComponent(normal, half, mat.specularStrength, mat.specularFallOff, mat.specularColor);

		ray shadowRay = {intersection->iPoint, lightDir};
		// Shade is defaulted 1 so that, if there is no shadow there is no scaling
		unsigned char S = 1;
		// Light attenuation is defaulted 1 so that, if the light is unattenuated there is no scaling
		double attenuation = 1;
		if( l.type == 1){
			double distanceToLight = length(sumPoints(2,l.loc, scale(-1,intersection->iPoint)));
			S = calculateShadows(shadowRay, distanceToLight, intersection->objIndex, spec);
			// if the light is an attenuated light source, use its constants to scale the dirstance to get expected fall off
			if(l.attenuated == 1){
				attenuation = calculateAttenuation(distanceToLight, &l);
			}
		} else {
			S = calculateShadows(shadowRay, DBL_MAX, intersection->objIndex, spec);
		}
		// combine the 3 components, scaling the light based components by the light intnsity and shadow flag
		color illuminatedComponent = scaleColor(l.intensity*S*attenuation, sumColors(2, diffuseComponent, specularComponent));
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
