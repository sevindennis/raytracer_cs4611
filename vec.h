/*
* Authors: Sevin Dennis and Ben Vessel... Just mentioning this.
*	Already aware of that we worked together with Prof. Kulh
*/
#ifndef _VECRAYTRACER_
#define _VECRAYTRACER_

/* Two dimmensional vector structure */
typedef struct {
   float x;
   float y;
} vec2f;  
 
/* Three dimmensional vector structure */
typedef struct {
	float x;
	float y;
	float z;
} vec3f;

vec3f vec3(float x, float y, float z);

vec2f vec2(float x, float y);

float vec2_dot(vec2f vecA, vec2f vecB);

float vec3_dot(vec3f vecA, vec3f vecB);

vec3f vec3_cross(vec3f vecA, vec3f vecB);

vec3f normalize(vec3f vec);

vec3f negate(vec3f vector);

vec3f vec3_sub(vec3f vecA, vec3f vecB);

vec3f vec3_add(vec3f vecA, vec3f vecB);

vec3f vec3_mult(vec3f vecA, float scalar);
#endif