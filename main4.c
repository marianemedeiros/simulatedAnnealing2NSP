#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<time.h>
#include <dirent.h> 
#include "libraries/assignment_problem.h"

int PERMUTATIONS;
int K;
int temp;
int finalTemp;
int it;
double reduction;
int vns;


int n_nurses = 25;
int n_days = 7;
int n_shifts = 4;
 
Constraints* c4;

void free_schedule(Schedule* s){
	for (int i = 0; i < n_days; i++){
		freeList(s->day_per_nurse[i]);
		free(s->day_per_nurse[i]);
	}
	free(s->day_per_nurse);

	for (int i = 0; i < n_nurses; i++){
		freeList(s->nurse_per_day[i]);
		free(s->nurse_per_day[i]);
	}
	free(s->nurse_per_day);
	free(s);
}


int** shifts_per_day(List** day_per_nurse){
	int** rt = (int**) calloc(n_days, sizeof(int*));
	
	for (int d = 0; d < n_days; d++){
		rt[d] = (int*) calloc(n_shifts, sizeof(int));
		List* nurses = day_per_nurse[d];
		Node* e = nurses->first;

		while(e != NULL){
			if(e->data == MORNING)
				rt[d][MORNING]++;
			if(e->data == EVENING)
				rt[d][EVENING]++;
			if(e->data == NIGHT)
				rt[d][NIGHT]++;
			if(e->data == FREE)
				rt[d][FREE]++;
			e = e->next;	
		}
	}
	return rt;
}

int* shifts_per_nurse(List** nurse_per_day){
	int	*rt = (int*) calloc(n_nurses, sizeof(int));

	for (int i = 0; i < n_nurses; i++){
		Node* aux = nurse_per_day[i]->first;
		while(aux != NULL){
			if(aux->data != FREE)
				rt[i]++;
			aux = aux->next;
		}
	}
	return rt;
}

//nurse_per_day - dia esta na coluna
//day_per_nurse - nurse esta na coluna 
int cost_solution(Schedule* s, Constraints* c, NspLib* nsp){
	int** minimum_coverage = shifts_per_day(s->day_per_nurse);
	int** same_assignments = (int**) calloc(n_nurses, sizeof(int*));
	int* shift_per_nurse = shifts_per_nurse(s->nurse_per_day);

	int total_cost = 0;
	
	for (int d = 0; d < n_days; d++){
		List* day_schedule = s->day_per_nurse[d];
		Node* nurse = day_schedule->first;
		int index_n = 0; int r = 0, r1 = 0, cost = 0;
		while(nurse != NULL){
			if(d == 0)
				same_assignments[index_n] = (int*) calloc(n_shifts, sizeof(int));

			int nHCV = 0, nSCV = 0;

			//verify next shift
			if(d != (n_days-1)){
				int next_shift = getElementByIndex(s->day_per_nurse[d+1],index_n);
				
				if(verify_shift(next_shift,nurse->data) == 1)
					nHCV++;
				
				if(nurse->data == next_shift)
						same_assignments[index_n][nurse->data]++;
			}

			if(verify_consecutive_assigments(same_assignments[index_n][nurse->data], c->consecutive_assigments_matrix[nurse->data][0], c->consecutive_assigments_matrix[nurse->data][1]) == 1)
				nHCV++;							

			//percorre o vetor verificando quantos turnos tem a nurse e depois verifica se a qntd quebra a regra ou nao
			if (verify_number_of_assigments(shift_per_nurse[index_n], c->number_of_assigments) == 1)
				nHCV++;

			if(verify_minimum_coverage(d, nsp->coverage_matrix, nurse->data, minimum_coverage[d][nurse->data]))
				nSCV++;
			r = nHCV;
			r1 = nSCV;

			if(nHCV != 0 || nSCV != 0)
				cost = nsp->preference_matrix[index_n][(d*n_shifts)+nurse->data] + Ph * nHCV + Ps * nSCV;

			index_n++;
			nurse = nurse->next;
		}
					total_cost+= cost;

		//printf("day: %d; cost: %d (nHCV: %d; nSCV: %d)\n", d, cost, r, r1);
	}
	//printf("Total cost: %d\n", total_cost);

	for (int i = 0; i < n_nurses; ++i){
		free(same_assignments[i]);
	}
	free(same_assignments);

	for (int i = 0; i < n_days; i++){
		free(minimum_coverage[i]);
	}
	free(minimum_coverage);
	free(shift_per_nurse);

	s->cost_solution = total_cost;
	return total_cost;
}




