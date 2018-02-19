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

	int i,count,j,k,size,sum,k1;
	int *countarr;
    int *displs,*displs2;
    displs = (int*)calloc(p,sizeof(int));
    displs2 = (int*)calloc(p,sizeof(int));
	countarr = (int*)calloc(p,sizeof(int));
	wordNode temp;
	msgNode arr,temp1;
	int* arr2=NULL;
	msgNode recvarr=NULL,recvdocs=NULL;
	/* create a type for struct car */
    const int nitems=2;
    int  blocklengths[2] = {28,1};
    MPI_Datatype types[2] = {MPI_CHAR, MPI_INT};
    MPI_Datatype mpi_msg_type;
    MPI_Aint     offsets[2];

    offsets[0] = offsetof(msgnode, s);
    offsets[1] = offsetof(msgnode, num);
    MPI_Type_create_struct(nitems, blocklengths, offsets, types, &mpi_msg_type);
    MPI_Type_commit(&mpi_msg_type);

	for(i=0;i<m;i+=p)
	{
		// i=0;
		for (j = 0; j < p && (j+i)<m; j++)
		{
			/* code */
			// j=0;
			count=0;
			arr=NULL;
			temp=lht[i+j];
			while(temp!=NULL){
				count++;
				arr=(msgNode)realloc(arr,sizeof(msgnode)*count);
				strcpy(arr[count-1].s,temp->key);
				printf("%s  %d\n",arr[count-1].s,arr[count-1].num );
				arr[count-1].num=temp->size;
				temp=temp->next;
			}
			printf("%d  %d\n",count,rank );
			MPI_Gather(&count, 1, MPI_INT,countarr,1, MPI_INT,j, MPI_COMM_WORLD);
			
			if(rank==j){
				displs[0]=0;
				for(k=1;k<p;k++){
					displs[k]=displs[k-1]+countarr[k-1];
				}
				size=displs[p-1]+countarr[p-1];
				recvarr=(msgNode)malloc(size*sizeof(msgnode));
				printf("size----%d\n",size);

			}
    		MPI_Gatherv(arr,count,mpi_msg_type,recvarr,countarr,displs,mpi_msg_type,j,MPI_COMM_WORLD);
			free(arr);
			
			if(rank==j){
				sum=0;
				for(k=0;k<p;k++){
					for(k1=0;k1<countarr[k];k1++){
						sum+=recvarr[k1].num;
					}
				}
				recvdocs=(msgNode)malloc(sum*sizeof(msgnode));
			}
		}
		MPI_Barrier(MPI_COMM_WORLD);
		
			// int index=0;	
			// for(k=0;k<p;k++){
			// 	count=0;
			// 	while(count<countarr[k]){
			// 		temp1=&recvarr[index];
			// 		printf("%s %d index %d count%d rank%d\n", temp1->s,temp1->num,index,count,rank);
			// 		count++;
			// 		index++;
			// 	}
			// }
			

	}
	// printWords(lht, m);
	// MPI_Barrier(MPI_COMM_WORLD);
	// wordNode* ght=(wordNode*)malloc(sizeof(wordNode)*((m/p)+1));
	// wordNode *ght;
	// char *wordarray;
	// char word[100];
	// if (rank == 0)
	// {
	// 	ght = (wordNode *)malloc(sizeof(wordNode) * m);
	// 	wordarray = (char *)malloc(sizeof(char) * (100 * p));
	// 	// totalstring = malloc(totlen * sizeof(char));
	// 	// for (int i=0; i<(100*p)-1; i++)
	// 	//     totalstring[i] = ' ';
	// 	// totalstring[totlen-1] = '\0';
	// }
	// int i;
	// wordNode temp;
	// for (i = (rank * 5); i < m; i++)
	// {
	// 	if (lht[i] != NULL)
	// 	{
	// 		temp = lht[i];
	// 		strcpy(word, temp->key);
	// 		printf("%s %d %d\n", word, rank, i);
	// 		MPI_Gather(word, 100, MPI_CHAR, wordarray, 100, MPI_CHAR, 0, MPI_COMM_WORLD);
	// 		break;
	// 	}
	// }
	// if (rank == 0)
	// {
	// 	printf("Hey\n");
	// 	for (i = 0; i < p; i++)
	// 	{
	// 		printf("%s %d\n", &wordarray[i * 100], &wordarray[i * 100]);
	// 	}
	// }

	// printWords(lht,m);
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();
	return 0;
}
