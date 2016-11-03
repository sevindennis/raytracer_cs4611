/* Author: Sevin Dennis, Ben Vesel <- (10/31/2016)
 * Program: RayTracer CS4611 
 * Course: CS4611
 * Sevin Dennis and Ben Vessel... Just mentioning this.
 *	Already aware of that we worked together with Prof. Kulh
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
	int shadow;
} RayHit;

/* Perspective matrix structure */
typedef struct {
  	vec3f camera_pos;
  	int distance;
  	int screen_width_world;
  	int screen_width_pixels;
  	float units_per_pixel;
} Perspective;

//Global Variables
material refl;
material red;
material blue;
material white;
material yellow;

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

/* Used for debugging the color of a pixel */
vec3f get_pixel_color(vec2f cord, unsigned char * array, int width){

	int pos = (cord.y*3*width)+(cord.x*3);

	return vec3(array[pos],array[pos+1],array[pos+2]);
}

/* Used for setting the color of a pixel */
void set_pixel_color(vec3f color, vec2f cord, unsigned char * array, int width){

	int pos = (cord.y*3*width)+(cord.x*3);

	array[pos] = color.x; //R
	array[pos+1] = color.y; //G
	array[pos+2] = color.z; //B
}

/* Reference Scene Hard Coded in */
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

/* Custom/Fancy Scene Hard Coded in */
void customGeometry() {
	
	// create three spheres
	sphere_new(vec3(0,-2,-14), 1, &sph1, refl);
	sphere_new(vec3(0,1,-14), 1.5, &sph2, refl);
	sphere_new(vec3(0,4,-14), 1, &sph3, refl);

	sphere_new(vec3(-5,-2,-14), 1, &sph1, yellow);
	sphere_new(vec3(-5,1,-14), 1, &sph2, refl);
	sphere_new(vec3(-5,4,-14), 1, &sph3, red);

	sphere_new(vec3(5,-2,-14), 1, &sph1, red);
	sphere_new(vec3(5,1,-14), 1, &sph2, refl);
	sphere_new(vec3(5,4,-14), 1, &sph3, yellow);

	// floor
	triangle_new(vec3(-8,-6,-20),
		vec3(8,-6,-10),
		vec3(8,-6,-20), &bot1,white);
	
	triangle_new(vec3(-8,-6,-20),
		vec3(-8,-6,-10),
		vec3(8,-6,-10), &bot2,white);

	// Ceiling
	triangle_new(vec3(-8,8,-20),
		vec3(8,8,-10),
		vec3(8,8,-20), &bot1,white);
	
	triangle_new(vec3(-8,8,-20),
		vec3(-8,8,-10),
		vec3(8,8,-10), &bot2,white);
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
			
		} else {
			second = sqrt(second);
			time_hit0 = (front + second) / vec3_dot(rayHit->ray.vector, rayHit->ray.vector);
			time_hit1 = (front - second) / vec3_dot(rayHit->ray.vector, rayHit->ray.vector);
	
			truetime_hit = time_hit0 < time_hit1 ? time_hit0 : time_hit1;

			//T wont be negative... thats behind you
			if(truetime_hit < 0){
				continue;
			}

			if( truetime_hit < rayHit->time_hit || rayHit->time_hit < 0 ) {

				rayHit->time_hit = truetime_hit;

				if(rayHit->shadow == 1){
					return;
				}

				// Calculate intersection Vector
				vec3f intersection_vec = rayHit->ray.vector;
				intersection_vec.x = intersection_vec.x * truetime_hit + rayHit->ray.posn.x;
				intersection_vec.y = intersection_vec.y * truetime_hit + rayHit->ray.posn.y;
				intersection_vec.z = intersection_vec.z * truetime_hit + rayHit->ray.posn.z;

				// Calculate normals and light vector
				vec3f normal = normalize(vec3_sub(intersection_vec, sph[i].pos));
				vec3f light_vector = normalize(vec3_sub( light_source, intersection_vec));
				
				// Calculate diffuse shading
				float diffuse = vec3_dot(normal,light_vector);

				if(diffuse < .2){
					diffuse = .2;
				}
				
				// Set the colors
				rayHit->color = sph[i].mat.color;
				rayHit->color.x = rayHit->color.x *  diffuse;
				rayHit->color.y = rayHit->color.y *  diffuse;
				rayHit->color.z = rayHit->color.z *  diffuse;

				// Calculate Shadows 
				Ray lightRay;

				//Nudge the intersect point a tiny bit... reduce static
				intersection_vec.x = intersection_vec.x + .00001*intersection_vec.x;
				intersection_vec.y = intersection_vec.y + .00001*intersection_vec.y;
				

				lightRay.vector = light_vector;
				lightRay.posn = intersection_vec;
				
				//Create a rayhit shadow object
				RayHit rayHit_shadow;
				rayHit_shadow.ray = lightRay;
				rayHit_shadow.shadow = 1;
				sphere_intersect(&rayHit_shadow);
				
				// Hit something, then its in a shade
				if(rayHit_shadow.time_hit > rayHit->time_hit){
					rayHit->color = vec3_mult(shadow.color, diffuse+1.3);
				}

				//Calculate reflection options for if the object is reflective
				rayHit->reflective = sph[i].mat.reflective;
				rayHit->hitPoint = intersection_vec;
				float refl_dot = 2 * vec3_dot(intersection_vec, normal);
				vec3f relf_normal = vec3_mult(normal, refl_dot);
				rayHit->normal = normalize(vec3_sub(intersection_vec,relf_normal));
			
			}	
		}
	}
}

