#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <getopt.h>
#include <stdint.h>
#include <string.h>
#include "mpi.h"
#define SIZE 5
//#define ITE 240000
#define ITE 1000000
#define MAX_TEMP 100.0
#define NO_NEIGHBOR 20.0
#define TEMP_AMBIANT NO_NEIGHBOR
#define ROOT 0

typedef struct {
	float* map;
	int width;
	int height;
} matrice;

char* uint32_to_char_array(uint32_t value) {
	char* res = malloc(sizeof(char) * 4);
	res[3] = value >> 24;
	res[2] = value >> 16;
	res[1] = value >> 8;
	res[0] = value;

	return res;
}

matrice buildMatrice(uint32_t width, uint32_t height, int bourrage) {
	matrice mat;
	mat.width = width;
	mat.height = height;
	mat.map = malloc(sizeof(float) * (width * height + bourrage));
	return mat;
}

char* float_to_color(float value) {
	char * color = malloc(sizeof(char) * 3);

	uint8_t red = (value > MAX_TEMP * 0.75) ? 255 : (value < MAX_TEMP * 0.5) ? 0 : 4 * (value - 0.5) / MAX_TEMP * 255;
	uint8_t blue = (value < MAX_TEMP * 0.25) ? 255 : (value > MAX_TEMP * 0.5) ? 0 : (-value + 0.5) / MAX_TEMP * 255;
	uint8_t green = (value > MAX_TEMP * 0.75) ? ((MAX_TEMP - value) / MAX_TEMP) * 255 :
					(value < MAX_TEMP * 0.25) ? ((value * 4) / MAX_TEMP * 255) : 255;

	color[0] = blue;
	color[1] = green;
	color[2] = red;
	return color;
}

