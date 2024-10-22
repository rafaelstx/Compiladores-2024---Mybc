/*@<main.c>::*/

#include <stdio.h>
#include <stdlib.h>
#include <main.h>

FILE *src;

int main(int argc, char *argv[])
{
	if(argc > 1){

		src = fopen(argv[1],"r");
		if(!src){
			fprintf(stderr, "Arquivo nao encontrado\n");
			exit(-1);
		}

	} else {
		src = stdin;
	}
	
		lookahead = gettoken(src);
		mybc();


		exit(0);

}
