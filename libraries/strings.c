#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define TAB 9 
#define ENTER 13
#define LINE_FEED 10
#define NULLL 0

/**
 * Copy a substring from 'de' until 'para'.
 *
 * @param char* line, an string from where will copy any substring.
 * @param int de, the index which will start the new string .
 * @param int para, the index wich will ends the new string.
 *
 * @returns char converted to int.
 */
int strncopy(char* line, int from, int to){
	char* t = (char*) calloc(((to-from)+2), sizeof(char));
	for (int i = 0; i < (to - from)+1; i++){
		t[i] = line[from];
		from++;
	}
	int r = 0;
	if(t[0] != '\0' && t[0] != TAB && t[0] != ENTER && t[0] != LINE_FEED && t[0] != NULLL)
		r = atoi(t);
	free(t);
	return r;
}

/**
 * Get number of spaces in an string.
 *
 * @param char* line, an string.
 *
 * @returns number of spaces on string.
 */
int spaces(char* line){
	int s = 0;
	for (int i = 0; i < strlen(line); i++){
		if(line[i] == TAB){
			s++;
		}
	}
	return s;	
}
