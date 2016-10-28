#include "assignment_problem.h"


void show_multipartite_graph(Multipartite_Graph* m){
	for (int i = 0; i < n_days; i++){
		printf("\nDay %d 	", i);
		printList(m->day_per_nurse[i]);
	}

	for (int i = 0; i < n_nurses; i++){
		printf("\nNurse %d 	", i);
		printList(m->nurse_per_day[i]);
	}
}


int verify_number_of_assigments(int shift_per_nurse, int* number_of_assigments){
	if(shift_per_nurse > number_of_assigments[0] && shift_per_nurse < number_of_assigments[1]){
		return 0;
	}else{
		return 1;
	}
}


void att_shift2nurse(int* shift_per_nurse, int i){
	shift_per_nurse[i]++;
}


int verify_minimum_coverage(int day, int** coverage_matrix, int shift, int nurse_shift){
	if(shift <= FREE && nurse_shift < coverage_matrix[day][shift])
		return 1;
	else
		return 0;
}


void att_nurse2shift(int* vet, int shift){
	if(shift == MORNING)
		vet[MORNING] = vet[MORNING] + 1;
	else if(shift == EVENING)
		vet[EVENING] = vet[EVENING] + 1;
	else if(shift == NIGHT)
		vet[NIGHT] = vet[NIGHT] + 1;
	else if(shift == FREE)		
		vet[FREE] = vet[FREE] + 1;
}


int verify_shift(int shift, int previous_shift){
	int cont = 0;
	if(previous_shift == EVENING && shift == MORNING){
		cont++;
	}
	else if(previous_shift == NIGHT && shift == MORNING){
		cont++;
	}
	else if(previous_shift == NIGHT && shift == EVENING){
		cont++;
	}
	return cont;
}


int get_shift(int element){
	if(element > MAX_SHIFTS)
		return FREE;
	return element;
}


int verify_consecutive_assigments(int consecutive_assigments, int min, int max){
	if(consecutive_assigments > max)
		return 1;
	//else if(consecutive_assigments < min)
	//	return 1;
	return 0;
}

int set_in_vector(int nurse, int shift, int minimum_coverage, int** m_cost, int** m_assigment, int** preference_matrix, int stop, int day,
	int* min_shifts, int * nurse_per_shift){
	int k = 0;
	int indice = 0;
	
	if(stop > 0)
		indice = stop;

	while(k < minimum_coverage){
		m_cost[nurse][indice] = preference_matrix[nurse][(day*n_shifts)+shift];
		m_assigment[nurse][indice] = shift;
		att_nurse2shift(nurse_per_shift, shift); // atribui um enfermeiro a um turno


		if(m_cost[nurse][indice] < min_shifts[nurse])
			min_shifts[nurse] = m_cost[nurse][indice];

		k++;
		indice++;
	}
	return k;
}

void set_minimum_coverage(int day, int nurse, int aux_mc, int aux_s, int* assignment_per_days, int** m_cost, int** m_assigment, 
	int**coverage_matrix, int** preference_matrix, int* min_shifts, int * nurse_per_shift){
	int minimum_coverage;
	m_cost[nurse] = (int*) calloc(n_nurses,sizeof(int));
	m_assigment[nurse] = (int*) calloc(n_nurses,sizeof(int));
	int line_zero = 0;
	int stop = 0;
	for (int j = 0; j < n_shifts; j++){// shifts
		//verificar a matrix de cobertura e colocar a demanda minima.
		minimum_coverage = coverage_matrix[day][j];
		if(nurse == 0 )
			assignment_per_days[day] +=  minimum_coverage;

		if(minimum_coverage != 0)
			line_zero = 1;	

		stop = set_in_vector(nurse,j,minimum_coverage, m_cost, m_assigment, preference_matrix, stop, day, min_shifts, nurse_per_shift);

	}

	if(line_zero == 0){
		assignment_per_days[day] = aux_mc;
		stop = set_in_vector(nurse,aux_s,aux_mc,m_cost, m_assigment, preference_matrix,stop, day, min_shifts, nurse_per_shift);
	}
}


int free_shifts(int min, int max, int assignment_per_day){
	int f = ((n_days - ((max-min)/2)) * n_nurses)/ n_days;

	if(f > assignment_per_day)
		f = n_nurses - assignment_per_day;
	return f;
}

