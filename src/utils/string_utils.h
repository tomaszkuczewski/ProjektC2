//By Toasz Kuczewski
#include <stdlib.h>
#include <string.h>

#ifndef STR_UTILS_TOM
#define STR_UTILS_TOM

void split_string(const char* string, int size, char*** output, int* arguments);
void split_string_free(char *** output, int size);

#endif
