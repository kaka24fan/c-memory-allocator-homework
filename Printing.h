#ifndef PRINTING_H
#define PRINTING_H

#include "stdio.h"
#include "string.h"

bool gPaddingLineparity = true;

void printPadded(char* str, int width)
{
	gPaddingLineparity = !gPaddingLineparity;
	char* paddingSymbol = gPaddingLineparity ? "." : "_";

	printf(str);
	int padding = width - strlen(str);

	if (padding == 1)
	{
		printf(" ");
	}
	else if (padding == 2)
	{
		printf("  ");
	}
	else if (padding > 2)
	{
		printf(" ");
		for (int i = 0; i < padding - 2; i++)
		{
			printf(paddingSymbol);
		}
		printf(" ");
	}
}

#endif