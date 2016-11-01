#ifndef _GEOMRAYTRACER_
#define _GEOMRAYTRACER_

#include "vec.h"

// Number of objects per array
#define ARRAY_OBJECTS 500

/* Start of geom header */
typedef struct {
	int reflective;
	vec3f color;
} material;

typedef struct {
	vec3f pos;
	float radius;
	material mat;
} sphere;

typedef struct {
	vec3f posA;
	vec3f posB;
	vec3f posC;
	material mat;
} triangle;

/* Geometry objects */
int sphere_index = 0;
int triangle_index = 0;
sphere   sph[ARRAY_OBJECTS];
triangle tri[ARRAY_OBJECTS];



/* 
 * Initialize and create a new triangle object then
 * add it to the tri array.
 *
 */
void triangle_new( vec3f posA, vec3f posB, vec3f posC, triangle * my_tri, material mat) {

	my_tri->posA = posA;
	my_tri->posB = posB;
	my_tri->posC = posC;
	my_tri->mat = mat;

	if(triangle_index < ARRAY_OBJECTS) {
		tri[triangle_index++] = *my_tri;	
	}
}

/*
 * Initialize and create a new sphere object then add 
 * it to the sph array filled with objects
 */
void sphere_new( vec3f pos, float radius, sphere * my_sph, material mat) {
	my_sph->pos = pos;
	my_sph->radius = radius;
	my_sph->mat = mat;
	if(sphere_index < ARRAY_OBJECTS) {
		sph[sphere_index++] = *my_sph;
	}
}

#endif