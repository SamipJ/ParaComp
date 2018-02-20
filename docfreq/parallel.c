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
	MPI_Request req,req1,req2,req3	;
	int i, count, j, k, size, sum, sum1, sum2, k1, x, y, z, indexdoc, indexword,size1;
	int *countarr = NULL, *countarr2 = NULL;
	int *displs = NULL, *displs2 = NULL;
	docNode temp2, headDoc,prevDoc,tempDoc,freeDoc;	
	wordNode temp, headWord,prev,freeWord;
	msgNode arr=NULL, temp1, sendDocData;
	int *arr2 = NULL;
	msgNode recvarr = NULL, recvdocs;
	/* create a type for struct car */
	const int nitems = 2;
	int blocklengths[2] = {150, 1};
	int msg[3];
	MPI_Datatype types[2] = {MPI_CHAR, MPI_INT};
	MPI_Datatype mpi_msg_type;
	MPI_Aint offsets[2];
	offsets[0] = offsetof(msgnode, s);
	offsets[1] = offsetof(msgnode, num);
	MPI_Type_create_struct(nitems, blocklengths, offsets, types, &mpi_msg_type);
	MPI_Type_commit(&mpi_msg_type);
	MPI_Status status;
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
				// free(temp->key);
				// printf("%s  %d\n",arr[count-1].s,arr[count-1].num );
				arr[count - 1].num = temp->size;
				sum += temp->size;
				sendDocData = (msgNode)realloc(sendDocData, sum * sizeof(msgnode));
				temp2 = temp->docLink;
				printf("hi----\n");
				if(strcmp(temp->key,"epublicans")==0){
					printf("fhello\n");
				}
				printf("%s\n",temp->key);
				printf("%d\n",temp->size);
				printf("%s\n",temp2->name);
				while (temp2 != NULL)
				{
					printf("hi\n");
					printf("%s\n",temp->key);
					printf("%s\n",temp2->name);
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
			
		}
		// if (rank == j)
		{
			// for (k = 0; k < sum1; k++)
			// {
			// 	printf("%d----%s %d\n",j, recvdocs[k].s, recvdocs[k].num);
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
			size1=0;
			for (x = 0; x < p; x++)
			{
				temp = headWord;
				// prev=NULL;	
				for (y = 0; y < countarr[x]; y++)
				{
					// if(strcmp(recvarr[indexword].s,"samip")==0){
					// 	printf("yeah");
					// }
					if (headWord == NULL)
					{
						headWord = (wordNode)malloc(sizeof(wordnode));
						size1++;
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
						// if(temp->key==NULL);
						// if(recvarr[indexword].s==NULL);
						// printf("%d\n",temp->size);
						// printf("%s\n",temp->key);
						// if(strcmp(recvarr[indexword].s,"samip")==0){
							// printf("hi\n");
						// }
						// printf("%s\n",recvarr[indexword].s);
						// if()
						if(strcmp(recvarr[indexword].s,temp->key)>0){
							temp->next=(wordNode)malloc(sizeof(wordnode));
							size1++;
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
							// printf("hi");
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
									indexdoc++;
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
										indexdoc++;
									}
								}
								// indexdoc++;
							}


						}
						else if(strcmp(recvarr[indexword].s,temp->key)<0)
						{
							if(strcmp(temp->key,headWord->key)==0){
								prev=(wordNode)malloc(sizeof(wordnode));
								size1++;
								prev->next=temp;
								prev->key=(char*)malloc(sizeof(char)*(strlen(recvarr[indexword].s)+1));
								strcpy(prev->key, recvarr[indexword].s);
								prev->size = recvarr[indexword].num;
								// printf("4>>>>>>%s %d\n",temp->key,temp->size);
								// prev->next = NULL;
								prev->docLink = (docNode)malloc(sizeof(docnode));
								tempDoc = prev->docLink;
								for (z = 0; z < prev->size - 1; z++)
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
								size1++;
								prev=prev->next;
								prev->next=temp;
								prev->key=(char*)malloc(sizeof(char)*(strlen(recvarr[indexword].s)+1));
								strcpy(prev->key, recvarr[indexword].s);
								prev->size = recvarr[indexword].num;
								// printf("5>>>>>>%s %d\n",temp->key,temp->size);
								// prev->next = temp;
								prev->docLink = (docNode)malloc(sizeof(docnode));
								tempDoc = prev->docLink;
								for (z = 0; z < prev->size - 1; z++)
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
		
		// if(rank!=0){
		// 	// sendDocData = NULL;
		// 	temp=headWord;
		// 	sendDocData= (msgNode)malloc((sum1+size1)*sizeof(msgnode));
		// 	count=0;
		// 	printf("%d %d\n",size1,sum1);
		// 	k=size1;
		// 	while(temp!=NULL){
		// 		count++;
		// 		strcpy(sendDocData[count - 1].s, temp->key);
		// 		// if(count==1) printf("%s %s\n",temp->key,sendDocData[count - 1].s);
		// 		free(temp->key);
		// 		sendDocData[count - 1].num = temp->size;
		// 		temp2 = temp->docLink;
		// 		while (temp2 != NULL)
		// 		{
		// 			strcpy(sendDocData[k].s, temp2->name);
		// 			sendDocData[k].num = temp2->freq;
		// 			freeDoc=temp2;
		// 			temp2 = temp2->next;
		// 			free(freeDoc);
		// 			k++;
		// 		}
		// 		freeWord=temp;
		// 		temp = temp->next;
		// 		free(freeWord);
		// 	}
		// 	// printf("%s\n",sendDocData[0].s);
		// 	msg[0]=rank;
		// 	msg[1]=size1;
		// 	msg[2]=sum1;
		// 	printf("hi\n");
		// 	for(i=0;i<sum1+size1;i++){
		// 		printf("%d : %d %s\n",i,sendDocData[i].num,sendDocData[i].s);
		// 	}
		// 	// MPI_Send(msg, 3, MPI_INT, 0, 1, MPI_COMM_WORLD);
		// 	printf("hi2\n");
		// 	// MPI_Send(sendDocData,size1+sum1,mpi_msg_type,0,2,MPI_COMM_WORLD);
		// 	free(sendDocData);
		// 	printf("hi3\n");
		// }
		
			
			// if(rank==0){
			// 	for(x=1;x<p;x++){
			// 		MPI_Recv(msg, 3, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
			// 		recvarr = (msgNode)calloc(msg[1]+msg[2],sizeof(msgnode));
			// 		MPI_Recv(recvarr,msg[1]+msg[2],mpi_msg_type,msg[0],2,MPI_COMM_WORLD,&status);
			// 		free(recvarr);
			// 	}

			// 	// MPI_Irecv(&count,1,MPI_INT,j,123,MPI_COMM_WORLD,&req);
			// 	// MPI_Wait(&req,MPI_STATUS_IGNORE);
			// 	// arr=(msgNode)malloc(count*sizeof(msgnode));
			// 	// MPI_Irecv(arr,count,mpi_msg_type,j,123,MPI_COMM_WORLD,&req);
			// 	// MPI_Wait(&req,MPI_STATUS_IGNORE);
			// 	// MPI_Irecv(&sum,1,MPI_INT,j,123,MPI_COMM_WORLD,&req);
			// 	// MPI_Wait(&req,MPI_STATUS_IGNORE);
			// 	// recvdocs=(msgNode)malloc(sum*sizeof(msgnode));
			// 	// MPI_Irecv(recvdocs,sum,MPI_INT,j,123,MPI_COMM_WORLD,&req);
			// 	// MPI_Wait(&req,MPI_STATUS_IGNORE);
			// 	// free(recvdocs);
			// 	// free(arr);
			// }

		// for(x=0;x<p;x++){
		// 	MPI_Barrier(MPI_COMM_WORLD);
		// 	if (rank!=x) continue;
		// 	// printf("---------%d\n",rank);
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
	// printf("%d\n",rank);
	MPI_Finalize();
	return 0;
}
