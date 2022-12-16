#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <omp.h>
#include "headers/fileHandler.h"
#include "headers/config_t.h"
#include "headers/types_t.h"
#include "headers/rules.h"
#include "headers/coord_t.h"

void printResult (object_t **eco, config_t conf);
void destroyLock (object_t **eco, config_t conf);

int main (int argc, char **argv) {
	if (argc < 2) {
		printf("Usage:\n%s <config_file>\n\n", argv[0]);
		exit(1);
	}

	config_t conf;

	object_t **eco;
	coord_t p;

	printf("Lendo arquivo");
	readFile(argv[1], &conf, &eco);

	#ifdef STEP
		system("clear");
		printf("-------GEN:%2d-------\n", 0);
		printMatrix (eco, conf.L, conf.C, conf);
	#endif

	for (unsigned int t = 1; t <= conf.N_GEN; t++){
		conf.GEN++;

		#ifdef STEP
		getchar();
		system("clear");
		printf("-------GEN:%2d-------\n", t);
		#endif

		for (unsigned int i = 0; i < conf.L; i++){
			for (unsigned int j = 0; j < conf.C; j++){
				if (eco[i][j].animal.type == RABBIT){
					p = (coord_t){.x = i, .y = j};
					rabbit_rules (eco, conf, p);
				}	
			}
		}

		#ifdef PARALELO
		#pragma omp parallel for   //paralelo
		#endif
		for (unsigned int i = 0; i < conf.L; i++){
			for (unsigned int j = 0; j < conf.C; j++){
				eco[i][j].animal = choose_rabbit(eco[i][j]);
			}
		}

		#ifdef STEP2
			printMatrix (eco, conf.L, conf.C, conf);
			getchar();
			system("clear");
			printf("-------GEN:%2d-------\n", t);
		#endif


		for (unsigned int i = 0; i < conf.L; i++){
			for (unsigned int j = 0; j < conf.C; j++){
				if (eco[i][j].animal.type == FOX){
					p = (coord_t){.x = i, .y = j};
					fox_rules (eco, conf, p);
				}	
			}
		}

		#ifdef PARALELO
		#pragma omp parallel for    //paralelo
		#endif
		for (unsigned int i = 0; i < conf.L; i++){
			for (unsigned int j = 0; j < conf.C; j++){
				eco[i][j].animal = choose_fox(eco[i][j]);
			}
		}

		printf("\rGeração %d/%d", t, conf.N_GEN);
		fflush(stdout);
		#ifdef STEP
			printMatrix (eco, conf.L, conf.C, conf);
		#endif
	}


	return 0;
}


void printResult (object_t **eco, config_t conf) {
	int N = 0;
	for (unsigned int i = 0; i < conf.L; i++)
		for (unsigned int j = 0; j < conf.C; j++)
			if (eco[i][j].animal.type != EMPTY){
				N++;
			}

	printf("%u %u %u 0 %u %u %u\n",conf.GEN_PROC_COELHOS,
									conf.GEN_PROC_RAPOSAS,
									conf.GEN_COMIDA_RAPOSAS,
									conf.L,
									conf.C,
									N);
	for (unsigned int i = 0; i < conf.L; i++)
		for (unsigned int j = 0; j < conf.C; j++)
			if (eco[i][j].animal.type != EMPTY){
				if (eco[i][j].animal.type == FOX) {
					printf("RAPOSA %d %d\n", i, j);
				}
				else if (eco[i][j].animal.type == RABBIT) {
					printf("COELHO %d %d\n", i, j);
				}
				else if (eco[i][j].animal.type == ROCK) {
					printf("ROCHA %d %d\n", i, j);
				}
			}
}
