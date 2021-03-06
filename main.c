#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <getopt.h>
#include <stdint.h>
#define SIZE 5
#define ITE 240000
#define MAX_TEMP 100.0
#define NO_NEIGHBOR 20.0

typedef struct {
	float* map;
	uint32_t width;
	uint32_t height;
} matrice;

char* uint32_to_char_array(uint32_t value) {
	char* res =  malloc(sizeof(char)*4);
	res[3] = value >> 24;
	res[2] = value >> 16;
	res[1] = value >> 8;
	res[0] = value;

	return res;
}

uint32_t char_array_to_uint32(char * value) {
	return *(uint32_t *)value;
}

matrice buildMatrice(uint32_t width,uint32_t height) {
	matrice mat;
	mat.width=width;
	mat.height=height;
	mat.map = malloc(sizeof(float)*width*height);
	return mat;
}

matrice buildMatriceWithData(uint32_t width,uint32_t height, float * data) {
	matrice mat;
	mat.width=width;
	mat.height=height;
	mat.map = data;
	return mat;
}

char* float_to_color(float value){
	char * color = malloc(sizeof(char)*3);

	uint8_t red = (value > MAX_TEMP*0.75) ? 255 : (value < MAX_TEMP*0.5) ? 0 : 4*(value-0.5) / MAX_TEMP * 255 ;
	uint8_t blue = (value < MAX_TEMP*0.25) ? 255 : (value > MAX_TEMP*0.5) ? 0 : (-value+0.5) / MAX_TEMP * 255 ;
	uint8_t green = (value > MAX_TEMP*0.75) ? 
						( (MAX_TEMP - value) / MAX_TEMP ) * 255 :
						(value < MAX_TEMP*0.25) ?
							( (value*4)/MAX_TEMP * 255 ) :
							255;

	color[0] = blue;
	color[1] = green;
	color[2] = red;
	return color;
}

void build(matrice tab,char *file_name) {

	FILE* fichier = NULL;

	fichier = fopen(file_name, "w");

	char * w = uint32_to_char_array(tab.width);
	char * h = uint32_to_char_array(tab.height);
	char * header1_size = uint32_to_char_array(tab.height*tab.width*3);
	char * header2_size = uint32_to_char_array(tab.height*tab.width*3+54);

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
		for (i=0;i<tab.width*tab.height;i++){
			rgb = float_to_color(tab.map[i]);
			fwrite(rgb,sizeof(char),sizeof(char)*3,fichier);
		}
	  	fclose(fichier);
		printf("%s printed\n",file_name);
	}
}

uint8_t fromBinary(char s) {
  return *( ( uint8_t * )&s );
}

matrice readImage(char * filename) {
	FILE *fp;
	fp = fopen(filename,"r+");
	float * data;
	uint32_t w ,h;
	int data_size;
	matrice mat;
	if (fp != NULL) {
		fseek(fp, 2, SEEK_SET);
		char size[4];
		fread(&size,1,4,fp);
		char width[4];
		fseek(fp,18,SEEK_SET);
		fread(&width,1,4,fp);
		w=char_array_to_uint32(width);
		char height[4];
		fseek(fp,22,SEEK_SET);
		fread(&height,1,4,fp);
		h=char_array_to_uint32(height);
		fseek(fp,54,SEEK_SET);

		data_size = char_array_to_uint32(size);

		char * data_picture = malloc(data_size);

		fread(data_picture,1,data_size,fp);
		data = malloc(sizeof(float)*(data_size/3));
		for (int i=0;i<data_size;i+=3){
			if (fromBinary(data_picture[i+2])>=250)
				data[i/3]=MAX_TEMP;
		}
	
		fclose(fp);
	}
	return buildMatriceWithData(w,h,data);
}

int TAB(int x, int y, uint32_t width) {
	return x + y * width;
}

