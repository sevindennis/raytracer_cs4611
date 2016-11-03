/*
* Authors: Sevin Dennis and Ben Vessel... Just mentioning this.
*	Already aware of that we worked together with Prof. Kulh
*/

#include "vec.h"
#include <math.h>

/* Standard way to create a new vec3f */
vec3f vec3(float x, float y, float z){
	vec3f new_vec3f = {x,y,z};
	return new_vec3f;
}

/* Standard way to create a new vec2f */
vec2f vec2(float x, float y) {
	vec2f new_vec2f = {x,y};
	return new_vec2f;
}

/* Standard way to do a dot product with vec2 */
float vec2_dot(vec2f vecA, vec2f vecB) {
	float result = vecA.x * vecB.x + vecA.y * vecB.y;
	return result;
}

/* Standard way to do a dot product with vec3 */
float vec3_dot(vec3f vecA, vec3f vecB) {
	float result = vecA.x * vecB.x + vecA.y * vecB.y + vecA.z * vecB.z;
	return result;
}
/* Standard way to do a cross product with vec2 */
vec3f vec3_cross(vec3f vecA, vec3f vecB){
	vec3f result;
	// (a2b3 - a3b2)i + (a3b1-a1b3)j + (a1b2+a2b1)k
	result.x = (vecA.y*vecB.z - vecA.z*vecB.y);
	result.y = (vecA.z*vecB.x - vecA.x*vecB.z);
	result.z = (vecA.x*vecB.y - vecA.y*vecB.x);
	
	return result;
}

/* Normalize a vector */
vec3f normalize(vec3f vector) {
	float norm = sqrt(powf(vector.x, 2) + powf(vector.y, 2) + powf(vector.z, 2));
	if( norm == 0 ){ ;
		return vector;
	}
	vec3f new_vec = vec3(vector.x / norm, vector.y / norm, vector.z / norm);
	return new_vec;
}

/* Negatre a vector */
vec3f negate(vec3f vector) {
	return vec3(-vector.x, -vector.y, -vector.z);
}

/* Subtract a vector A from a vector B */
vec3f vec3_sub(vec3f vecA, vec3f vecB) {
	return vec3(vecA.x-vecB.x, vecA.y-vecB.y, vecA.z - vecB.z);
}

/* Add a vector A from a vector B */
vec3f vec3_add(vec3f vecA, vec3f vecB) {
	return vec3(vecA.x+vecB.x, vecA.y+vecB.y, vecA.z + vecB.z);
}

/* Multiply a vector A by a scalar  */
vec3f vec3_mult(vec3f vecA, float scalar) {
	return vec3(vecA.x * scalar, vecA.y * scalar, vecA.z * scalar);
}