/* Checks for intersection between a triangle and a ray */
void triangle_intersect(RayHit * rayHit) {
	rayHit->time_hit = -1;
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
		J = temp.posA.x - rayHit->ray.posn.x;
		K = temp.posA.y - rayHit->ray.posn.y;
		L = temp.posA.z - rayHit->ray.posn.z;

		//All copied from slides 
		
		M = A * ( E * I - H * F) + B * ( G * F - D * I ) + C * ( D * H - E * G );	
		time_hit = (-1 * (F * (A * K - J * B) + E * (J * C - A * L) + D * ( B * L - K * C) )) / M;
		if(time_hit < 0){
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
		
		// If it made this far, from the above code.... you hit a triangle
		if(rayHit->time_hit > time_hit){
			continue;
			
		}else{
			rayHit->time_hit = time_hit;
		}
	
		if(rayHit->shadow == 1){
			return;
		}
		
		//Do the same steps from when you hit a circle
		
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

		//Nudge it a tiny bit
		intersection_vec.x = intersection_vec.x + .00001*intersection_vec.x;
		intersection_vec.y = intersection_vec.y + .00001*intersection_vec.y;
		

		lightRay.vector = light_vector;
		lightRay.posn = intersection_vec;
		
		RayHit rayHit_shadow;
		rayHit_shadow.ray = lightRay;
		rayHit_shadow.shadow = 1;
		sphere_intersect(&rayHit_shadow);
		
		if(rayHit_shadow.time_hit >= 0){
			rayHit->color = shadow.color;
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

	yellow.reflective = 0;
	yellow.color = vec3(255,255,0);
	
	
	char * reference = "reference.png";
	char * custom = "custom.png";
	char * fancy = "fancy.png";
	
	// Handle command line arguments
	if( argc < 2 ) {
		printf("Did not specify enough arguments.  Using reference.png\n");
		imageFileName = "reference.png";
		light_source = vec3(3,5,-15);
		referenceGeometry();
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

		if(imageFileName[0] == custom[0] || imageFileName[0] == fancy[0]){
			if(imageFileName[0] == custom[0]){
				printf("Using custom renderer, now rendering image for custom.\n");
				customGeometry();
				width = 2048;
				height = 2048;
				light_source = vec3(-5,189,-3);
			}else{
				printf("Using fancy renderer, now rendering image for custom.\n");
				customGeometry();
				width = 2300;
				height = 2300;
				light_source = vec3(-5,189,-3);
			}

		}else{
			if(imageFileName[0] == reference[0]){
				referenceGeometry();
				light_source = vec3(3,5,-15);
			}else{
				printf("Did not specify any corret arguments.  Using reference scene.\n");
				referenceGeometry();
				light_source = vec3(3,5,-15);
			}
			
		}
	}

	//Generate the array for the Image
	unsigned long size = height * width * 3 * sizeof(unsigned char);
	unsigned char * arrayContainingImage = malloc(size);
	
	if( !arrayContainingImage ) {
		printf("Failed to allocate memory.  Exiting\n");
		exit(10);
	}
	
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

			
			RayHit *rayHit_sph = malloc(sizeof(RayHit));
			memset(rayHit_sph, 0, sizeof(RayHit));
			rayHit_sph->ray = myRay;
			sphere_intersect(rayHit_sph);

			RayHit *rayHit_tri = malloc(sizeof(RayHit));
			memset(rayHit_tri, 0, sizeof(RayHit));
			rayHit_tri->ray = myRay;
			triangle_intersect(rayHit_tri);
			
			int count = 0;
			if(rayHit_sph->reflective == 1 ){

				vec3f temp1 = rayHit_sph->normal;
				vec3f temp2 = rayHit_sph->hitPoint;

				//adjust the hit point a tad

				rayHit_sph->ray.vector = temp1;
				rayHit_sph->ray.posn = temp2;
				sphere_intersect(rayHit_sph);
				//If you hit another reflective object


				rayHit_tri->ray.vector = temp1;
				rayHit_tri->ray.posn = temp2;
				triangle_intersect(rayHit_tri);

				//only bounce 3 times max
				while(count !=2 && rayHit_sph->reflective == 1){
					
					//Reset temp 1 and 2 to the correct starting pos/vector
					temp1 = rayHit_sph->normal;
					temp2 = rayHit_sph->hitPoint;

					//Set them like above
					rayHit_sph->ray.vector = temp1;
					rayHit_sph->ray.posn = temp2;

					//Recall intersect for sphere
					sphere_intersect(rayHit_sph);

					//Same thing for triangle
					rayHit_tri->ray.vector = temp1;
					rayHit_tri->ray.posn = temp2;
					triangle_intersect(rayHit_tri);

					count++;

				}

				/*  Alogrithum to set the colors :)   */
				
				if(rayHit_tri->time_hit > 0){
						set_pixel_color(rayHit_tri->color, vec2(x, y), arrayContainingImage, myPer.screen_width_pixels);
				}
				if(rayHit_sph->color.x == 0 && rayHit_sph->color.y == 0 && rayHit_sph->color.z == 0){
						continue;
				}else{
					set_pixel_color(rayHit_sph->color, vec2(x, y), arrayContainingImage, myPer.screen_width_pixels);
				}
				
			}else{
				if(rayHit_tri->time_hit > 0){
						set_pixel_color(rayHit_tri->color, vec2(x, y), arrayContainingImage, myPer.screen_width_pixels);
				}
				if(rayHit_sph->color.x == 0 && rayHit_sph->color.y == 0 && rayHit_sph->color.z == 0){
						continue;
				}else{
					set_pixel_color(rayHit_sph->color, vec2(x, y), arrayContainingImage, myPer.screen_width_pixels);
				}

			}					
		}
	}
	
	//Write out Image
	stbi_write_png(imageFileName, width, height, 3, arrayContainingImage, width*3);
	free(arrayContainingImage);



}




