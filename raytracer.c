/* Author: Sevin Dennis, Ben Vesel
 * Program: RayTracer
 * Course: CS4611
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "geom.h"
#include "vec.h"


/* Ray structure */
typedef struct {
   vec3f vector;
   vec3f posn;

} Ray;

/* RayHit structure */
typedef struct {
	Ray ray;
	float time_hit;
	vec3f normal;
	int reflective;
	vec3f hitPoint;
	vec3f color;
} RayHit;

/* Perspective matrix structure */
typedef struct {
  	vec3f camera_pos;
  	int distance;
  	int screen_width_world;
  	int screen_width_pixels;
  	float units_per_pixel;
} Perspective;


/* Used to test stbi_write_png to make sure it works */
void color_array(unsigned char * array, long unsigned int size){

	long unsigned  i;
	for(i = 0; i <= size; i++){

		//Red
		if(i % 3 == 0){
			array[i]=0xFF;

		//Green
		} else if(i % 3 == 1){
			//array[i]=0xFF;

		//Blue
		} else {
			array[i] = 0xFF;;
			
		}
	}
}

vec3f get_pixel_color(vec2f cord, unsigned char * array, int width){

	int pos = (cord.y*3*width)+(cord.x*3);

	return vec3(array[pos],array[pos+1],array[pos+2]);
}

void set_pixel_color(vec3f color, vec2f cord, unsigned char * array, int width){

	int pos = (cord.y*3*width)+(cord.x*3);

	array[pos] = color.x; //R
	array[pos+1] = color.y; //G
	array[pos+2] = color.z; //B
}
	material refl;
	material red;
	material blue;
	material white;
	
	material shadow;
	
	sphere sph1;
	sphere sph2;
	sphere sph3;
	triangle back1;
	triangle back2;
	triangle bot1;
	triangle bot2;
	triangle right;

	vec3f light_source;

void referenceGeometry() {
	
	// create three spheres
	
	sphere_new(vec3(0,0,-16), 2, &sph1, refl);

	
	sphere_new(vec3(3,-1,-14), 1, &sph2, refl);

	
	sphere_new(vec3(-3,-1,-14), 1, &sph3, red);

	// back wall
	
	triangle_new(vec3(-8,-2,-20),
	vec3(8,-2,-20),
	vec3(8,10,-20), &back1,blue);

	
	triangle_new(vec3(-8,-2,-20),
		vec3(8,10,-20),
		vec3(-8,10,-20), &back2,blue);

	// floor
	
	triangle_new(vec3(-8,-2,-20),
		vec3(8,-2,-10),
		vec3(8,-2,-20), &bot1,white);
	
	triangle_new(vec3(-8,-2,-20),
		vec3(-8,-2,-10),
		vec3(8,-2,-10), &bot2,white);
	// right red triangle
	
	triangle_new(vec3(8,-2,-20),
		vec3(8,-2,-10),
		vec3(8,10,-20), &right,red);

}

/* Checks for intersection between a triangle and a ray */
void sphere_intersect(RayHit * rayHit) {

	float time_hit0 = -1;
	float time_hit1 = -1;
	float truetime_hit;
	rayHit->time_hit = -1;
	rayHit->color = vec3(0, 0, 0);


	for( int i = 0; i < sphere_index; i++) {
		float front = vec3_dot(negate(rayHit->ray.vector), vec3_sub(rayHit->ray.posn,sph[i].pos));
		float second = pow(vec3_dot(rayHit->ray.vector, vec3_sub(rayHit->ray.posn,sph[i].pos)), 2) - (vec3_dot(rayHit->ray.vector, rayHit->ray.vector) * (vec3_dot(vec3_sub(rayHit->ray.posn,sph[i].pos), vec3_sub(rayHit->ray.posn,sph[i].pos)) - pow(sph[i].radius, 2)));

		if( second < 0 ) {
			//printf( "no solution" );
		} else {
			second = sqrt(second);
			time_hit0 = (front + second) / vec3_dot(rayHit->ray.vector, rayHit->ray.vector);
			time_hit1 = (front - second) / vec3_dot(rayHit->ray.vector, rayHit->ray.vector);
			//printf("%d %f, %f\n",i, time_hit0, time_hit1);
			
			truetime_hit = time_hit0 < time_hit1 ? time_hit0 : time_hit1;
			if( truetime_hit < rayHit->time_hit || rayHit->time_hit < 0 ) {
				rayHit->time_hit = truetime_hit;
				//printf("setting color %f %f\n", sph[i].mat.color.x, sph[i].mat.color.y);


				vec3f intersection_vec = rayHit->ray.vector;
				intersection_vec.x = intersection_vec.x * truetime_hit + rayHit->ray.posn.x;
				intersection_vec.y = intersection_vec.y * truetime_hit + rayHit->ray.posn.y;
				intersection_vec.z = intersection_vec.z * truetime_hit + rayHit->ray.posn.z;

				vec3f normal = normalize(vec3_sub(intersection_vec, sph[i].pos));
				vec3f light_vector = normalize(vec3_sub( light_source, intersection_vec));
				
				float diffuse = vec3_dot(normal,light_vector);

				rayHit->color = sph[i].mat.color;

				if(diffuse < .2){
					diffuse = .2;
				}
				
				rayHit->color.x = rayHit->color.x *  diffuse;
				rayHit->color.y = rayHit->color.y *  diffuse;
				rayHit->color.z = rayHit->color.z *  diffuse;

				rayHit->reflective = sph[i].mat.reflective;
				rayHit->hitPoint = intersection_vec;
				float refl_dot = 2 * vec3_dot(intersection_vec, normal);
				vec3f relf_normal = vec3_mult(normal, refl_dot);
				rayHit->normal = vec3_sub(intersection_vec,relf_normal);

			}
		}	
	}
}

