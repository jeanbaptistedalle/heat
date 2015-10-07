#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdint.h>
#define SIZE 5
#define ITE 10000
#define MAX_TEMP 1000000000.0
#define NO_NEIGHBOR 20.0
#define	WIDTH 1280
#define HEIGHT 720
//#define WIDTH 1920
//#define HEIGHT 1080

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_RESET   "\x1b[0m"

char* uint32_to_char_array(uint32_t value) {
	char* res =  malloc(sizeof(char)*4);
	res[3] = value << 24;
	res[2] = value << 16;
	res[1] = value << 8;
	res[0] = value << 0;

	return res;
}

char* float_to_color(float value){
	char * color = malloc(sizeof(char)*3);
	uint8_t red = (value>(MAX_TEMP/2.0)) ? (value-(MAX_TEMP/2.0)) / (MAX_TEMP/2.0) * 255 : 0;
	uint8_t blue = (value<(MAX_TEMP/2.0)) ? ((MAX_TEMP/2.0)-value) / (MAX_TEMP/2.0) * 255 : 0;
	uint8_t green = (value>=(MAX_TEMP/2.0)) ? (MAX_TEMP-value) / (MAX_TEMP/2.0) * 255 : value / (MAX_TEMP/2.0) * 255 ;
	color[0] = blue;
	color[1] = green;
	color[2] = red;
	return color;
}

void build(float* data,char *file_name) {

	FILE* fichier = NULL;

	fichier = fopen(file_name, "w");
// header taille 1920*1080 
/*	char header[54] = { 0x42 ,0x4d ,
						0x36 ,0xec ,0x5e ,0x01 , //Size of the BMP file
						0x00 ,0x00 ,
						0x00 ,0x00 ,
						0x36 ,0x00 ,0x00 ,0x00 ,

						0x28 ,0x00 ,0x00 ,0x00 ,
						0x80 ,0x07 ,0x00 ,0x00 , //width
						0x38 ,0x04 ,0x00 ,0x00 , //height
						0x01 ,0x00 ,
						0x18 ,0x00 ,
						0x00 ,0x00 ,0x00 ,0x00 ,
						0x00 ,0xec ,0x5e ,0x00 , //Size of the raw bitmap data (including padding)
						0xc4 ,0x0e ,0x00 ,0x00 ,
						0xc4 ,0x0e ,0x00 ,0x00 ,
						0x00 ,0x00 ,0x00 ,0x00 ,
						0x00 ,0x00 ,0x00 ,0x00 };
*/
// header taille 1280*720 
	char header[54] = { 0x42 ,0x4d ,
						0x36 ,0x30 ,0x2a ,0x00 , //Size of the BMP file
						0x00 ,0x00 ,
						0x00 ,0x00 ,
						0x36 ,0x00 ,0x00 ,0x00 ,

						0x28 ,0x00 ,0x00 ,0x00 ,
						0x00 ,0x05 ,0x00 ,0x00 , //width
						0xd0 ,0x02 ,0x00 ,0x00 , //height
						0x01 ,0x00 ,
						0x18 ,0x00 ,
						0x00 ,0x00 ,0x00 ,0x00 ,
						0x00 ,0x30 ,0x2a ,0x00 , //Size of the raw bitmap data (including padding)
						0xc4 ,0x0e ,0x00 ,0x00 ,
						0xc4 ,0x0e ,0x00 ,0x00 ,
						0x00 ,0x00 ,0x00 ,0x00 ,
						0x00 ,0x00 ,0x00 ,0x00 };


	if ( fichier != NULL) {
		fwrite(header,sizeof(char),sizeof(header),fichier);
		int i;
		char *rgb;
		for (i=0;i<WIDTH*HEIGHT;i++){
			rgb = float_to_color(data[i]);
			fwrite(rgb,sizeof(char),sizeof(char)*3,fichier);
		}
	  	fclose(fichier);
	}
}

int TAB(int x, int y){
	return x + y*WIDTH;
}

int TAB2(int x, int y , int width){
	return x + y*width;
}

char *color(float val){
	if(val >= 4*MAX_TEMP/5){
		return ANSI_COLOR_RED;
	}else
	if(val >= 3*MAX_TEMP/5){
		return ANSI_COLOR_YELLOW;
	}else
	if(val >= 2*MAX_TEMP/5){
		return ANSI_COLOR_GREEN;
	}else
	if(val >= 1*MAX_TEMP/5){
		return ANSI_COLOR_BLUE;
	}
	return ANSI_COLOR_RESET;
}