void build(matrice tab, char *file_name) {

	FILE* fichier = NULL;

	fichier = fopen(file_name, "w");

	char * w = uint32_to_char_array(tab.width);
	char * h = uint32_to_char_array(tab.height);
	char * header1_size = uint32_to_char_array(tab.height * tab.width * 3);
	char * header2_size = uint32_to_char_array(tab.height * tab.width * 3 + 54);

	char header1[2] = { 0x42, 0x4d };

	char header2[12] = { 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00 };

	char header3[8] = { 0x01, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00 };

	char header4[16] = { 0xc4, 0x0e, 0x00, 0x00, 0xc4, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

	if (fichier != NULL ) {
		fwrite(header1, sizeof(char), sizeof(header1), fichier);
		fwrite(header1_size, sizeof(char), sizeof(char) * 4, fichier);
		fwrite(header2, sizeof(char), sizeof(header2), fichier);
		fwrite(w, sizeof(char), sizeof(char) * 4, fichier);
		fwrite(h, sizeof(char), sizeof(char) * 4, fichier);
		fwrite(header3, sizeof(char), sizeof(header3), fichier);
		fwrite(header2_size, sizeof(char), sizeof(char) * 4, fichier);
		fwrite(header4, sizeof(char), sizeof(header4), fichier);
		int i;
		char *rgb;
		for (i = 0; i < tab.width * tab.height; i++) {
			rgb = float_to_color(tab.map[i]);
			fwrite(rgb, sizeof(char), sizeof(char) * 3, fichier);
		}
		fclose(fichier);
		printf("%s printed\n", file_name);
	}
}

int TAB(int x, int y, int width) {
	return x + y * width;
}

/**
 * Fonction permettant l'initialisation du plateau
 */
void generate(matrice tab) {
	int i, j;
	for (i = 0; i < tab.width; i++) {
		for (j = 0; j < tab.height; j++) {
			tab.map[TAB(i, j, tab.width)] = TEMP_AMBIANT;
		}
	}
}

void putHotPoints(matrice next) {
	int i, j;
	for (i = (next.width / 3); i < (next.width / 3) + 10; i++) {
		for (j = (next.height / 2); j < (next.height / 2) + 10; j++) {
			next.map[TAB(i, j, next.width)] = MAX_TEMP;
		}
	}
	for (i = (next.width * 2 / 3); i < (next.width * 2 / 3) + 10; i++) {
		for (j = (next.height / 2); j < (next.height / 2) + 10; j++) {
			next.map[TAB(i, j, next.width)] = MAX_TEMP;
		}
	}
}

void calculNext(matrice tab, matrice next, float delta, int *heatPoints, int nbHeatPoints, int debut, int fin) {
	int i = 0;
	putHotPoints(tab);
#pragma omp parallel for
	for (i = debut; i < fin; i++) {
//		printf("%d < %d < %d\n", debut, i, fin);
		float upside = ((i - tab.width) < 0) ? NO_NEIGHBOR : tab.map[i - tab.width];
		float downside = (i + tab.width > tab.width * tab.height - 1) ? NO_NEIGHBOR : tab.map[i + tab.width];
		//Si on est au bord du tableau d'un côté ou de l'autre, on renvoie NO_NEIGHBOR
		float leftside = (i % tab.width == 0) ? NO_NEIGHBOR : tab.map[i - 1];
		float rightside = (i % tab.width == tab.width - 1) ? NO_NEIGHBOR : tab.map[i + 1];
//		float e = 10.0e-5;
//		if(upside>e || downside>e || rightside>e || leftside>e){
//			printf("%f %f %f %f\n",upside, downside, rightside, leftside);
//		}
		float temp = tab.map[i] + delta * (-4 * tab.map[i] + upside + downside + rightside + leftside);
		next.map[i] = temp;
	}
}

// mpiexec -n 6 ./heat_mpi.exe
int main(int argc, char *argv[]) {
	int mpi_rang;
	int mpi_nb;
	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rang);
	MPI_Comm_size(MPI_COMM_WORLD, &mpi_nb);

	unsigned int width = 600;
	unsigned int height = 400;
	/* Attention si valeur trop basse ca pète */
	float dt = 10.0e-5;  // dt = 5.0e-1;

	int option = 0;
	while ((option = getopt(argc, argv, "w:h:t:")) != -1) {
		switch (option) {
		case 'w':
			width = atoi(optarg);
			while (width % 8 != 0) {
				width++;
			}
			break;
		case 'h':
			height = atoi(optarg);
			while (height % 8 != 0) {
				height++;
			}
			break;
		case 't':
			if ((dt = atof(optarg)) > 10.0e-3)
				dt = 10.0e-3;
			break;
		default:
			printf("bad args: \n\t-w %%d+\n\t-h %%d\n\t-t float < 10.0e-3 && > 10.0e-0\n");
			return 1;
		}
	}

	float d = 1.0 / ((float) SIZE - 1.0);
	float delta = dt / pow(d, 2.0);

	int nbHeatPoints = 1;
	int *heatPoints = malloc(sizeof(float) * nbHeatPoints);

	int bourrage = mpi_nb - ((width * height) % mpi_nb);
	int size = width * height + bourrage;
	int sizeOfPart = size / mpi_nb;

	matrice tab = buildMatrice(width, height, bourrage);
	generate(tab);

	matrice next = buildMatrice(width, height, bourrage);
	generate(next);

	char str[15];
	int index = 0;
	int i = 0;
	//A chaque tour de boucle, chaque processus calcule un morceau de tableau
	for (i = 0;; i++) {
		//on selectionne le morceau à calculer, de mpi_rang*sizeOfPart à (mpi_rang+1)*sizeOfPart
		int debut = mpi_rang * sizeOfPart;
		int fin;
		fin = (mpi_rang + 1) * sizeOfPart;
		calculNext(tab, next, delta, heatPoints, nbHeatPoints, debut, fin);
		//puis, on recrée le tableau. Avec allgather, chacun récupère le tableau et est donc prêt à calculer la suite
		MPI_Allgather(&next.map[debut], sizeOfPart, MPI_FLOAT, tab.map, sizeOfPart, MPI_FLOAT, MPI_COMM_WORLD);
		if ((i % 4000) == 0 && mpi_rang == 0) {
			sprintf(str, "%d.bmp", index++);
			build(tab, str);
		}
	}
	MPI_Finalize();
	return EXIT_SUCCESS;
}

//int main(int argc, char *argv[]) {
//	int mpi_rang;
//	int mpi_nb;
//	MPI_Init(&argc, &argv);
//
//	MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rang);
//	MPI_Comm_size(MPI_COMM_WORLD, &mpi_nb);
//	int part = 2;
//	int total = mpi_nb * part;
//	int *tabTotal = (int*) malloc(sizeof(int) * total);
//	int i;
//	for (i = 0; i < part; i++) {
//		tabTotal[part * mpi_rang + i] = part * mpi_rang + i;
//	}
//	int*buff = (int*) malloc(sizeof(int) * part);
//	memcpy(buff, &tabTotal[part * mpi_rang], sizeof(int) * part);
//	if (mpi_rang == 0) {
//		printf("avant !\n");
//		for (i = 0; i < total; i++) {
//			printf("%d\n", tabTotal[i]);
//		}
//	}
//	MPI_Gather(buff, part, MPI_INT, tabTotal, part, MPI_INT, ROOT, MPI_COMM_WORLD);
//	if (mpi_rang == 0) {
//		printf("après !\n");
//		for (i = 0; i < total; i++) {
//			printf("%d\n", tabTotal[i]);
//		}
//	}
//	MPI_Finalize();
//	return EXIT_SUCCESS;
//}
