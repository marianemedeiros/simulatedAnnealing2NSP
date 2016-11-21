#include<stdio.h>
#include<stdlib.h>
#include "cost_matrix.h"

extern int n_nurses;
extern int n_days;
extern int n_shifts;

int verify_constraints(NspLib* nsp, Constraints*c , Schedule* sc, int n, int day);
int* shifts_per_day2(List** nurse_per_day, int d);
int** shifts_per_day(List** day_per_nurse);
int sequencial_shifts(List* nurse_per_day, int* vet);
int verify_minimum_coverage1(int* coverage_matrix, int* minimum_coverage);
void recombine_schedule(int** m_assigment, int* assignment_vector, Schedule* s, int day);
void pcr(Schedule* s, NspLib* nsp, Constraints* c);
void prt(Schedule* s, NspLib* nsp, Constraints* c);