void putHotPoints(matrice next) {
	int i,j;
	for(i = (next.width/6); i<(next.width/6)+10;i++){
		for(j = (next.height/5); j<(next.height/5)+10;j++){
			next.map[TAB(i,j,next.width)] = MAX_TEMP;
		}
	}
	for(i = (next.width*2/6); i<(next.width*2/6)+10;i++){
		for(j = (next.height/5); j<(next.height/5)+10;j++){
			next.map[TAB(i,j,next.width)] = MAX_TEMP;
		}
	}

	for(i = (next.width*3/6); i<(next.width*3/6)+10;i++){
		for(j = (next.height/5); j<(next.height/5)+10;j++){
			next.map[TAB(i,j,next.width)] = MAX_TEMP;
		}
	}
	for(i = (next.width*4/6); i<(next.width*3/6)+10;i++){
		for(j = (next.height/5); j<(next.height/5)+10;j++){
			next.map[TAB(i,j,next.width)] = MAX_TEMP;
		}
	}
	for(i = (next.width*4/6); i<(next.width*4/6)+10;i++){
		for(j = (next.height/5); j<(next.height/5)+10;j++){
			next.map[TAB(i,j,next.width)] = MAX_TEMP;
		}
	}
	for(i = (next.width*5/6); i<(next.width*5/6)+10;i++){
		for(j = (next.height/5); j<(next.height/5)+10;j++){
			next.map[TAB(i,j,next.width)] = MAX_TEMP;
		}
	}

	for(i = (next.width/6); i<(next.width/6)+10;i++){
		for(j = (next.height*4/5); j<(next.height*4/5)+10;j++){
			next.map[TAB(i,j,next.width)] = MAX_TEMP;
		}
	}
	for(i = (next.width*2/6); i<(next.width*2/6)+10;i++){
		for(j = (next.height*4/5); j<(next.height*4/5)+10;j++){
			next.map[TAB(i,j,next.width)] = MAX_TEMP;
		}
	}

	for(i = (next.width*3/6); i<(next.width*3/6)+10;i++){
		for(j = (next.height*4/5); j<(next.height*4/5)+10;j++){
			next.map[TAB(i,j,next.width)] = MAX_TEMP;
		}
	}
	for(i = (next.width*4/6); i<(next.width*3/6)+10;i++){
		for(j = (next.height*4/5); j<(next.height*4/5)+10;j++){
			next.map[TAB(i,j,next.width)] = MAX_TEMP;
		}
	}
	for(i = (next.width*4/6); i<(next.width*4/6)+10;i++){
		for(j = (next.height*4/5); j<(next.height*4/5)+10;j++){
			next.map[TAB(i,j,next.width)] = MAX_TEMP;
		}
	}
	for(i = (next.width*5/6); i<(next.width*5/6)+10;i++){
		for(j = (next.height*4/5); j<(next.height*4/5)+10;j++){
			next.map[TAB(i,j,next.width)] = MAX_TEMP;
		}
	}
}

/**
 * Fonction permettant l'initialisation du plateau ainsi que des points chauds. Chaque points chaud est initialisé à MAX_TEMP alors que le reste est à TEMP_AMBIANT
 */
void generate(matrice tab) {
	int i, j;
	for (i = 0; i < tab.width; i++) {
		for (j = 0; j < tab.height; j++) {
			tab.map[TAB(i, j, tab.width)] = NO_NEIGHBOR;
		}
	}
	putHotPoints(tab);
}

void calculNext(matrice tab, matrice next, float delta, int *heatPoints,
		int nbHeatPoints) {
	#pragma omp parallel for
	for (int i = 0; i < tab.width; i++) {
		for (int j = 0; j < tab.height; j++) {
			if (tab.map[TAB(i,j,tab.width)]> 0.999*MAX_TEMP)
				next.map[TAB(i, j, next.width)]=tab.map[TAB(i,j,tab.width)];
			else {
				float upside = (i == 0) ? NO_NEIGHBOR : tab.map[TAB(i - 1, j, tab.width)];
				float downside = (i == tab.width - 1) ? NO_NEIGHBOR : tab.map[TAB(i + 1, j, tab.width)];
				float rightside = (j == 0) ? NO_NEIGHBOR : tab.map[TAB(i, j - 1, tab.width)];
				float leftside = (j == tab.height - 1) ? NO_NEIGHBOR : tab.map[TAB(i, j + 1, tab.width)];
	
				next.map[TAB(i, j, next.width)] = tab.map[TAB(i, j, tab.width)]	+ delta	* (-4 * tab.map[TAB(i, j, tab.width)] + upside + downside + rightside + leftside);
			}
		}
	}
}

int main(int argc , char *argv[]){

	unsigned int width=600;
	unsigned int height=400;
	/* Attention si valeur trop basse ca pète */
	float dt = 10.0e-3;  // dt = 5.0e-1;
	char * filename;
	int image_in =0;

	int option = 0;
    while ( (option=getopt(argc, argv,"w:h:t:i:")) != -1 ) {
        switch (option) {
            case 'w' :
				width = atoi(optarg);
				break;
			case 'h' :
				height = atoi(optarg);
				break;
			case 'i' :
				filename = optarg;
				image_in=1;
				break;
			case 't' :
				if ( (dt = atof(optarg)) > 10.0e-3 )
					dt = 10.0e-3;
				break;
			default :
				printf("bad args: \n\t-w %%d+\n\t-h %%d\n\t-t float < 10.0e-3 && > 10.0e-0\n");
				return 1;
        }
    }
	matrice tab,next;
	if (image_in==1){
		tab = readImage(filename);
		next = readImage(filename);
	} else {
		tab = buildMatrice(width,height);
		generate(tab);
		next = buildMatrice(width,height);	
	}


   	float d = 1.0/((float)SIZE-1.0) ;
   	float delta = dt/pow(d,2.0) ;

	int nbHeatPoints = 1;
	int *heatPoints = malloc(sizeof(float)*nbHeatPoints);




	char str[15];
	int index = 0;

	int i=0;
	for(i = 0; i < ITE ;i++){
		calculNext(tab, next, delta,heatPoints, nbHeatPoints);
		if ((i % 4000) == 0) {
			sprintf(str, "%d.bmp", index++);
			build(next,str);
		}
		if (i==0) 
			free(tab.map);

		tab = next;
	}
	free(tab.map);

	return 0;
}
