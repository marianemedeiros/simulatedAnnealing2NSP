#include "assignment_problem.h"

void show_multipartite_graph(Schedule* m){
	for (int i = 0; i < n_days; i++){
		printf("\nDay %d 	", i);
		printList(m->day_per_nurse[i]);
	}

	for (int i = 0; i < n_nurses; i++){
		printf("\nNurse %d 	", i);
		printList(m->nurse_per_day[i]);
	}
}

void print_cost_m(int** m_cost, int** m_assigment){
	printf("\n Cost matrix\n");
	for (int i = 0; i < n_nurses; i++){
		for (int j = 0; j < n_nurses; j++){
			printf("%d ", m_cost[i][j]);			
		}
		printf("\n");	
	}

	printf("\n Assignment matrix\n");
	for (int i = 0; i < n_nurses; i++){
		for (int j = 0; j < n_nurses; j++){
			printf("%d ", m_assigment[i][j]);			
		}
	printf("\n");	
	}
}

int number_free_shift(int min, int max){
	return ((n_days - ((min + max)/2)) * n_nurses)/n_days;
}

void set_minimum_coverage(int* turnos_nurse, int day, int nurse, int** m_cost, int** m_cost2, int** m_assigment, 
	int**coverage_matrix, int** preference_matrix, int* min_shifts, int * nurse_per_shift, int* min_att){
	int minimum_coverage;
	m_cost[nurse] = (int*) calloc(n_nurses,sizeof(int));
	m_cost2[nurse] = (int*) calloc(n_nurses,sizeof(int));
	m_assigment[nurse] = (int*) calloc(n_nurses,sizeof(int));
	int stop = 0;
	for (int j = 0; j < n_shifts; j++){// shifts
		//verificar a matrix de cobertura e colocar a demanda minima.
		minimum_coverage = coverage_matrix[day][j];

		int k = 0, indice = 0;
		if(stop > 0)
			indice = stop;

		while(k < minimum_coverage){
			m_cost[nurse][indice] = preference_matrix[nurse][(day*n_shifts)+j];
			m_cost2[nurse][indice] = preference_matrix[nurse][(day*n_shifts)+j];
			m_assigment[nurse][indice] = j;
			att_nurse2shift(nurse_per_shift, j); // atribui um enfermeiro a um turno
			(*turnos_nurse)++;
			if(m_cost[nurse][indice] < min_shifts[nurse]){
				min_shifts[nurse] = m_cost[nurse][indice];
				min_att[nurse] = m_assigment[nurse][indice];
			}

			k++;
			indice++;
		}
		stop = indice;
	}
}

void assignment_free_shifts(int* turnos_nurse, int free_shifts, int d, int i, int** m_cost, int** m_cost2, int** preference_matrix,
int** m_assigment, int* nurses_per_shift, int* min_shifts, int* min_att){
	int f = 0;
	while(f < free_shifts && *turnos_nurse < n_nurses){
		m_cost[i][*turnos_nurse] = preference_matrix[i][(d*n_shifts)+FREE];
		m_cost2[i][*turnos_nurse] = preference_matrix[i][(d*n_shifts)+FREE];
		
		m_assigment[i][*turnos_nurse] = FREE;
		att_nurse2shift(nurses_per_shift, FREE); // atribui um enfermeiro a um turno
				
		if(m_cost[i][*turnos_nurse] < min_shifts[i]){
			min_shifts[i] = m_cost[i][*turnos_nurse];
			min_att[i] = m_assigment[i][*turnos_nurse];
		}

		(*turnos_nurse)++;
		f++;	
	}
}

void assignment_min_shifts(int turnos_nurse, int d, int i, int** m_cost, int** m_cost2, int** preference_matrix,
int** m_assigment, int* nurses_per_shift, int* min_att){
	while(turnos_nurse < n_nurses){
		m_cost[i][turnos_nurse] = preference_matrix[i][(d*n_shifts)+min_att[i]];
		m_cost2[i][turnos_nurse] = preference_matrix[i][(d*n_shifts)+min_att[i]];
		m_assigment[i][turnos_nurse] = min_att[i];
		att_nurse2shift(nurses_per_shift, min_att[i]); // atribui um enfermeiro a um turno
			
		turnos_nurse++;
	}		
}

