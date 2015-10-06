#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#define SIZE 5
#define ITE 10000
#define MAX_TEMP 100
#define NO_NEIGHBOR 0
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_RESET   "\x1b[0m"


int TAB(int x, int y){
	return x + y*SIZE;
}

char *color(double val){
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

void show(double *tab){
	int i,j;
	for(i = 0; i<SIZE;i++){
		for(j = 0; j<SIZE;j++){
			double val = tab[TAB(i,j)];
			//printf("%so",color(val));
			printf("%f\t",val);
		}
		printf("\n");
	}
	printf("\n");
	printf("\e[1;1H\e[2J");
}

void generate(double *tab, int x, int y){
	int i,j;
	for(i = 0; i<SIZE;i++){
		for(j = 0; j<SIZE;j++){
			if(x == i && y == j){			
				tab[TAB(i,j)] = MAX_TEMP;
			}else{
				tab[TAB(i,j)] = 0;
			}
		}
	}
}

void calculNext(double *tab, double *next, double delta){
	int i,j;
	for(i = 0; i<SIZE;i++){
		for(j = 0; j<SIZE;j++){
			double upside = (i == 0)?NO_NEIGHBOR:tab[TAB(i-1,j)];
			double downside = (i == SIZE-1)?NO_NEIGHBOR:tab[TAB(i+1,j)];
			double rightside = (j == 0)?NO_NEIGHBOR:tab[TAB(i,j-1)];
			double leftside = (j == SIZE-1)?NO_NEIGHBOR:tab[TAB(i,j+1)];
			next[TAB(i,j)] = tab[TAB(i,j)] + delta * (-4 * tab[TAB(i,j)]+upside+downside+rightside+leftside);
		}
	}
}

int main(){
	double dt = 10.0e-7;  // dt = 5.0e-1;
    	double d = 1.0/((double)SIZE-1.0) ;

    	double delta = dt/pow(d,2.0) ;
	double *tab = malloc(sizeof(double)*SIZE*SIZE);

	int x = SIZE/2;
	int y = SIZE/2;
	generate(tab,x,y);	
	int i;
	for(i = 0; i<ITE;i++){
		double *next = malloc(sizeof(double)*SIZE*SIZE);
		calculNext(tab, next, delta);
		show(next);
		free(tab);
		tab = next;
		usleep(500000);
	}
	return 0;
}
