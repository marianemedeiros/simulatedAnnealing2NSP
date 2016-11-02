#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<time.h>
#include "libraries/assignment_problem.h"


 int n_nurses;
 int n_days;
 int n_shifts;

 static NspLib* nsp;
 static Constraints* c;

int cost_solution(Schedule* s){
/**	int** same_assignments = (int**) calloc(n_nurses, sizeof(int*));// matrix de enfermeiros por turnos, 
	//que será armazenado a quantidade de turnos atribuidos consecutivamente a um enfermeiro
	int** nurse_per_shift_in_day = (int**) calloc(n_days, sizeof(int*));

	int total_cost = 0;
	//percorrer a lista de nurse por dia.
	for (int n = 0; n < n_nurses; n++){
		int* shift_per_nurse = (int*) calloc(n_nurses, sizeof(int));
		List* days = s->nurse_per_day[n];
		Node* s = days->first;
		
		printf("Nurse %d - ", n);
		printList(days);
		printf("\n");
		for (int d = 0; d < days->size; d++){
			nurse_per_shift_in_day[d] = (int*) calloc(n_shifts, sizeof(int));

			if(d == 0 )
				same_assignments[n] = (int*) calloc(n_shifts,sizeof(int));
			

			int nHCV = 0, nSCV = 0;
			int shift = s->data;
			Node* next = s->next;

			if(next != NULL){
				int next_shift = next->data;
				if(verify_shift(next_shift,shift) == 1)
					nHCV++;

				if(shift == next_shift)
					same_assignments[n][shift]++;
			}

			if(shift != FREE)
				att_shift2nurse(shift_per_nurse,n);
			
			att_nurse2shift(nurse_per_shift_in_day[d],shift);
			total_cost += nsp->preference_matrix[n][(d*n_shifts)+shift] + (Ph*nHCV);

			s = s->next;
		}

		int nHCV = 0, nSCV = 0;
		for (int k = 0; k < n_shifts; k++){
			for (int d = 0; d < n_days; d++){
				if(verify_minimum_coverage(d, nsp->coverage_matrix, k, nurse_per_shift_in_day[d][k]))
				nSCV++;
			}
			
			if(verify_consecutive_assigments(same_assignments[n][k], c->consecutive_assigments_matrix[k][0], c->consecutive_assigments_matrix[k][1]) == 1)
				nHCV++;	
			
		}		

		if (verify_number_of_assigments(shift_per_nurse[n], c->number_of_assigments) == 1)
			nHCV++;

		total_cost += (Ph*nHCV) + (Ps*nSCV);

		free(shift_per_nurse);
		free(same_assignments[n]);
	}
	free(same_assignments);

	printf("total_cost: %d\n", total_cost);
	return total_cost;
	**/
	return NULL;
}




Schedule* generate_neighbor(Schedule* s){
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

void simulated_annealing(Schedule* initial_s, int t0, int tf, int n_it, double ro){
	Schedule* current_s = initial_s;
	Schedule* best_s = initial_s;
	int t = t0;

	while(t > tf){
		for (int i = 0; i < n_it; i++){
			Schedule* s_line = copy_solution(current_s);
			s_line = generate_neighbor(s_line);
			s_line->cost_solution = cost_solution(s_line);

			int delta_custo = s_line->cost_solution - current_s->cost_solution;
			if(delta_custo <= 0 || randomize(delta_custo,t) == 1)
				current_s = s_line;
		
			if(current_s->cost_solution < best_s->cost_solution)
				best_s = current_s;
		}		
	//reduz a temperatura
		t = ro * t; // fator de redução. TODO ver outras formas
	}

}

int main(){
	nsp =  readNspFile((char*)"/home/mariane/Dropbox/mestrado/MHOC/trabalho_implementacao/semDlib/files/7290.nsp");
	c = readConstrainstsFile((char*)"/home/mariane/Dropbox/mestrado/MHOC/trabalho_implementacao/semDlib/files/casos-1-8/1.gen");	
	
	n_nurses = nsp->problem_size[0];
	n_days = nsp->problem_size[1];
	n_shifts = nsp->problem_size[2];

	Schedule *m =  build_cost_matrix(nsp, c);

	//simulated_annealing(m,1,1,1,0.8);
	
	//show_multipartite_graph(m);
	//showNsp(nsp);
	//showConstraints(c);

	free(nsp);
	free(c);
}
