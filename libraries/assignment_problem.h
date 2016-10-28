#include<stdio.h>
#include<stdlib.h>
#include "nspLib.h"
#include "hungarian/hungarian.h"

extern int n_nurses;
extern int n_days;
extern int n_shifts;

#define MORNING 0 
#define EVENING 1 
#define NIGHT 2 
#define FREE 3 
#define ONE 1

#define MAX_SHIFTS 3

#define Ph 100
#define Ps 100

/**
* This struct save a schedule solution. The schedule is save in two matrix of lists.
* 	1 - nurse_per_day, save the relation between nurse(lines) and day(columns);
* 	2 - day_per_nurse, save the relation between day(lines) and nurses(columns)
* The content in this two matrix is the shift atributed to one nurse in a day.
*/
typedef struct{
	List** nurse_per_day;
	List** day_per_nurse;
	int cost_solution;
}Multipartite_Graph;

/**
* Show the content of an schedule.
* @param m, is the struct that save the schedule.
* @param size1, is the size of nurse_per_day matrix.
* @param size2, is the size of day_per_nurse matrix.
*/
void show_multipartite_graph(Multipartite_Graph* m);

/**
* Verify number of assignments to a nurse. Check if this number it's between 
* an interval given in number_of_assignments vector.
* 
* @param shift_per_nurse, number of shifts assigned to a nurse.
* @number_of_assigments, vector with min and max values of assignments.
*
* @return 1 if number of shifts is lower or higher than min and max values. Otherwise, returns 0.
**/
int verify_number_of_assigments(int shift_per_nurse, int* number_of_assigments);

/**
* Assigned a shift to a nurse.
* 
* @param shift_per_nurse, vector that save amount of shifts to a nurse.
* @i, nurse.
**/
void att_shift2nurse(int* shift_per_nurse, int i);

/*
 * Verify if have minimum number of nurses assigned to shift j.
 
 * @param day, day will be the line index of coverage_matrix.
 * @param coverage_matrix, is the matrix that stores the minimum coverage of a day and a shift.
 * @param shift, shift will be the column index of coverage_matrix.
 * @param nurse_shift, how many nurses are assigned to the shift.
 *
 * @returns int, if the minimum coverage of nurses to day and shift is satisfied return 0, 
   otherwise, return 1.
*/
int verify_minimum_coverage(int day, int** coverage_matrix, int shift, int nurse_shift);

/*
* Assigns a nurse to a shift.
*
* @param vet, this vector stores the amount of nurses that are assigned to a shift.
* @param shift, shift that will be assigned a nurse.
*/
void att_nurse2shift(int* vet, int shift);

/*
* Checks if sequence of assigment is allowed.
*
* @param shift, shift that is being assigned.
* @param previous_shift, the previous shift assigned before shift.
*
* @return how many times this assigned break the rules.
*/
int verify_shift(int shift, int previous_shift);

/**
* Get the number assigned to a shift. This function is used because, cost matrix have number of shifts equal number of nurses,
* but, shift higher than max number of shifts are equal FREE shift.
*
* @param element, shift.
*
* @return number assigned to shift.
*/
int get_shift(int element);

/**
* This function verify number of consecutive shifts assigned to a nurse.
*
* @param consecutive_assignments, number of consecutive assignments given to a nurse
* @param max, max number of consecutive assignments that are allowed.
* @param min, min number of consecutive assignments that are allowed.
*
* @return 1 if number of consecutive assignments are lower or higher the min and max. Otherwise, return 0.
*/
int verify_consecutive_assigments(int consecutive_assigments, int min, int max);

/**
* This funtion build cost matrix and construct first solution of schedule based on cost matrix.
*
* @param nsp, informations about preferences and dimension of problem.
* @param c, informations about constraints.
*
* @return a schedule.
*/
Multipartite_Graph* build_cost_matrix(NspLib* nsp, Constraints* c);

Multipartite_Graph* build_cost_matrix2(NspLib* nsp, Constraints* c);