#include "oc_helper.h"
#include <stdlib.h>
#include <string.h>

void sleep_ms(int milliseconds) {
  struct timespec ts;
  ts.tv_sec = milliseconds / 1000;
  ts.tv_nsec = (milliseconds % 1000) * 1000000;
  nanosleep(&ts, NULL);
}

char **split_string(char *str, char delimiter, int *numParts) {
  int i, j = 0, k = 0;
  int len = strlen(str);
  char **parts = (char **)malloc((len / 2 + 1) * sizeof(char *));

  for (i = 0; i <= len; i++) {
    if (str[i] == delimiter || str[i] == '\0') {
      parts[j] = (char *)malloc((i - k + 1) * sizeof(char));
      strncpy(parts[j], str + k, i - k);
      parts[j][i - k] = '\0';
      j++;
      k = i + 1;
    }
  }

  *numParts = j;
  return parts;
}