/* Checks for intersection between a triangle and a ray */
void triangle_intersect(RayHit * rayHit) {
	float A, B, C, D, E, F, G, H, I, J, K, L, M, beta, gamma, time_hit;
	for( int i = 0; i < triangle_index; i++) {
		triangle temp = tri[i];
		A = temp.posA.x - temp.posB.x;
		B = temp.posA.y - temp.posB.y;
		C = temp.posA.z - temp.posB.z;
		D = temp.posA.x - temp.posC.x;
		E = temp.posA.y - temp.posC.y;
		F = temp.posA.z - temp.posC.z;
		G = rayHit->ray.vector.x;
		H = rayHit->ray.vector.y;
		I = rayHit->ray.vector.z;
		J = temp.posA.x - 0;
		K = temp.posA.y - 0;
		L = temp.posA.z - 0;
		
		M = A * ( E * I - H * F) + B * ( G * F - D * I ) + C * ( D * H - E * G );	
		time_hit = (-1 * (F * (A * K - J * B) + E * (J * C - A * L) + D * ( B * L - K * C) )) / M;
		if( time_hit < 0  ) {
			continue;
		}
		gamma = ( I * ( A * K - J * B ) + H * ( J * C - A * L ) + G * (B * L - K * C )) / M;
		if( gamma < 0 || gamma > 1 ) {
			continue;
		}
		beta = ( J * ( E * I - H * F ) + K * ( G * F - D * I ) + L * ( D * H - E * G ) ) / M;
		if( beta < 0 || beta > 1 - gamma ) {
			continue;
		}

		
		if(time_hit > rayHit->time_hit ){
			rayHit->time_hit = time_hit;
		
		 	vec3f intersection_vec = rayHit->ray.vector;
			intersection_vec.x = intersection_vec.x * time_hit + rayHit->ray.posn.x;
			intersection_vec.y = intersection_vec.y * time_hit + rayHit->ray.posn.y;
			intersection_vec.z = intersection_vec.z * time_hit + rayHit->ray.posn.z;

			vec3f v1 = vec3_sub(temp.posA,temp.posB);
			vec3f v2 = vec3_sub(temp.posC,temp.posB);

			vec3f normal = normalize(vec3_cross(v2,v1));
			vec3f light_vector = normalize(vec3_sub( light_source, intersection_vec));
		
			float diffuse = vec3_dot(normal,light_vector);

			if(diffuse < .2){
				diffuse = .2;
			}

			rayHit->color = tri[i].mat.color;	
			rayHit->color.x = rayHit->color.x *  diffuse;
			rayHit->color.y = rayHit->color.y *  diffuse;
			rayHit->color.z = rayHit->color.z *  diffuse;

			// Calculate Shadows 
			Ray lightRay;
			lightRay.vector = light_vector;
			lightRay.posn = intersection_vec;
			
			RayHit rayHit_shadow;
			memset(&rayHit_shadow, 0, sizeof(rayHit_shadow));
			rayHit_shadow.ray = lightRay;
			sphere_intersect(&rayHit_shadow);
			
			
			if(rayHit_shadow.time_hit >= 0){
				rayHit->color = shadow.color;
			}
		}
	}
}


/* Returns a Ray to use with something. */
void GetRay(Perspective p, vec2f screenCoord, Ray *newRay){

	// Calculate the exact location of the middle of the pixel (offset of .5 * units_per_pixel)
	newRay->vector = normalize(vec3((-1 + screenCoord.x*p.units_per_pixel + .5 * p.units_per_pixel), (1 - screenCoord.y * p.units_per_pixel - .5 * p.units_per_pixel), -2));
	newRay->posn = vec3(0,0,0);

}

