#include "string_utils.h"

void split_string(const char* string, int size, char*** output, int* arguments)
{
	//Predefiniowana tablica wskaznikow na 20 elementow
	char * predefp [20];
	int predeflen[20];

	//Predefinowany buffor
	char buffer[100];
	int len = 0;
	int strsize = 0;

	//Zerowanie tablicy wskaznikow
	for(int i = 0; i < 20; i++)
	{
		predefp[i] = 0;
		predeflen[i] = 0;
	}

	*arguments = 0;
	while(strsize < size)
	{
		//Litery
		if( (*string >= 65 && *string <= 90) || (*string >= 97 && *string <= 122) || (*string >= 48 && *string <=57) || (*string == 46))
		{
			//Wpisanie liczby do buffora
			buffer[len] = *string;
			len++;
		}
		else if(*string == ' ' || *string == '\t')
		{
			if(len > 0)
			{
				predefp[*arguments] = malloc(len + 1);
				predefp[*arguments][len] = '\0';
				memcpy(predefp[*arguments], buffer, len + 1);
				predeflen[*arguments] = len + 1;

				*arguments = *arguments + 1;
				len = 0;

				memset(buffer, '\0', 100);

				if(*arguments >= 20)
					break;
			}
		}
		string ++;
		strsize++;
	}
	//On exit
	if(len > 0)
	{
		predefp[*arguments] = malloc(len + 1);
		predefp[*arguments][len] = '\0';
		memcpy(predefp[*arguments], buffer, len + 1);
		predeflen[*arguments] = len + 1;
		*arguments = *arguments + 1;
	}

	*output = malloc(sizeof(char *) * (*arguments));
	for(int i = 0; i < *arguments; i++)
	{
		(*output)[i] = malloc(predeflen[i]);
		memcpy((*output)[i], predefp[i], predeflen[i]);
	}
}


void split_string_free(char *** output, int size)
{
	for(int i = 0; i < size; i++)
	{
		free((*output)[i]);
	}
	free(*output);
}
