#include "operators.h"

int verify_minimum_coverage1(int* coverage_matrix, int* minimum_coverage){
	int r = 0;
	if(minimum_coverage[0] < coverage_matrix[0])
		r++;
	if(minimum_coverage[1] < coverage_matrix[1])
		r++;
	if(minimum_coverage[2] < coverage_matrix[2])
		r++;
	if(minimum_coverage[3] < coverage_matrix[3])
		r++;

	return r;
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

 //turnos sequencias proibidos e att consecutivas
int sequencial_shifts(List* nurse_per_day, int* vet){
	int cont= 0;
		Node* node = nurse_per_day->first;
		while(node->next != NULL){
			Node* aux = node->next;
			int cont = 0;
			if(node->data == EVENING && aux->data == MORNING)
				cont++;
			else if(node->data == NIGHT && aux->data == MORNING)
				cont++;
			else if(node->data == NIGHT && aux->data == EVENING)
				cont++;
			

			if(node->data == aux->data)
				cont++;

			if(node->data != FREE)
				(*vet)++;

			node = node->next;
		}
	return cont;
}

int* shifts_per_day2(List** nurse_per_day, int d){
	int* rt = (int*) calloc(n_shifts, sizeof(int));
	
	for (int i = 0; i < n_nurses; i++){
		int e = getElementByIndex(nurse_per_day[i], d);
		//printf("%d ", e);
		if(e == MORNING)
			rt[MORNING]++;
		if(e == EVENING)
			rt[EVENING]++;
		if(e == NIGHT)
			rt[NIGHT]++;
		if(e == FREE)
			rt[FREE]++;
	}

	//printf("day: %d (m: %d, t: %d, n: %d, f: %d)\n", d, rt[MORNING], rt[EVENING], rt[NIGHT], rt[FREE]);
	return rt;
}


int verify_constraints(NspLib* nsp, Constraints*c , Schedule* sc, int n, int day){
	int vet = 0;
	int nHCV = sequencial_shifts(sc->nurse_per_day[n], &vet);

	if (verify_number_of_assigments(vet, c->number_of_assigments) == 1)
		nHCV+= nHCV;
	return (Ph * nHCV);
}

void recombine_schedule(int** m_assigment, int* assignment_vector, Schedule* s, int day){
	if(day > 0){
		for (int i = 0; i < n_nurses; i++){
			setList(s->day_per_nurse[day],m_assigment[i][assignment_vector[i]],i);
			//printList(s->day_per_nurse[day]);
			//printf("\n");
		}
		//printf("\n");
		for (int i = 0; i < n_nurses; i++){
			setList(s->nurse_per_day[i], m_assigment[i][assignment_vector[i]],day);
			//printList(s->nurse_per_day[i]);
			//printf("\n");
		}
	}
}
void pcr(Schedule* s, NspLib* nsp, Constraints* c){

	for (int day = 0; day < n_days-1; day++){
		int** m_cost = (int**) calloc(n_nurses, sizeof(int*));
		int** m_cost2 = (int**) calloc(n_nurses, sizeof(int*));
		int** m_assigment = (int**) calloc(n_nurses, sizeof(int*));

		for (int nurse1 = 0; nurse1 < n_nurses; nurse1++){
			m_cost[nurse1] = (int*) calloc(n_nurses, sizeof(int));
			m_cost2[nurse1] = (int*) calloc(n_nurses, sizeof(int));
			m_assigment[nurse1] = (int*) calloc(n_nurses, sizeof(int));

			List* list1 = s->nurse_per_day[nurse1];
			Node* n1 = getNodeByIndex(list1,day);

			Node* aux = n1->next;

			for (int nurse2 = 0; nurse2 < n_nurses; nurse2++){
				List* list2 = s->nurse_per_day[nurse2];
				Node* n2 = getNodeByIndex(list2,day+1);

				n1->next = n2;
				
				//calculos
				int* minimum_coverage = shifts_per_day2(s->nurse_per_day,day+1);
				int x = verify_minimum_coverage1(nsp->coverage_matrix[day+1], minimum_coverage);
				free(minimum_coverage);

				if(x == 0){
					int cons = verify_constraints(nsp, c, s, nurse1, day+1);
					m_cost[nurse1][nurse2] += nsp->preference_matrix[nurse1][(day+1*n_shifts)+n2->data] + cons;
					m_cost2[nurse1][nurse2] += nsp->preference_matrix[nurse1][(day+1*n_shifts)+n2->data] + cons;
					m_assigment[nurse1][nurse2] = n2->data;
				}
				//final do calculo das constraints

				n1->next = aux;
			}
		}
		 //hungaro
		//hungarian algorithm
		hungarian_problem_t *p = (hungarian_problem_t*) calloc(1,sizeof(hungarian_problem_t));
		
		hungarian_init(p, m_cost2 , n_nurses,n_nurses, HUNGARIAN_MODE_MINIMIZE_COST) ;
		hungarian_solve(p);
		//hungarian_print_assignment_vector(p->assignment_vector,n_nurses,day);

		recombine_schedule(m_assigment, p->assignment_vector, s,day+1);

		hungarian_free(p);

		for (int i = 0; i < n_nurses; i++){
			free(m_cost[i]);
			free(m_cost2[i]);
			free(m_assigment[i]);
		}
		free(m_cost);
		free(m_cost2);
		free(m_assigment);
	}
}

void prt(Schedule* s, NspLib* nsp, Constraints* c){
	for (int d = 0; d < n_days; d++){

		for (int nurse1 = 0; nurse1 < n_nurses; nurse1++){
			List* list1 = s->nurse_per_day[nurse1];
			Node* n1 = getNodeByIndex(list1,d);

			Node* aux = n1->next;

			for (int nurse2 = 0; nurse2 < n_nurses; nurse2++){
				List* list2 = s->nurse_per_day[nurse2];
				Node* n2 = getNodeByIndex(list2,d+1);

				n1->next = n2;
			
				Node* p_n1; Node* p_aux;
				if(d > 0){
					p_n1 = getNodeByIndex(list2, d-1);
					p_aux = p_n1->next;

					p_n1->next = n1;
				}
				
				//calcula custos
				printf("Day %d\n", d);
				printList(list1);
				printf("\n");
				break;

				if(d > 0)
					p_n1->next = p_aux;
				n1->next = aux;
			}
			break;
		}
	}
}