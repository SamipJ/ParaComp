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
	// m = 10;
	wordNode *lht = (wordNode *)calloc(m,sizeof(wordNode));
	dirname = (char *)malloc(sizeof(char) * 10);
	procid = (char *)malloc(sizeof(char) * 5);
	pc = (char *)malloc(sizeof(char) * 3);
	sprintf(pc, "%d", rank);
	sqo = (char *)malloc(sizeof(char) * 2);
	char *files=(char*)calloc(32,sizeof(char));
	strcpy(files,"files-1/");
	// printf("%s\n",files);
	strcpy(procid, strcat(strcat(strcpy(sqo, "["), pc), "]"));
	// strcpy(dirname, "files");
	files=strcat(files,pc);
	// printf("%s\n",files);
	strcpy(dirname,files);
	printf("%s\n",dirname);
	lht = fill_lht(m, lht, dirname, procid);
	// printWords(lht,m);
	free(procid);
	free(pc);
	free(sqo);
	free(dirname);
	free(files);

	int i, count, j, k, size, sum, sum1, sum2, k1, x, y, z, indexdoc, indexword;
	int *countarr = NULL, *countarr2 = NULL;
	int *displs = NULL, *displs2 = NULL;
	docNode temp2, headDoc,prevDoc,tempDoc,freeDoc;	
	wordNode temp, headWord,prev,freeWord;
	msgNode arr, temp1, sendDocData=NULL;
	int *arr2 = NULL;
	msgNode recvarr = NULL, recvdocs = NULL;
	/* create a type for struct car */
	const int nitems = 2;
	int blocklengths[2] = {150, 1};
	MPI_Datatype types[2] = {MPI_CHAR, MPI_INT};
	MPI_Datatype mpi_msg_type;
	MPI_Aint offsets[2];
	offsets[0] = offsetof(msgnode, s);
	offsets[1] = offsetof(msgnode, num);
	MPI_Type_create_struct(nitems, blocklengths, offsets, types, &mpi_msg_type);
	MPI_Type_commit(&mpi_msg_type);

	for (i = 0; i < m; i += p)
	{
		// i=0;
		for (j = 0; j < p && (j + i) < m; j++)
		{
			/* code */
			// j=0;
			count = 0;
			arr = NULL;
			temp = lht[i + j];
			sum = 0;
			k = 0;
			sendDocData=NULL;
			while (temp != NULL)
			{
				count++;
				arr = (msgNode)realloc(arr, sizeof(msgnode) * count);
				strcpy(arr[count - 1].s, temp->key);
				free(temp->key);
				// printf("%s  %d\n",arr[count-1].s,arr[count-1].num );
				arr[count - 1].num = temp->size;
				sum += temp->size;
				sendDocData = (msgNode)realloc(sendDocData, sum * sizeof(msgnode));
				temp2 = temp->docLink;
				while (temp2 != NULL)
				{
					strcpy(sendDocData[k].s, temp2->name);
					sendDocData[k].num = temp2->freq;
					freeDoc=temp2;
					temp2 = temp2->next;
					free(freeDoc);
					k++;
				}
				freeWord=temp;
				temp = temp->next;
				free(freeWord);
				
			}
			if(rank==j){
				displs = (int *)calloc(p, sizeof(int));
				countarr = (int *)calloc(p, sizeof(int));
			}

			// printf("1 My Rank %d\n",rank );
			MPI_Gather(&count, 1, MPI_INT, countarr, 1, MPI_INT, j, MPI_COMM_WORLD);

			if (rank == j)
			{
				displs[0] = 0;
				for (k = 1; k < p; k++)
				{
					displs[k] = displs[k - 1] + countarr[k - 1];
				}
				size = displs[p - 1] + countarr[p - 1];
				recvarr = (msgNode)malloc(size * sizeof(msgnode));
				// printf("size----%d\n",size);
			}
			// printf("2 My Rank %d\n",rank );
			MPI_Gatherv(arr, count, mpi_msg_type, recvarr, countarr, displs, mpi_msg_type, j, MPI_COMM_WORLD);
			free(arr);
			if (rank == j)
			{
				sum1 = 0;
				countarr2 = (int *)malloc(p * sizeof(int));
				displs2 = (int *)malloc(p * sizeof(int));
				x=0;
				for (k = 0; k < p; k++)
				{
					sum2 = 0;
					for (k1 = 0; k1 < countarr[k]; k1++)
					{
						sum2 += recvarr[x].num;
						x++;
					}
					countarr2[k] = sum2;
					displs2[k] = sum1;
					sum1 += sum2;
				}
				recvdocs = (msgNode)malloc(sum1 * sizeof(msgnode));
			}

			// printf("3 My Rank %d\n",rank );
			// printf("%d\n",sum);
			MPI_Gatherv(sendDocData, sum, mpi_msg_type, recvdocs, countarr2, displs2, mpi_msg_type, j, MPI_COMM_WORLD);
			free(sendDocData);
			if (rank == j)
			{
				// for (k = 0; k < sum1; k++)
				// {
				// 	printf("----%s %d\n", recvdocs[k].s, recvdocs[k].num);
				// }
				free(countarr2);
				free(displs2);
				indexword = 0;
				indexdoc = 0;
				temp = NULL;
				prev = NULL;
				headWord = NULL;
				tempDoc = NULL;
				prevDoc = NULL;
				headDoc = NULL;
				for (x = 0; x < p; x++)
				{
					temp = headWord;
					// prev=NULL;	
					for (y = 0; y < countarr[x]; y++)
					{
						if (headWord == NULL)
						{
							headWord = (wordNode)malloc(sizeof(wordnode));
							temp = headWord;
							temp->key=(char*)malloc(sizeof(char)*(strlen(recvarr[indexword].s)+1));
							strcpy(temp->key, recvarr[indexword].s);
							temp->size = recvarr[indexword].num;
							// printf("1>>>>>>%s %d\n",temp->key,temp->size);
							temp->next = NULL;
							temp->docLink = (docNode)malloc(sizeof(docnode));
							tempDoc = temp->docLink;
							for (z = 0; z < temp->size - 1; z++)
							{
								tempDoc->name=(char*)malloc(sizeof(char)*(strlen(recvarr[indexdoc].s)+1));
								strcpy(tempDoc->name, recvdocs[indexdoc].s);
								tempDoc->freq = recvdocs[indexdoc].num;
								// printf("-1<<<<<%s %d\n",tempDoc->name,tempDoc->freq);
								prevDoc = tempDoc;
								tempDoc->next = (docNode)malloc(sizeof(docnode));
								tempDoc = tempDoc->next;
								indexdoc++;
							}
							tempDoc->name=(char*)malloc(sizeof(char)*(strlen(recvarr[indexdoc].s)+1));
							strcpy(tempDoc->name, recvdocs[indexdoc].s);
							tempDoc->freq = recvdocs[indexdoc].num;
							// printf("0<<<<<%s %d\n",tempDoc->name,tempDoc->freq);
							indexdoc++;
							tempDoc->next=NULL;
						}
						else
						{
							while(temp->next!=NULL && strcmp(recvarr[indexword].s,temp->key)>0){
								prev=temp;
								temp=temp->next;
							}
							if(strcmp(recvarr[indexword].s,temp->key)>0){
								temp->next=(wordNode)malloc(sizeof(wordnode));
								temp=temp->next;
								temp->key=(char*)malloc(sizeof(char)*(strlen(recvarr[indexword].s)+1));
								strcpy(temp->key, recvarr[indexword].s);
								// tempDoc->name=recvdocs[indexdoc].s;
								temp->size = recvarr[indexword].num;
								// printf("2>>>>>>%s %d\n",temp->key,temp->size);
								temp->next = NULL;
								temp->docLink = (docNode)malloc(sizeof(docnode));
								tempDoc = temp->docLink;
								for (z = 0; z < temp->size - 1; z++)
								{
									tempDoc->name=(char*)malloc(sizeof(char)*(strlen(recvarr[indexdoc].s)+1));
									strcpy(tempDoc->name, recvdocs[indexdoc].s);
									tempDoc->freq = recvdocs[indexdoc].num;
									// printf("1<<<<<%s %d\n",tempDoc->name,tempDoc->freq);
									prevDoc = tempDoc;
									tempDoc->next = (docNode)malloc(sizeof(docnode));
									tempDoc = tempDoc->next;
									indexdoc++;
								}
								tempDoc->name=(char*)malloc(sizeof(char)*(strlen(recvarr[indexdoc].s)+1));
								strcpy(tempDoc->name, recvdocs[indexdoc].s);
								tempDoc->freq = recvdocs[indexdoc].num;
								// printf("2<<<<<%s %d\n",tempDoc->name,tempDoc->freq);
								indexdoc++;
								tempDoc->next=NULL;
							}
							else if(strcmp(recvarr[indexword].s,temp->key)==0)
							{
								// printf("3>>>>>>%s %d\n",temp->key,temp->size);
								temp->size+=recvarr[indexword].num;
								tempDoc=temp->docLink;
								prevDoc=NULL;
								for(z=0;z<(recvarr[indexword].num);z++){
									// printf("z:%d\n",z);
									if ((recvdocs[indexdoc].num == temp->docLink->freq && z==0)||recvdocs[indexdoc].num > temp->docLink->freq){
										prevDoc = (docNode)malloc(sizeof(docnode));
										prevDoc->freq=recvdocs[indexdoc].num;
										prevDoc->name=(char*)malloc(sizeof(char)*(strlen(recvdocs[indexdoc].s)+1));
										strcpy(prevDoc->name,recvdocs[indexdoc].s);
										// printf("3<<<<<%s %d\n",prevDoc->name,prevDoc->freq);
										prevDoc->next=tempDoc;
										temp->docLink = prevDoc;
										// tempDoc = temp->docLink;
									}else{
										while(tempDoc->next!=NULL&& tempDoc->freq > recvdocs[indexdoc].num){
											prevDoc=tempDoc;
											tempDoc=tempDoc->next;
										}
										if(tempDoc->freq > recvdocs[indexdoc].num){
											while(z<(recvarr[indexword].num)){
											tempDoc->next = (docNode)malloc(sizeof(docnode));
											tempDoc=tempDoc->next;
											tempDoc->freq = recvdocs[indexdoc].num;
											tempDoc->name=(char*)malloc(sizeof(char)*(strlen(recvdocs[indexdoc].s)+1));
											strcpy(tempDoc->name,recvdocs[indexdoc].s);
											// printf("4<<<<<%s %d\n",tempDoc->name,tempDoc->freq);
											prevDoc = tempDoc;
											tempDoc->next=NULL;
											z++;
											indexdoc++;}
											break;
										}else{
											prevDoc->next = (docNode)malloc(sizeof(docnode));
											prevDoc=prevDoc->next;
											prevDoc->freq = recvdocs[indexdoc].num;
											prevDoc->name=(char*)malloc(sizeof(char)*(strlen(recvdocs[indexdoc].s)+1));
											strcpy(prevDoc->name,recvdocs[indexdoc].s);
											// printf("5<<<<<%s %d\n",tempDoc->name,tempDoc->freq);
											prevDoc->next = tempDoc;
										}
									}
									indexdoc++;
								}


							}
							else if(strcmp(recvarr[indexword].s,temp->key)<0)
							{
								if(strcmp(temp->key,headWord->key)==0){
									prev=(wordNode)malloc(sizeof(wordnode));
									prev->next=temp;
									prev->key=(char*)malloc(sizeof(char)*(strlen(recvarr[indexword].s)+1));
									strcpy(prev->key, recvarr[indexword].s);
									prev->size = recvarr[indexword].num;
									// printf("4>>>>>>%s %d\n",temp->key,temp->size);
									prev->next = NULL;
									prev->docLink = (docNode)malloc(sizeof(docnode));
									tempDoc = temp->docLink;
									for (z = 0; z < temp->size - 1; z++)
									{
										tempDoc->name=(char*)malloc(sizeof(char)*(strlen(recvarr[indexdoc].s)+1));
										strcpy(tempDoc->name, recvdocs[indexdoc].s);
										tempDoc->freq = recvdocs[indexdoc].num;
										// printf("6<<<<<%s %d\n",tempDoc->name,tempDoc->freq);
										prevDoc = tempDoc;
										tempDoc->next = (docNode)malloc(sizeof(docnode));
										tempDoc = tempDoc->next;
										indexdoc++;
									}
									tempDoc->name=(char*)malloc(sizeof(char)*(strlen(recvarr[indexdoc].s)+1));
									strcpy(tempDoc->name, recvdocs[indexdoc].s);
									tempDoc->freq = recvdocs[indexdoc].num;
									// printf("7<<<<<%s %d\n",tempDoc->name,tempDoc->freq);
									indexdoc++;
									tempDoc->next=NULL;
									headWord=prev;
								}
								else{
									prev->next=(wordNode)malloc(sizeof(wordnode));
									prev=prev->next;
									prev->next=temp;
									temp->key=(char*)malloc(sizeof(char)*(strlen(recvarr[indexword].s)+1));
									strcpy(temp->key, recvarr[indexword].s);
									prev->size = recvarr[indexword].num;
									// printf("5>>>>>>%s %d\n",temp->key,temp->size);
									prev->next = NULL;
									prev->docLink = (docNode)malloc(sizeof(docnode));
									tempDoc = temp->docLink;
									for (z = 0; z < temp->size - 1; z++)
									{
										tempDoc->name=(char*)malloc(sizeof(char)*(strlen(recvarr[indexdoc].s)+1));
										strcpy(tempDoc->name, recvdocs[indexdoc].s);
										tempDoc->freq = recvdocs[indexdoc].num;
										// printf("8<<<<<%s %d\n",tempDoc->name,tempDoc->freq);
										prevDoc = tempDoc;
										tempDoc->next = (docNode)malloc(sizeof(docnode));
										tempDoc = tempDoc->next;
										indexdoc++;
									}
									tempDoc->name=(char*)malloc(sizeof(char)*(strlen(recvarr[indexdoc].s)+1));
									strcpy(tempDoc->name, recvdocs[indexdoc].s);
									tempDoc->freq = recvdocs[indexdoc].num;
									// printf("9<<<<<%s %d\n",tempDoc->name,tempDoc->freq);
									indexdoc++;
									tempDoc->next=NULL;
								}

							}
						}
						indexword++;
					}
				}
				free(countarr);
				free(displs);
				free(recvdocs);
				free(recvarr);

				

			}
			
		}
		// for(x=0;x<p;x++){
		// 	MPI_Barrier(MPI_COMM_WORLD);
		// 	if (rank!=x) continue;
		// 	temp = headWord;
		// 	while(temp!=NULL){
		// 		tempDoc = temp->docLink;
		// 		printf("%s word %d\n",temp->key,temp->size);
		// 		while(tempDoc!=NULL){
		// 			printf("\t%s doc %d\n",tempDoc->name,tempDoc->freq);
		// 			tempDoc=tempDoc->next;
		// 		}
		// 		temp=temp->next;
		// 	}
		// }
	}
	// MPI_Barrier(MPI_COMM_WORLD);

	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();
	return 0;
}
