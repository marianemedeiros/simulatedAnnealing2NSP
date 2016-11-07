#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<time.h>
#include <dirent.h> 
#include "libraries/assignment_problem.h"

#define PERMUTATIONS 1

 int n_nurses = 25;
 int n_days = 7;
 int n_shifts = 4;
 
void free_schedule(Schedule* s){
	for (int i = 0; i < n_days; i++){
		free(s->day_per_nurse[i]);
	}
	free(s->day_per_nurse);

	for (int i = 0; i < n_nurses; i++){
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
	free(shift_per_nurse);
	
	for (int i = 0; i < n_days; i++){
		free(minimum_coverage[i]);
	}
	free(minimum_coverage);

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
	int k = 1;

	while(k <= neighbor_struct){
		s_2line = generate_neighbor(s_2line, PERMUTATIONS);
		s_2line->cost_solution = cost_solution(s_2line, c, nsp);

		if(s_2line->cost_solution < s_line->cost_solution){
			free_schedule(s_line);
			s_line = (Schedule*) calloc(1, sizeof(Schedule));
			s_line = s_2line;
			k = 1;
		}else
			k++;
	}
	return s_line;
}

Schedule* simulated_annealing(Schedule* initial_s, int t0, int tf, int n_it, double ro, int use_vnd, Constraints* c, NspLib* nsp){
	printf("Initial temperature: %d; Final temperature: %d; Iterations: %d, VND: %d\n", t0,tf, n_it, use_vnd);
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
				//printf("depois do if\n");
				Schedule* s_2line = vnd(s_line, 1000,c,nsp);
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
//				printf("free best:::: %d\n", best_s->cost_solution);
				free_schedule(best_s);
				//best_s = (Schedule*) calloc(1, sizeof(Schedule));
				best_s = copy_solution(current_s);
				
				printf("-- Cost: %d (it: %d, t: %d)\n", best_s->cost_solution, i, t);
				//show_multipartite_graph(best_s);
			}
		}	
		//reduz a temperatura
		t = ro * t; // fator de redução.
	}
	return best_s;
}

void readDir(){
	Constraints* c1 = readConstrainstsFile((char*)"/home/mariane/Dropbox/mestrado/MHOC/trabalho_implementacao/semDlib/files/casos-1-8/1.gen");
	Constraints* c2 = readConstrainstsFile((char*)"/home/mariane/Dropbox/mestrado/MHOC/trabalho_implementacao/semDlib/files/casos-1-8/2.gen");
	Constraints* c3 = readConstrainstsFile((char*)"/home/mariane/Dropbox/mestrado/MHOC/trabalho_implementacao/semDlib/files/casos-1-8/3.gen");
	Constraints* c4 = readConstrainstsFile((char*)"/home/mariane/Dropbox/mestrado/MHOC/trabalho_implementacao/semDlib/files/casos-1-8/4.gen");
	Constraints* c5 = readConstrainstsFile((char*)"/home/mariane/Dropbox/mestrado/MHOC/trabalho_implementacao/semDlib/files/casos-1-8/5.gen");
	Constraints* c6 = readConstrainstsFile((char*)"/home/mariane/Dropbox/mestrado/MHOC/trabalho_implementacao/semDlib/files/casos-1-8/6.gen");
	Constraints* c7 = readConstrainstsFile((char*)"/home/mariane/Dropbox/mestrado/MHOC/trabalho_implementacao/semDlib/files/casos-1-8/7.gen");
	Constraints* c8 = readConstrainstsFile((char*)"/home/mariane/Dropbox/mestrado/MHOC/trabalho_implementacao/semDlib/files/casos-1-8/8.gen");

	DIR *d;
  	struct dirent *dir;
  	char url[100] = "/home/mariane/Dropbox/mestrado/MHOC/trabalho_implementacao/semDlib/files/teste/";

  	d = opendir("/home/mariane/Dropbox/mestrado/MHOC/trabalho_implementacao/semDlib/files/teste/");
  	if (d){
    	while ((dir = readdir(d)) != NULL){
    		if(strstr(dir->d_name, "nsp") != NULL){
    			char* aux = (char*) calloc(256,sizeof(char));
    			strcat(aux, url);
    			strcat(aux, dir->d_name);
    			printf("%s\n", aux);
    			
    			NspLib* nsp =  readNspFile(aux);
    			//Constraints* c = readConstrainstsFile((char*)"/home/mariane/Dropbox/mestrado/MHOC/trabalho_implementacao/semDlib/files/casos-1-8/1.gen");	
				
				//build matrix
				Schedule *m =  build_cost_matrix(nsp, c1);
				m->cost_solution = cost_solution(m, c1, nsp);
				printf("Initial Cost: %d\n", m->cost_solution);
				
				//simulated
				Schedule *rt = simulated_annealing(m,100,1,10,0.8,0, c1, nsp);	
				show_multipartite_graph(rt);

				//freeNsp(nsp);
				free(nsp);
				free(aux);
				//free_schedule(m);
				//free_schedule(rt);
    		}
    	}
		closedir(d);
  	}
}


int main(){
	//readDir();
	
	NspLib* nsp =  readNspFile((char*)"/home/mariane/Documents/simulatedAnnealing2NSP/files/7290.nsp");
	Constraints* c = readConstrainstsFile((char*)"/home/mariane/Documents/simulatedAnnealing2NSP/files/casos-1-8/1.gen");	
	

	Schedule *m =  build_cost_matrix(nsp, c);
	m->cost_solution = cost_solution(m, c, nsp);
	printf("Initial Cost: %d\n", m->cost_solution);
	
	Schedule *rt = simulated_annealing(m,10000,1,1000,0.8,1, c, nsp);	
	show_multipartite_graph(rt);

	
	//show_multipartite_graph(m);
	//showNsp(nsp);
	//showConstraints(c);

	//free(nsp);
	//free(c);
}
