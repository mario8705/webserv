#include <stdio.h>
#include <stdlib.h>

int main()
{
	printf("Content-Type: text/html; charset=utf-8\r\n");
	printf("\r\n");

	printf("Coucou Je suis un programme en C %s\n", getenv("REQUEST_URI"));
	return 0;
}
