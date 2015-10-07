#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#define SIZE 5
#define ITE 10000
#define MAX_TEMP 150
#define NO_NEIGHBOR 25
#define TEMP_AMBIANT NO_NEIGHBOR
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_RESET   "\x1b[0m"

int TAB(int x, int y) {
	return x + y * SIZE;
}

/**
 * Fonction permettant de determiner la couleur d'un point à partir de sa température
 */
char *color(float val) {
	if (val >= 4 * MAX_TEMP / 5) {
		return ANSI_COLOR_RED;
	} else if (val >= 3 * MAX_TEMP / 5) {
		return ANSI_COLOR_YELLOW;
	} else if (val >= 2 * MAX_TEMP / 5) {
		return ANSI_COLOR_GREEN;
	} else if (val >= 1 * MAX_TEMP / 5) {
		return ANSI_COLOR_BLUE;
	}
	return ANSI_COLOR_RESET;
}

/**
 * Fonction permettant de convertir une temperature en un pourcentage (0% > temperature ambiante, 100% > max temperature)
 */
float float2Percentage(float val) {
	return (val - TEMP_AMBIANT) / (MAX_TEMP - TEMP_AMBIANT) * 100;
}

unsigned int float2RGB(float val) {
	float percentage = float2Percentage(val);
	int r;
	int g;
	int b;
	if (percentage < 33.333) {
		r = g = 0;
		b = ((percentage / 33.3333) * 100) * 2.40 + 10;
	} else if (percentage < 66.6666) {
		r = 0;
		g = ((percentage - 33.3333) / (66.6666 - 33.3333) * 100) * 2.55;
		b = 0;
	} else {
		r = ((percentage - 66.6666) / (100.0 - 66.6666) * 100) * 2.55;
		b = g = 0;
	}
	return r * 255 * 255 + g * 255 + b;
	//return ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);
}

/**
 * Fonction permettant l'affichage en terminal du plateau, chaque "pixel" étant représenté par sa temperature actuelle
 */
void show(float *tab) {
	int i, j;
	for (i = 0; i < SIZE; i++) {
		for (j = 0; j < SIZE; j++) {
			float val = tab[TAB(i, j)];
			//printf("%so",color(val));
			int rgb = float2RGB(val);
			if (rgb == 0) {
				printf("0x000000");
			} else {
				printf("%#08x \t", float2RGB(val));
			}
		}
		printf("\n");
	}
	printf("\n");
	printf("\e[1;1H\e[2J");
}

/**
 * Fonction permettant l'initialisation du plateau ainsi que des points chauds. Chaque points chaud est initialisé à MAX_TEMP alors que le reste est à TEMP_AMBIANT
 */
void generate(float *tab, int *heatPoints, int nbHeatPoints) {
	int i, j, k;
	for (i = 0; i < SIZE; i++) {
		for (j = 0; j < SIZE; j++) {
			tab[TAB(i, j)] = TEMP_AMBIANT;
			for (k = 0; k < nbHeatPoints; k++) {
				if (heatPoints[k] == TAB(i, j)) {
					tab[TAB(i, j)] = MAX_TEMP;
				}
			}
		}
	}
}

/**
 * Fonction permettant le calcul du plateau suivant à partir du plateau actuel. La variation de delta permet de rendre l'animation plus ou moins rapide
 */
void calculNext(float *tab, float *next, float delta) {
	int i, j;
	for (i = 0; i < SIZE; i++) {
		for (j = 0; j < SIZE; j++) {
			float upside = (i == 0) ? NO_NEIGHBOR : tab[TAB(i - 1, j)];
			float downside = (i == SIZE - 1) ? NO_NEIGHBOR : tab[TAB(i + 1, j)];
			float rightside = (j == 0) ? NO_NEIGHBOR : tab[TAB(i, j - 1)];
			float leftside = (j == SIZE - 1) ? NO_NEIGHBOR : tab[TAB(i, j + 1)];
			next[TAB(i, j)] = tab[TAB(i, j)]
					+ delta
							* (-4 * tab[TAB(i, j)] + upside + downside
									+ rightside + leftside);
		}
	}
}

/**
 * Programme principal
 */
int heat() {
	float dt = 5.0e-1;
	float d = 1.0 / ((float) SIZE - 1.0);
	float delta = dt / pow(d, 2.0);

	float *tab = malloc(sizeof(float) * SIZE * SIZE);

	int nbHeatPoints = 2;
	int *heatPoints = malloc(sizeof(float) * nbHeatPoints);
	heatPoints[0] = TAB(2, 1);
	heatPoints[1] = TAB(2, 3);
	generate(tab, heatPoints, nbHeatPoints);
	int i;
	for (i = 0; i < ITE; i++) {
		float *next = malloc(sizeof(float) * SIZE * SIZE);
		calculNext(tab, next, delta);
		show(next);
		free(tab);
		tab = next;
		usleep(1000000);
	}
	return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
	return heat();
}
