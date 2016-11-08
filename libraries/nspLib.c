#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "strings.h"

extern int n_nurses;
extern int n_days;
extern int n_shifts;

typedef struct constraints{
	int *problem_size;
	int *number_of_assigments;
	int *consecutive_working_shifts;
	int **consecutive_assigments_matrix;// consecutive same working shifts and number of assigments per shift
}Constraints;

typedef struct nspLib{
	int *problem_size;
	int **coverage_matrix; // alocar dinamicamente de acordo com o tamanho do problema (problem_size)
	int **preference_matrix; // alocar dinamicamente de acordo com a quantidade de enfermeiros(linhas) x (numero de dias*qnt de turnos) (colunas)
}NspLib;

void freeConstraints(Constraints* c){
	free(c->problem_size);
	free(c->number_of_assigments);
	free(c->consecutive_working_shifts);

	for (int i = 0; i < n_shifts; i++){
		free(c->consecutive_assigments_matrix[i]);
	}
	free(c->consecutive_assigments_matrix);
}

void freeNsp(NspLib* nsp){
	free(nsp->problem_size);

	for (int i = 0; i < n_days; i++){
		free(nsp->coverage_matrix[i]);
	}
	//free(nsp->coverage_matrix);

	for (int i = 0; i < (n_days*n_shifts); i++){
		free(nsp->preference_matrix[i]);
	}
	//free(nsp->preference_matrix);
}

void showVector(int *v){
	for (int i = 0; i < 4; ++i){
		printf("%d ", *(v+i));
	}
	printf("\n");
}

void readLine(char* line, int* vector){
	int j = 0, s = 0, x = 0;
	
	for (int i = 0; i < strlen(line); i++){
		if(line[i] == TAB){
			vector[x] = strncopy(line,j,i);
			x++;
			s = i;
			j = i + 1;
		}
	}

	//copy from the last (space + 1) until the last charecter
	vector[x] = strncopy(line,s+1,strlen(line)-1);
	x++;
//	return vector;
}

NspLib* readNspFile(char* url){
NspLib* nsp = (NspLib*) calloc(1, sizeof(NspLib));

int index_c_m = 0; // index coverage matrix
int index_p_m = 0; // index preference matrix

FILE* arq = fopen(url,"rt");
char* line = (char*) calloc(1000, sizeof(char));
int n_line = 0;
	if(!arq){
	     perror("Can't open file.");
 		exit(1);			
	}else{
		while(!feof(arq)){
			fgets(line, 255, arq);

			if(n_line == 0){ // first line. (problem size matix)
				nsp->problem_size = (int*) calloc(3, sizeof(int));
				readLine(line, nsp->problem_size);

				nsp->preference_matrix = (int**) calloc(nsp->problem_size[0],sizeof(int*));
				nsp->coverage_matrix = (int**) calloc(nsp->problem_size[1],sizeof(int*));
			}

			if(n_line >= 2 && n_line <= nsp->problem_size[1]+1){// coverage matrix
				nsp->coverage_matrix[index_c_m] = (int*) calloc(nsp->problem_size[2], sizeof(int));
				readLine(line, nsp->coverage_matrix[index_c_m]);
				index_c_m++;
			}
			

			if(n_line >= (nsp->problem_size[1]+3)){
				nsp->preference_matrix[index_p_m] = (int*) calloc((nsp->problem_size[1]*nsp->problem_size[2]), sizeof(int));
				readLine(line, nsp->preference_matrix[index_p_m]);
				index_p_m++;
			}
   			n_line++;
		}

	    fclose(arq);
		//printf("Success read nsp file!\n\n");
     }
     free(line);
     return nsp;
}

Constraints* readConstrainstsFile(char* url){

FILE* arq = fopen(url,"rt");
char line[1000];
int n_line = 0, index_ca_m = 0;
Constraints *c = (Constraints*) calloc(1, sizeof(Constraints));

	if(!arq){
	     perror("Can't open file.");
 		exit(1);			
	}else{
		while(!feof(arq)){
			fgets(line, 255, arq);
			
			if(n_line == 0){ // problem size
				c->problem_size = (int*) calloc(2,sizeof(int));
				c->problem_size[0] = strncopy(line,0,0);
				c->problem_size[1] = strncopy(line,2,2);
				
				c->consecutive_assigments_matrix = (int**) calloc(c->problem_size[1],sizeof(int*));
			}

			if(n_line == 2){ // number of assigments (min and max)
				c->number_of_assigments = (int*) calloc(2,sizeof(int));
				c->number_of_assigments[0] = strncopy(line,0,0);
				c->number_of_assigments[1] = strncopy(line,2,2);
			}

			if(n_line == 4){ // consecutive working shifts (min nd max)
				c->consecutive_working_shifts = (int*) calloc(2,sizeof(int));
				c->consecutive_working_shifts[0] = strncopy(line,0,0);
				c->consecutive_working_shifts[1] = strncopy(line,2,2);
			}

			if(n_line >= 6 && n_line < (6+c->problem_size[1])){ 
				c->consecutive_assigments_matrix[index_ca_m] = (int*) calloc(4, sizeof(int));
				//readLine(line, c->consecutive_assigments_matrix[index_ca_m]);
				c->consecutive_assigments_matrix[index_ca_m][0] = strncopy(line,0,0);
				c->consecutive_assigments_matrix[index_ca_m][1] = strncopy(line,2,2);
				c->consecutive_assigments_matrix[index_ca_m][2] = strncopy(line,4,4);
				c->consecutive_assigments_matrix[index_ca_m][3] = strncopy(line,6,6);

				index_ca_m++;
			}
			
			n_line++;
		}


	    fclose(arq);
	    //printf("Success read constraints file!\n\n");
	    return c;
     }
}

void showConstraints(Constraints* c){
	printf("Problem Size: %d , %d \n", c->problem_size[0], c->problem_size[1]);
	printf("Number of assigments: %d , %d \n", c->number_of_assigments[0], c->number_of_assigments[1]);
	printf("Consecutive working shifts: %d , %d \n", c->consecutive_working_shifts[0], c->consecutive_working_shifts[1]);

	for (int i = 0; i < c->problem_size[1]; ++i){
		for (int j = 0; j < 4; j++){
			printf("%d ", c->consecutive_assigments_matrix[i][j]);
		}
		printf("\n");
	}
}

void showNsp(NspLib* nsp){
	printf("Problem Size: %d , %d, %d \n", nsp->problem_size[0], nsp->problem_size[1], nsp->problem_size[2]);
	
	printf("Coverage Matrix\n");
	for (int i = 0; i < nsp->problem_size[1]; ++i){
		for (int j = 0; j < nsp->problem_size[2]; j++){
			printf("%d ", nsp->coverage_matrix[i][j]);
		}
		printf("\n");
	}	

	printf("Preference Matrix\n");
	for (int i = 0; i < nsp->problem_size[0]; ++i){
		for (int j = 0; j < (nsp->problem_size[2]*nsp->problem_size[1]); j++){
			printf("%d ", nsp->preference_matrix[i][j]);
		}
		printf("\n");
	}
}



