#include "operators.h"

int verify_minimum_coverage1(int** coverage_matrix, int** minimum_coverage){
	int r = 0;
	if(minimum_coverage[0][0] < coverage_matrix[0][0])
		r++;
	if(minimum_coverage[0][1] < coverage_matrix[0][1])
		r++;
	if(minimum_coverage[0][2] < coverage_matrix[0][2])
		r++;
	if(minimum_coverage[0][3] < coverage_matrix[0][3])
		r++;

	if(minimum_coverage[1][0] < coverage_matrix[1][0])
		r++;
	if(minimum_coverage[1][1] < coverage_matrix[1][1])
		r++;
	if(minimum_coverage[1][2] < coverage_matrix[1][2])
		r++;
	if(minimum_coverage[1][3] < coverage_matrix[1][3])
		r++;

	if(minimum_coverage[2][0] < coverage_matrix[2][0])
		r++;
	if(minimum_coverage[2][1] < coverage_matrix[2][1])
		r++;
	if(minimum_coverage[2][2] < coverage_matrix[2][2])
		r++;
	if(minimum_coverage[2][3] < coverage_matrix[2][3])
		r++;

	if(minimum_coverage[3][0] < coverage_matrix[3][0])
		r++;
	if(minimum_coverage[3][1] < coverage_matrix[3][1])
		r++;
	if(minimum_coverage[3][2] < coverage_matrix[3][2])
		r++;
	if(minimum_coverage[3][3] < coverage_matrix[3][3])
		r++;

	if(minimum_coverage[4][0] < coverage_matrix[4][0])
		r++;
	if(minimum_coverage[4][1] < coverage_matrix[4][1])
		r++;
	if(minimum_coverage[4][2] < coverage_matrix[4][2])
		r++;
	if(minimum_coverage[4][3] < coverage_matrix[4][3])
		r++;

	if(minimum_coverage[5][0] < coverage_matrix[5][0])
		r++;
	if(minimum_coverage[5][1] < coverage_matrix[5][1])
		r++;
	if(minimum_coverage[5][2] < coverage_matrix[5][2])
		r++;
	if(minimum_coverage[5][3] < coverage_matrix[5][3])
		r++;

	if(minimum_coverage[6][0] < coverage_matrix[6][0])
		r++;
	if(minimum_coverage[6][1] < coverage_matrix[6][1])
		r++;
	if(minimum_coverage[6][2] < coverage_matrix[6][2])
		r++;
	if(minimum_coverage[6][3] < coverage_matrix[6][3])
		r++;


	return r;
}

int** shifts_per_day1(List** day_per_nurse){
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
			if(node->data == EVENING && aux->data == MORNING){
				cont++;
			}
			else if(node->data == NIGHT && aux->data == MORNING){
				cont++;
			}
			else if(node->data == NIGHT && aux->data == EVENING){
				cont++;
			}

			if(node->data == aux->data)
				cont++;

			if(node->data != FREE)
				(*vet)++;

			node = node->next;
		}
	return cont;
}


int verify_constraints(NspLib* nsp, Constraints*c , List* s){
	//int** minimum_coverage = shifts_per_day1(s->day_per_nurse);
	//int x = verify_minimum_coverage1(nsp->coverage_matrix, minimum_coverage);
	//int nSCV = x;

	int vet = 0;
	int nHCV = sequencial_shifts(s, &vet);

	if (verify_number_of_assigments(vet, c->number_of_assigments) == 1)
		nHCV+= nHCV;
	return nHCV;
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

				//printList(list1);
				//printf("\n");

				//calculos
				int nHCV = verify_constraints(nsp, c, list1);
				m_cost[nurse1][nurse2] += nsp->preference_matrix[nurse1][(day+1*n_shifts)+n2->data] + Ph * nHCV;
				m_cost2[nurse1][nurse2] += nsp->preference_matrix[nurse1][(day+1*n_shifts)+n2->data] + Ph * nHCV;
				m_assigment[nurse1][nurse2] = n2->data;
				//final do calculo das constraints

				n1->next = aux;
			}
			//printf("\n\n");
		}

		 //hungaro
		//hungarian algorithm
		hungarian_problem_t *p = (hungarian_problem_t*) calloc(1,sizeof(hungarian_problem_t));
		
		hungarian_init(p, m_cost2 , n_nurses,n_nurses, HUNGARIAN_MODE_MINIMIZE_COST) ;
		hungarian_solve(p);
		hungarian_print_assignment_vector(p->assignment_vector,n_nurses,day);

		for (int i = 0; i < n_nurses; i++){
			printf("%d ", m_assigment[i][p->assignment_vector[i]]); 
		}
		//free(schedule_day);
		//schedule_day = (int*) calloc(n_nurses, sizeof(int));
		//int c = combine_schedule(shift_per_nurse, m_cost, p->assignment_vector, m_assigment, nurse_per_day, day_per_nurse, d, schedule_day);

		hungarian_free(p);


		printf("\n\n\n");
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