/*Main of the Program.*/
int main(int argc, char *argv[]){

	int width = 512;
	int height = 512;
	char * imageFileName;

	//Set the colors
	// make a material which is reflective
	refl.reflective = 1;
	refl.color = vec3(0,0,0); // color is not used when material is reflective!

	// make several diffuse materials to choose from
	red.reflective = 0;
	red.color = vec3(255,0,0);

	blue.reflective = 0;
	blue.color = vec3(0,0,255);

	white.reflective = 0;
	white.color = vec3(255,255,255);
	
	shadow.reflective = 0;
	shadow.color = vec3(45,43,43);
	
	light_source = vec3(3,5,-15);
	
	// Handle command line arguments
	if( argc < 2 ) {
		printf("Did not specify enough arguments.  Using reference.png\n");
		imageFileName = "reference.png";
	} else {
		imageFileName = malloc(sizeof(char) * 50);
		strncpy(imageFileName, argv[1], 49 );
		int offset = strlen(imageFileName);
		if( offset + 3 > 50 ) {
			printf("Specified a file name too long.  Exiting\n");
		}
		
		// Name the output image
		imageFileName[offset] = '.';
		imageFileName[offset+1] = 'p';
		imageFileName[offset+2] = 'n';
		imageFileName[offset+3] = 'g';
	}
	
	//Generate the array for the Image
	unsigned long size = height * width * 3 * sizeof(unsigned char);
	unsigned char * arrayContainingImage = malloc(size);
	
	if( !arrayContainingImage ) {
		printf("Failed to allocate memory.  Exiting\n");
		exit(10);
	}

	referenceGeometry();

	// Color the array
	//color_array(arrayContainingImage, size); //Temp Coloring to test writing image

	//Our camera pos and perspective
	Perspective myPer;
	myPer.camera_pos = vec3(0,0,0); // Camera Pos
	myPer.distance = -2; // 2 units from screen...
	myPer.screen_width_world = 2; //Screen width in meters
	myPer.screen_width_pixels = width; //Screen width in pixels
	myPer.units_per_pixel = ((float) myPer.screen_width_world)/myPer.screen_width_pixels;

	//Calculate all the reflections and etc. of a 512x512 
	//Described in slides
	//convert_geom_to_screen(myPer);

	for(int x = 0;x<height;x++){
		for(int y=0; y<width;y++){
			//create a ray
			Ray myRay;

			//generate that ray
			GetRay(myPer, vec2(x, y), &myRay);

			
			RayHit rayHit_sph;
			memset(&rayHit_sph, 0, sizeof(rayHit_sph));
			rayHit_sph.ray = myRay;
			sphere_intersect(&rayHit_sph);

			RayHit rayHit_tri;
			memset(&rayHit_tri, 0, sizeof(rayHit_tri));
			rayHit_tri.ray = myRay;
			triangle_intersect(&rayHit_tri);
			

			if(rayHit_sph.reflective == 1){

				vec3f temp1 = rayHit_sph.normal;
				vec3f temp2 = rayHit_sph.hitPoint;

				rayHit_sph.ray.vector = temp1;
				rayHit_sph.ray.posn = temp2;
				sphere_intersect(&rayHit_sph);

				//No hit on any sphere
				if(rayHit_sph.time_hit < 0){
					//Test for triangles

					rayHit_sph.ray.vector = temp1;
					rayHit_sph.ray.posn = temp2;
					triangle_intersect(&rayHit_sph);

					set_pixel_color(rayHit_sph.color, vec2(x, y), arrayContainingImage, myPer.screen_width_pixels);
				}else{
					//There was a hit, use that color
					set_pixel_color(rayHit_sph.color, vec2(x, y), arrayContainingImage, myPer.screen_width_pixels);
				}
				//IF we hit nothing at all.... treat it black(void)
				if(rayHit_sph.time_hit < 0){
					rayHit_sph.color = vec3(0,0,0);
					set_pixel_color(rayHit_sph.color, vec2(x, y), arrayContainingImage, myPer.screen_width_pixels);
				}
				//If no reflection		
			}else{
				// Color the pixel by the right object
				if(rayHit_sph.time_hit >= 0){
					set_pixel_color(rayHit_sph.color, vec2(x, y), arrayContainingImage, myPer.screen_width_pixels);

				}else{
					set_pixel_color(rayHit_tri.color, vec2(x, y), arrayContainingImage, myPer.screen_width_pixels);
				}
			}
					
		}
	}
	
	//Write out Image
	stbi_write_png(imageFileName, width, height, 3, arrayContainingImage, width*3);
	free(arrayContainingImage);



}




