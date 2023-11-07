#include <stdio.h>
#include <string.h>
#include "revert_string.h"

void RevertString(char *str)
{
  int len = strlen(str);
  int i, j;

  for (i = 0, j = len - 1; i < j; i++, j--)
  {
    char temp = str[i];
    str[i] = str[j];
    str[j] = temp;
  }
}
