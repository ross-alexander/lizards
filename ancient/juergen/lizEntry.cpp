#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	char s[200];
	fprintf(stdout, "Content-type: text/plain\n\n");
	fscanf(stdin, "%200s", s);
	fprintf(stdout, "%s\n\n", s);

	FILE *stream = fopen("entry.txt", "w");
	fprintf(stream, "%s\n", s);
	while (fscanf(stdin, "%200s", s) != EOF)
	{
		fprintf(stream, "%s\n", s);
	}
	exit(0);
}