Schedule* generate_neighbor(Schedule* s, int changes){
	for (int i = 0; i < changes; i++){
		time_t t;
		srand((unsigned) time(&t));

		int nurse1 = rand() % n_nurses;
		int nurse2 = rand() % n_nurses;
		int day = rand() % n_days;

		int e1 = getElementByIndex(s->nurse_per_day[nurse1], day);
		int e2 = getElementByIndex(s->nurse_per_day[nurse2], day);

		setList(s->nurse_per_day[nurse1],e2,day);
		setList(s->nurse_per_day[nurse2],e1,day);

		setList(s->day_per_nurse[day],e2,nurse1);
		setList(s->day_per_nurse[day],e1,nurse2);
	}
	return s;
}

Schedule* copy_solution(Schedule* s){
	Schedule* rt = (Schedule*) calloc(1,sizeof(Schedule));
	
	List** nurse_per_day = (List**) calloc(n_nurses, sizeof(List*));
	List** day_per_nurse = (List**) calloc(n_days, sizeof(List*));
	
	for (int i = 0; i < n_nurses; i++){
		nurse_per_day[i] = (List*) calloc(n_days, sizeof(List));
		nurse_per_day[i] = copyList(s->nurse_per_day[i], nurse_per_day[i]);	
	}
	
	for (int i = 0; i < n_days; i++){
		day_per_nurse[i] = (List*) calloc(n_nurses, sizeof(List));
		day_per_nurse[i] = copyList(s->day_per_nurse[i], day_per_nurse[i]);	
	}

	rt->nurse_per_day = nurse_per_day;
	rt->day_per_nurse = day_per_nurse;
	rt->cost_solution = s->cost_solution;

	return rt;
}

double randomize(int delta_custo, int temperature){
	srand (time(NULL));
	if((rand()/(double)(RAND_MAX)) < exp((-delta_custo)/temperature))
		return 1;

	return 0;
}

Schedule* vnd(Schedule* s, int neighbor_struct, Constraints* c, NspLib* nsp){
	Schedule *s_line = copy_solution(s);
	Schedule *s_2line = copy_solution(s);
	int aux = 0;
	int k = 1;

	while(k <= neighbor_struct){
		s_2line = generate_neighbor(s_2line, PERMUTATIONS);
		s_2line->cost_solution = cost_solution(s_2line, c, nsp);

		if(s_2line->cost_solution < s_line->cost_solution){
			aux = 1;
			free_schedule(s_line);
			s_line = (Schedule*) calloc(1, sizeof(Schedule));
			s_line = s_2line;
			k = 1;
		}else
			k++;
	}
	
	if(aux == 0)
		free_schedule(s_2line);

	return s_line;
}

Schedule* simulated_annealing(Schedule* initial_s, int t0, int tf, int n_it, double ro, int use_vnd, Constraints* c, NspLib* nsp){
	printf("Initial temperature: %d; Final temperature: %d; Iterations: %d, VND: %d, Vizinhança do VND: %d\n", t0,tf, n_it, use_vnd, K);
	Schedule* current_s = copy_solution(initial_s);
	Schedule* best_s = copy_solution(initial_s);;
	int t = t0;
	while(t > tf){
		for (int i = 0; i < n_it; i++){
			Schedule* s_line = copy_solution(current_s);
			s_line = generate_neighbor(s_line, PERMUTATIONS);
			s_line->cost_solution = cost_solution(s_line, c, nsp);
			int delta_custo = 0;
			

			if(use_vnd == 1){
				Schedule* s_2line = vnd(s_line, K ,c,nsp);
				delta_custo = s_2line->cost_solution - current_s->cost_solution;
				free_schedule(s_2line);
			}else{
				delta_custo = s_line->cost_solution - current_s->cost_solution;
			}

			if(delta_custo < 0 || randomize(delta_custo,t) == 1){
				free_schedule(current_s);
				//current_s = (Schedule*) calloc(1, sizeof(Schedule));
				current_s = copy_solution(s_line);

			}
				
			if(current_s->cost_solution < best_s->cost_solution){
				//printf("free best:::: %d\n", best_s->cost_solution);
				free_schedule(best_s);
				//best_s = (Schedule*) calloc(1, sizeof(Schedule));
				best_s = copy_solution(current_s);
				
				printf("-- Cost: %d (it: %d, t: %d)\n", best_s->cost_solution, i, t);
			
					//show_multipartite_graph(best_s);
			}
		free_schedule(s_line);
		}

		//reduz a temperatura
		t = ro * t; // fator de redução.
	}
	free_schedule(current_s);
	return best_s;
}

