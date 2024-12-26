#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
	FILE *f = fopen("bello.txt", "w");
	char bello[25555];
	char mess[85554];
	char mess1[85554];
	// while(fgets(bello, sizeof(bello),f))
	// {
	// 	printf("\t%s", bello);
	// }

	printf("Entrez un message : ");
	scanf("%[\n]", bello);
	// system("echo'%s' | figlet | lolcat", bello);

	fgets(bello, sizeof(bello),stdin);
		sprintf(mess, "figlet '%s' | lolcat", bello);
		system("clear");
	system(mess);
	sprintf(mess1, "figlet '%s' | lolcat > bello.txt", bello);

	system(mess1);

	fclose(f);

	// fprintf(f, const char *restrict format, ...)

	return 0;
}