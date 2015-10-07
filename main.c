#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdint.h>
#define SIZE 5
#define ITE 10000
#define MAX_TEMP 255.0
#define NO_NEIGHBOR 0.0
#define	WIDTH 128
#define HEIGHT 72
//#define WIDTH 1920
//#define HEIGHT 1080

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_RESET   "\x1b[0m"

char* uint32_to_char_array(uint32_t value) {
	char* res =  malloc(sizeof(char)*4);
	res[3] = value >> 24;
	res[2] = value >> 16;
	res[1] = value >> 8;
	res[0] = value;

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

	char * w = uint32_to_char_array(WIDTH);
	char * h = uint32_to_char_array(HEIGHT);
	char * header1_size = uint32_to_char_array(HEIGHT*WIDTH*3);
	char * header2_size = uint32_to_char_array(HEIGHT*WIDTH*3+54);

/*
	printf("%d %hhx %hhx %hhx %hhx\n",WIDTH,w[0],w[1],w[2],w[3]);
	printf("%d %hhx %hhx %hhx %hhx\n",HEIGHT,h[0],h[1],h[2],h[3]);
	printf("%d %hhx %hhx %hhx %hhx\n",HEIGHT*WIDTH*3,header1_size[0],header1_size[1],header1_size[2],header1_size[3]);
	printf("%d %hhx %hhx %hhx %hhx\n",HEIGHT*WIDTH*3+54,header2_size[0],header2_size[1],header2_size[2],header2_size[3]);
*/

	char header1[2] = { 0x42 ,0x4d };

	char header2[12] = { 0x00 ,0x00 , 0x00 ,0x00 ,0x36 ,0x00 ,0x00 ,0x00, 0x28 ,0x00 ,0x00 ,0x00};

	char header3[8] = { 0x01 ,0x00 , 0x18 ,0x00 , 0x00 ,0x00 ,0x00 ,0x00 };

	char header4[16] = {0xc4 ,0x0e ,0x00 ,0x00 ,
						0xc4 ,0x0e ,0x00 ,0x00 ,
						0x00 ,0x00 ,0x00 ,0x00 ,
						0x00 ,0x00 ,0x00 ,0x00 };


	if ( fichier != NULL) {
		fwrite(header1,sizeof(char),sizeof(header1),fichier);
		fwrite(header1_size,sizeof(char),sizeof(char)*4,fichier);
		fwrite(header2,sizeof(char),sizeof(header2),fichier);
		fwrite(w,sizeof(char),sizeof(char)*4,fichier);
		fwrite(h,sizeof(char),sizeof(char)*4,fichier);
		fwrite(header3,sizeof(char),sizeof(header3),fichier);
		fwrite(header2_size,sizeof(char),sizeof(char)*4,fichier);
		fwrite(header4,sizeof(char),sizeof(header4),fichier);
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
		}
	}
	for(i = (WIDTH/3); i<(WIDTH/3)+10;i++){
		for(j = (HEIGHT/2); j<(HEIGHT/2)+10;j++){
			next[TAB(i,j)] = MAX_TEMP;
		}
	}
	for(i = (WIDTH*2/3); i<(WIDTH*2/3)+10;i++){
		for(j = (HEIGHT/2); j<(HEIGHT/2)+10;j++){
			next[TAB(i,j)] = MAX_TEMP;
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
	float dt = 10.0e-7 ;  // dt = 5.0e-1;
   	float d = 1.0/((float)SIZE-1.0) ;

   	float delta = dt/pow(d,2.0) ;
	float *tab = malloc(sizeof(float)*WIDTH*HEIGHT);
	
	int nbHeatPoints = 200;
	int *heatPoints = malloc(sizeof(float)*nbHeatPoints);
	int i=0,i2=0, j=0;
	for (i=0;i < 10;i++)
			for (j=0;j < 10;j++)
					heatPoints[TAB2(i,j,10)] = TAB((WIDTH/3)+i,(HEIGHT/2)+j);
					heatPoints[TAB2(i+10,j+10,10)] = TAB((WIDTH*2/3)+i,(HEIGHT/2)+j);
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
