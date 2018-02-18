#include "hash.h"
#include <mpi.h>

int main(int argc, char **argv)
{
	MPI_Init(&argc, &argv);
	int m, rank, p;
	char *dirname, *procid, *pc, *sqo;
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	m = 5000;
	wordNode *lht = (wordNode *)malloc(m * sizeof(wordNode));
	dirname = (char *)malloc(sizeof(char) * 10);
	procid = (char *)malloc(sizeof(char) * 5);
	pc = (char *)malloc(sizeof(char) * 3);
	sprintf(pc, "%d", rank);
	sqo = (char *)malloc(sizeof(char) * 2);
	strcpy(procid, strcat(strcat(strcpy(sqo, "["), pc), "]"));
	strcpy(dirname, "files");
	lht = fill_lht(m, lht, dirname, procid);
	free(procid);
	free(pc);
	free(sqo);
	// MPI_Barrier(MPI_COMM_WORLD);
	// wordNode* ght=(wordNode*)malloc(sizeof(wordNode)*((m/p)+1));
	wordNode *ght;
	char *wordarray;
	char word[100];
	if (rank == 0)
	{
		ght = (wordNode *)malloc(sizeof(wordNode) * m);
		wordarray = (char *)malloc(sizeof(char) * (100 * p));
		// totalstring = malloc(totlen * sizeof(char));
		// for (int i=0; i<(100*p)-1; i++)
		//     totalstring[i] = ' ';
		// totalstring[totlen-1] = '\0';
	}
	int i;
	wordNode temp;
	for (i = (rank * 5); i < m; i++)
	{
		if (lht[i] != NULL)
		{
			temp = lht[i];
			strcpy(word, temp->key);
			printf("%s %d %d\n", word, rank, i);
			MPI_Gather(word, 100, MPI_CHAR, wordarray, 100, MPI_CHAR, 0, MPI_COMM_WORLD);
			break;
		}
	}
	if (rank == 0)
	{
		printf("Hey\n");
		for (i = 0; i < p; i++)
		{
			printf("%s %d\n", &wordarray[i * 100], &wordarray[i * 100]);
		}
	}

	// printWords(lht,m);
	MPI_Finalize();
	return 0;
}