Multipartite_Graph* build_cost_matrix2(NspLib* nsp, Constraints* c){
	Multipartite_Graph* m = (Multipartite_Graph*) calloc(1, sizeof(Multipartite_Graph));
	m->cost_solution = 0;

	hungarian_problem_t* p = (hungarian_problem_t*) calloc(1,sizeof(hungarian_problem_t));
	int* assignment_per_days = (int*) calloc(n_days,sizeof(int));

	List** nurse_per_day = (List**) calloc(n_nurses, sizeof(List*));
	List** day_per_nurse = (List**) calloc(n_days, sizeof(List*));
	int** same_assignments = (int**) calloc(n_nurses, sizeof(int*));// matrix de enfermeiros por turnos, 
	//que será armazenado a quantidade de turnos atribuidos consecutivamente a um enfermeiro
	int* shift_per_nurse = (int*) calloc(n_nurses, sizeof(int));// quantidade de turno por nurse

	for (int d = 0; d < n_days; d++){//days
		int** m_cost = (int**) calloc(n_nurses, sizeof(int*));
		int** m_assigment = (int**) calloc(n_nurses, sizeof(int*));

		int* min_shifts = (int*) calloc(n_nurses, sizeof(int));
		int* nurses_per_shift = (int*) calloc(n_shifts, sizeof(int));

		day_per_nurse[d] = initializeList();

		int mc = rand() % n_nurses; // usar só quando tiver uma linha só com zeros na matriz de cobertura
		int s = rand() % n_shifts; // usar só quando tiver uma linha só com zeros na matriz de cobertura

		for (int i = 0; i < n_nurses; i++){// nurse
			int nHCV = 0, nSCV = 0;
			same_assignments[i] = (int*) calloc(n_shifts,sizeof(int));
			if(d == 0)
				nurse_per_day[i] = initializeList();
			min_shifts[i] = 99999;
			set_minimum_coverage(d, i, mc, s, assignment_per_days, m_cost, m_assigment, nsp->coverage_matrix, nsp->preference_matrix, min_shifts, nurses_per_shift);
			
			if(d != 0){
				for (int s = 0; s < assignment_per_days[d]; s++){
					if(verify_shift(m_assigment[i][s],p->assignment_vector[i]) == 1)
						nHCV++;
					
					if(m_assigment[i][s] == p->assignment_vector[i])
						same_assignments[i][p->assignment_vector[i]]++;
						
					if(verify_consecutive_assigments(same_assignments[i][p->assignment_vector[i]], c->consecutive_assigments_matrix[0][p->assignment_vector[i]], c->consecutive_assigments_matrix[1][p->assignment_vector[i]]) == 1)
						nHCV++;							

					if (verify_number_of_assigments(shift_per_nurse[i], c->number_of_assigments) == 1)
						nHCV++;

					if(verify_minimum_coverage(d, nsp->coverage_matrix, m_assigment[i][s], nurses_per_shift[m_assigment[i][s]]))
						nSCV++;
						
					m_cost[i][s] = Ph * nHCV + Ps * nSCV;

					if(m_cost[i][s] < min_shifts[i])
						min_shifts[i] = m_cost[i][s];

				}

			}

			//block II
			//atribui folgas

			// completa com menor valor do bloco I
			for (int k = assignment_per_days[d]; k < n_nurses; k++){
				m_cost[i][k] = min_shifts[i];
			}

			/*if(d == 0){
				for (int k = 0; k < n_nurses; k++)
					printf(" %d ", m_cost[i][k]);
				printf("\n");
			}
			*/
			free(same_assignments[i]);
		}

		//hungarian algorithm
		hungarian_free(p);
		p = (hungarian_problem_t*) calloc(1,sizeof(hungarian_problem_t));
		
		hungarian_init(p, m_cost , m_assigment, n_nurses,n_nurses, HUNGARIAN_MODE_MINIMIZE_COST) ;
		hungarian_solve(p,nurse_per_day, day_per_nurse[d],shift_per_nurse, m_cost);
		
		//hungarian_print_assignment_vector(p->assignment_vector, n_nurses, d);
		//hungarian_print_assignment(p);

		
		free(min_shifts);
		for (int i = 0; i < n_nurses; i++){
			free(m_cost[i]);
			free(m_assigment[i]);
		}
		free(m_cost);
		free(m_assigment);
		free(nurses_per_shift);

	}

	free(same_assignments);
	free(assignment_per_days);
	free(shift_per_nurse);

	m->nurse_per_day = nurse_per_day;
	m->day_per_nurse = day_per_nurse;
	return m;
}