int combine_schedule(int* shift_per_nurse, int** m_cost, int* assignment_vector, int** m_assigment, List** nurse_per_day, List** day_per_nurse, int day, int* schedule_day){
	int cost = 0;
	for (int i = 0; i < n_nurses; i++){
		schedule_day[i] = m_assigment[i][assignment_vector[i]];
		shift_per_nurse[i]++;
		cost += m_assigment[i][assignment_vector[i]];
		addLastList(nurse_per_day[i], m_assigment[i][assignment_vector[i]]);
      	addLastList(day_per_nurse[day], m_assigment[i][assignment_vector[i]]);
	}
	return cost;
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

int verify_consecutive_assigments(int consecutive_assigments, int min, int max){
	if(consecutive_assigments > max)
		return 1;
	else if(consecutive_assigments < min)
		return 1;
	return 0;
}

int verify_number_of_assigments(int shift_per_nurse, int* number_of_assigments){
	if(shift_per_nurse > number_of_assigments[0] && shift_per_nurse < number_of_assigments[1]){
		return 0;
	}else{
		return 1;
	}
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

Schedule* build_cost_matrix(NspLib* nsp, Constraints* c){
	Schedule* s = (Schedule*) calloc(1, sizeof(Schedule));
	int* schedule_day = (int*) calloc(n_nurses, sizeof(int));

	List** nurse_per_day = (List**) calloc(n_nurses, sizeof(List*));
	List** day_per_nurse = (List**) calloc(n_days, sizeof(List*));
	int schedule_cost = 0;
	int* shift_per_nurse = (int*) calloc(n_nurses, sizeof(int));// quantidade de turno por nurse

	for (int d = 0; d < n_days; d++){//days
		day_per_nurse[d] = initializeList();
		int** m_cost = (int**) calloc(n_nurses, sizeof(int*));
		int** m_cost2 = (int**) calloc(n_nurses, sizeof(int*));
		int** m_assigment = (int**) calloc(n_nurses, sizeof(int*));

		int* min_shifts = (int*) calloc(n_nurses, sizeof(int));
		int* min_att = (int*) calloc(n_nurses, sizeof(int));

		int* nurses_per_shift = (int*) calloc(n_shifts, sizeof(int));

		for (int i = 0; i < n_nurses; i++){// nurse
			int* same_assignments = (int*) calloc(n_shifts,sizeof(int));

			int turnos_nurse = 0;
			if(d == 0)
				nurse_per_day[i] = initializeList();
			min_shifts[i] = 99999;
			set_minimum_coverage(&turnos_nurse, d, i, m_cost, m_cost2, m_assigment, nsp->coverage_matrix, nsp->preference_matrix, min_shifts, nurses_per_shift, min_att);
			
			// colocar quantidade de turno frees
			int free_shifts = number_free_shift(c->number_of_assigments[0], c->number_of_assigments[1]);
			
			assignment_free_shifts(&turnos_nurse, free_shifts, d, i, m_cost, m_cost2, nsp->preference_matrix, m_assigment, nurses_per_shift, min_shifts, min_att);
			
			//completar com o turno de menor custo o que sobrar sem nada alocado
			if(turnos_nurse < n_nurses)
				assignment_min_shifts(turnos_nurse, d, i, m_cost, m_cost2, nsp->preference_matrix, m_assigment, nurses_per_shift, min_att);
		
			// calcular as quebras as restrições.
			if(d != 0){
				int nHCV = 0, nSCV = 0;
				//hard 1: turnos seguidos que sao proibidos
				for (int j = 0; j < n_nurses; j++){
					nHCV = 0; nSCV = 0;
					if(verify_shift(m_assigment[i][j], schedule_day[i]) == 1){
						nHCV++;
						//printf("H1: Dia %d - Nurse %d quebrou %d vezes. (na combinação: %d -- %d) (j:%d)\n",d, i, nHCV, schedule_day[i], m_assigment[i][j], j);
					}

					if(m_assigment[i][j] == schedule_day[i])
						same_assignments[schedule_day[i]]++;
					

					if(verify_consecutive_assigments(same_assignments[schedule_day[i]], c->consecutive_assigments_matrix[schedule_day[i]][0], c->consecutive_assigments_matrix[schedule_day[i]][1]) == 1){
						nHCV++;							
						//printf("H2: Dia %d - Nurse %d quebrou restrição H2. (i:%d) (j:%d)\n",d, i, i,j);
					}

					if (verify_number_of_assigments(shift_per_nurse[i], c->number_of_assigments) == 1)
						nHCV++;

					if(verify_minimum_coverage(d, nsp->coverage_matrix, m_assigment[i][j], nurses_per_shift[m_assigment[i][j]]))
						nSCV++;

					if(nHCV != 0)
						m_cost[i][j] += Ph * nHCV + Ps * nSCV;
				}

			}
			free(same_assignments);
		}

		//hungarian algorithm
		hungarian_problem_t *p = (hungarian_problem_t*) calloc(1,sizeof(hungarian_problem_t));
		
		hungarian_init(p, m_cost2 , n_nurses,n_nurses, HUNGARIAN_MODE_MINIMIZE_COST) ;
		hungarian_solve(p);
		
		free(schedule_day);
		schedule_day = (int*) calloc(n_nurses, sizeof(int));
		int c = combine_schedule(shift_per_nurse, m_cost, p->assignment_vector, m_assigment, nurse_per_day, day_per_nurse, d, schedule_day);

		printf("Dia %d - ", d);
		printList(day_per_nurse[d]);
		printf(" - cost %d \n", c);
		//print_cost_m(m_cost, m_assigment);
		schedule_cost += c;
		
		free(min_shifts);
		free(min_att);
		for (int i = 0; i < n_nurses; i++){
			free(m_cost[i]);
			free(m_cost2[i]);
			free(m_assigment[i]);
		}
		free(m_cost);
		free(m_cost2);
		free(m_assigment);
		free(nurses_per_shift);
	}

	free(shift_per_nurse);
	printf("Schedule cost: %d\n", schedule_cost);
	
	s->nurse_per_day = nurse_per_day;
	s->day_per_nurse = day_per_nurse;
	s->cost_solution = schedule_cost;

	return s;
}