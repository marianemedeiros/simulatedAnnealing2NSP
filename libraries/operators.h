#include<stdio.h>
#include<stdlib.h>
#include "assignment_problem.h"

extern int n_nurses;
extern int n_days;
extern int n_shifts;

void pcr(Schedule* s, NspLib* nsp, Constraints* c);
int getNextShift(List** list, int day);
int* same_assignments(List** list, int nurse);
int verify_minimum_coverage1(int* coverage_matrix, int* minimum_coverage);
int* shifts_per_nurse1(List* list_d);
int* shifts_per_day1(List* list_d);