#include "operators.h"


// quantos e quais turnos sao atribuidos em cada dia.
int* shifts_per_day1(List** schedule, int day){
	int	*rt = (int*) calloc(n_shifts, sizeof(int));

	for (int i = 0; i < n_nurses; i++){
		int e = getElementByIndex(schedule[i],day);
		if(e == MORNING)
			rt[0]++;
		if(e == EVENING)
			rt[1]++;
		if(e == NIGHT)
			rt[2]++;
	}
	return rt;
}

int verify_minimum_coverage1(int* coverage_matrix, int* minimum_coverage){
	if(minimum_coverage[0] < coverage_matrix[0])
		return 1;
	if(minimum_coverage[1] < coverage_matrix[1])
		return 1;
	if(minimum_coverage[2] < coverage_matrix[2])
		return 1;
	if(minimum_coverage[3] < coverage_matrix[3])
		return 1;
	return 0;
}

int* same_assignments(List** list, int nurse){
	int *rt = (int*) calloc(n_shifts, sizeof(int));

	for (int i = 0; i < n_days; i++){
		List* l1 = list[i];
		List* l2 = list[i+1];
		int s1 = getElementByIndex(l1,nurse);
		int s2 = getElementByIndex(l2,nurse);

		if(s1 == s2 && s1 == 0)
			rt[0]++;
		if(s1 == s2 && s1 == 1)
			rt[1]++;
		if(s1 == s2 && s1 == 2)
			rt[2]++;
		if(s1 == s2 && s1 == 3)
			rt[3]++;
			
	}
	return rt;
		
}

int getNextShift(List** list, int day, int nurse){
	List* l1 = list[day];
	List* l2 = list[day+1];
	int s2 = getElementByIndex(l2,nurse);
	return s2;
}

/**
void pcr2(List** day_per_nurse){

	for (int day = 0; day < n_days; day++){
		List* list_d1 = day_per_nurse[i];
		Node* d1 = list_d1->first;
		int index_d1 = 0;

		List* list_d2 = day_per_nurse[i+1];
		Node* d2 = list_d1->first;
		int index_d2 = 0;

		int** m_cost = (int**) calloc(n_nurses, sizeof(int*));
		int** m_cost2 = (int**) calloc(n_nurses, sizeof(int*));
		int** m_assigment = (int**) calloc(n_nurses, sizeof(int*));


		while(d1->next != NULL){
			while(d2->next != NULL){
				int data_d2 = d2->data;
				//troca o turno data_d1 (list_d1) por data_d2 
				setList(list_d1, data_d2, index_d1);
				int data_d1 = d1->data;

				//faz os cálculos das retrições
				int* minimum_coverage = shifts_per_day1(list_d1);
				int* shift_per_nurse = shifts_per_nurse1(list_d1);
				int* sa = same_assignments(list_d1, index_d1);

				if(data_d1 == getNextShift(list_d1,day))
					nHCV++;

				if(verify_consecutive_assigments(sa[data_d1], c->consecutive_assigments_matrix[data_d1][0], c->consecutive_assigments_matrix[data_d1][1]) == 1)
					nHCV++;		

				if (verify_number_of_assigments(shift_per_nurse, c->number_of_assigments) == 1)
					nHCV++;

				if(verify_minimum_coverage1(nsp->coverage_matrix[day], minimum_coverage) == 1)
					nSCV++;

				free(sa);
				free(shift_per_nurse);
				free(minimum_coverage);

				//if(nHCV != 0 || nSCV != 0)
				int cost = nsp->preference_matrix[index_d1][(day*n_shifts)+nurse->data] + Ph * nHCV + Ps * nSCV;

				m_cost[index_d1][index_d2] = cost;
				m_cost2[index_d1][index_d2] = cost;
				m_assigment[index_d1][index_d2] = data_d2;

				index_d2 = index_d2 + 1;
				d2 = d2->next;
			}

			index_d1 = index_d1 + 1;
			d1 = d1->next;
		}

		//hungaro
		hungarian_problem_t *p = (hungarian_problem_t*) calloc(1,sizeof(hungarian_problem_t));
		
		hungarian_init(p, m_cost2 , n_nurses,n_nurses, HUNGARIAN_MODE_MINIMIZE_COST) ;
		hungarian_solve(p);
		
		//free(schedule_day);
		//schedule_day = (int*) calloc(n_nurses, sizeof(int));
		//int c = combine_schedule(shift_per_nurse, m_cost, p->assignment_vector, m_assigment, nurse_per_day, day_per_nurse, d, schedule_day);

		hungarian_free(p);

		//fim do hungaro
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
**/

int* shifts_per_nurse1(List** nurse_per_day){
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

void pcr(List** nurse_per_day, NspLib* nsp, Constraints* c){

	for (int day = 0; day < n_days; day++){
		int** m_cost = (int**) calloc(n_nurses, sizeof(int*));
		int** m_cost2 = (int**) calloc(n_nurses, sizeof(int*));
		int** m_assigment = (int**) calloc(n_nurses, sizeof(int*));

		for (int nurse1 = 0; nurse1 < n_nurses; nurse1++){
			List* list1 = nurse_per_day[nurse1];
			Node* n1 = getNodeByIndex(list1,day);
			Node* aux = n1->next;

			for (int nurse2 = 0; nurse2 < n_nurses; nurse2++){
				int nSCV = 0, nHCV = 0;
				List* list2 = nurse_per_day[nurse2];
				Node* n2 = getNodeByIndex(list2,day+1);
				n1->next = n2;
				printList(nurse_per_day[day]);
				printf("\n");
				//calculos
				//verify_constraints(list1);
				int* minimum_coverage = shifts_per_day1(nurse_per_day, day);
				int* shift_per_nurse = shifts_per_nurse1(nurse_per_day);

				if(verify_minimum_coverage1(nsp->coverage_matrix[day], minimum_coverage) == 1)
					nSCV++;
				
				for (int i = 0; i < n_nurses; i++){
					printf("%d ", shift_per_nurse[i]);
					if(verify_number_of_assigments(shift_per_nurse[i], c->number_of_assigments))
						nHCV++;
				}
				
				if(nSCV != 0 || nHCV != 0)
					printf("nSCV: %d\n", nHCV);

				free(minimum_coverage);
				free(shift_per_nurse);
				n1->next = aux;
			}
			printf("\n\n");
		}
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