void show(float *tab,char * file_name){

	build(tab,file_name);
	printf("%s printed\n",file_name);
/*
	printf("\e[1;1H\e[2J\n");
	int i,j;
	for(i = 0; i<WIDTH;i++){
		for(j = 0; j<HEIGHT;j++){
			float val = tab[TAB(i,j)];
			//printf("%so",color(val));
			printf("%f\t",val);
		}
		printf("\n");
	}
	printf("\n");
	printf("\e[1;1H\e[2J");
*/
}

/**
 * Fonction permettant l'initialisation du plateau ainsi que des points chauds. Chaque points chaud est initialisé à MAX_TEMP alors que le reste est à TEMP_AMBIANT
 */
void generate(float *tab, int *heatPoints, int nbHeatPoints){
	int i,j,k;
	for(i = 0; i<WIDTH;i++){
		for(j = 0; j<HEIGHT;j++){
			tab[TAB(i,j)] = 0;
			for(k=0; k < nbHeatPoints; k++){
				if(heatPoints[k] == TAB(i,j)){
					tab[TAB(i,j)] = MAX_TEMP;
				}
			}
		}
	}
}

void calculNext(float *tab, float *next, float delta, int *heatPoints, int nbHeatPoints){
	int i,j,k=0;
	for(i = 0; i<WIDTH;i++){
		for(j = 0; j<HEIGHT;j++){
			float upside = (i == 0)?NO_NEIGHBOR:tab[TAB(i-1,j)];
			float downside = (i == WIDTH-1)?NO_NEIGHBOR:tab[TAB(i+1,j)];
			float rightside = (j == 0)?NO_NEIGHBOR:tab[TAB(i,j-1)];
			float leftside = (j == HEIGHT-1)?NO_NEIGHBOR:tab[TAB(i,j+1)];

			next[TAB(i,j)] = tab[TAB(i,j)] + delta * (-4 * tab[TAB(i,j)]+upside+downside+rightside+leftside);
			//for(k=0; k < nbHeatPoints; k++){
				if(heatPoints[k] == TAB(i,j)){
					next[TAB(i,j)] = MAX_TEMP;
				}
			//}
		}
	}
}

/*
void calculNext(float *tab, float *next, float delta){
	int i,j;
	for(i = 0; i<WIDTH;i++){
		for(j = 0; j<HEIGHT;j++){
			float upside = (i == 0)?NO_NEIGHBOR:tab[TAB(i-1,j)];
			float downside = (i == WIDTH-1)?NO_NEIGHBOR:tab[TAB(i+1,j)];
			float rightside = (j == 0)?NO_NEIGHBOR:tab[TAB(i,j-1)];
			float leftside = (j == HEIGHT-1)?NO_NEIGHBOR:tab[TAB(i,j+1)];
			next[TAB(i,j)] = tab[TAB(i,j)] + delta * (-4 * tab[TAB(i,j)]+upside+downside+rightside+leftside);
		}
	}
}
*/


int main(){

	int index = 0;
	char str[15];
	srand(time(NULL));
	int picture_index = 0;

	/* Attention si valeur trop basse ca pète */
	float dt = 10.0e-3 ;  // dt = 5.0e-1;
   	float d = 1.0/((float)SIZE-1.0) ;

   	float delta = dt/pow(d,2.0) ;
	float *tab = malloc(sizeof(float)*WIDTH*HEIGHT);
	
	int nbHeatPoints = 1;
	int *heatPoints = malloc(sizeof(float)*nbHeatPoints);
	int i=0, j=0;
//	for (i=0;i < 10;i++)
//			for (j=0;j < 10;j++)
//					heatPoints[TAB2(i,j,10)] = TAB((WIDTH/2)+i,(HEIGHT/2)+j);
	heatPoints[0] = TAB((WIDTH/2),(HEIGHT/2));

	generate(tab,heatPoints, nbHeatPoints);	

	for(i = 0; ;i++){
		float *next = malloc(sizeof(float)*WIDTH*HEIGHT);
		//calculNext(tab, next, delta);
		calculNext(tab, next, delta,heatPoints, nbHeatPoints);
		if ((i % 4000) == 0) {
			sprintf(str, "%d.bmp", index++);
			show(next,str);
		}
		free(tab);
		tab = next;
//		usleep(500);
	}
	return 0;
}