void saveDatas(char* name, char* name1, char* constraint, int temp, int finalTemp, int it, double reduction, int vns, Schedule* s, int initial_cost){
   FILE *fp;

   fp = fopen(name, "a+");

   if(fp == NULL)
   	printf("nao abriu\n");

   //fprintf(fp, "Constraints File;Temperature;Final Temp;Iterations;Reduction Rate;VNS;K;Initial cost;Final Cost\n", constraint, temp, finalTemp, it, reduction, vns, K,initial_cost, s->cost_solution);
   fprintf(fp, "%s,%s;%d;%d;%d;%f;%d;%d;%d;%d\n", name1,constraint, temp, finalTemp, it, reduction, vns, K, initial_cost, s->cost_solution);
   	fclose(fp);
}

void saveSchedule(char* name, char* constraint, int temp, int finalTemp, int it, double reduction, int vns, Schedule* s, int initial_cost){
   FILE *fp;

   fp = fopen(name, "a+");

   if(fp == NULL)
   	printf("nao abriu1\n");

   fprintf(fp, "\nConstraints File: %s; Temperature: %d, Final Temp: %d, Iterations: %d, Reduction Rate: %f, VNS: %d, K: %d\n", constraint, temp, finalTemp, it, reduction, vns, K);

	for (int i = 0; i < n_days; i++){
		fprintf(fp, "Day %d 	", i);
		Node* node = s->day_per_nurse[i]->first;
		while(node != NULL){
			fprintf(fp, "%d ", node->data);
			node = node->next;
		}
		fprintf(fp, "\n");
	}
	fprintf(fp, "Initial Cost: %d -- Final cost: %d ", initial_cost, s->cost_solution);
   	fclose(fp);
}

void readParams(){

FILE* arq = fopen("params.txt","rt");
char* line = (char*) calloc(1000, sizeof(char));
int i = 0;
	if(!arq){
	     perror("Can't open file.");
 		exit(1);			
	}else{
		while(!feof(arq)){
			fgets(line, 1000, arq);
			char* token = strtok(line, ":");

			if(i == 0)
				temp = atoi(token);
			if(i == 1)
				finalTemp = atoi(token);
			if(i == 2)
				it = atoi(token);
			if(i == 3)
				reduction = atof(token);
			if(i == 4)
				vns = atoi(token);
			if(i == 5)
				K = atoi(token);
			if(i == 6)
				PERMUTATIONS = atoi(token);

			i++;
		}
	}
free(line);
}

int main(){
	readParams();
	c4 = readConstrainstsFile((char*)"files/casos-1-8/4.gen");
	char name1[2] = "4";

	char* saveAt_2 = (char*) calloc(256,sizeof(char));
	saveAt_2 = strcat(saveAt_2, "resultados/resuts4.cvs");

	DIR * directory;
	struct dirent *dir;
	directory = opendir("./files/N25");
	if(directory){
		dir = readdir(directory);
		while( dir != NULL){
	        if (!strcmp (dir->d_name, ".") || !strcmp (dir->d_name, "..") ){
				dir = readdir(directory);
	            continue;
	        }	
	        
	        NspLib* nsp =  readNspFile(dir->d_name);

	        Schedule *m =  build_cost_matrix(nsp, c4);
			m->cost_solution = cost_solution(m, c4, nsp);
				
			Schedule* rt = simulated_annealing(m,temp,finalTemp,it,reduction,vns, c4, nsp);	

			char* saveAt = (char*) calloc(256,sizeof(char));
			strcat(saveAt, "resultados/");
			strcat(saveAt, dir->d_name);
			
			saveSchedule(saveAt, name1, temp,finalTemp,it,reduction,vns,rt, m->cost_solution);
			saveDatas(saveAt_2, saveAt, name1, temp,finalTemp,it,reduction,vns,rt,m->cost_solution);
			
			free(saveAt);
			free_schedule(rt);
			free_schedule(m);   			

	        freeNsp(nsp);
	        dir = readdir(directory);
		}
	}


	freeConstraints(c4);
}