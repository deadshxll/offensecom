#pragma once
#include <sys/time.h>
#include <time.h>

/*
  File that contains all the helper functions required throughout the code
*/

// String format functions
void trim_string(char *str);
char **split_string(char *str, char delimiter, int *numParts);

// Sleep functions
void sleep_ms(int milliseconds);
