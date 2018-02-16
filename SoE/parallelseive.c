// mpicc parallelseive.c -lm -o exe
// mpirun -np 4 ./exe
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
// #include <string.h>
#include <time.h>
#include <math.h>

int main(int argc, char** argv) {
    // Initialize the MPI environment
    MPI_Init(&argc, &argv);

    int rank, p, i, j, k,primesize;
    long int n;
    char* primebool;
    int* primertn;

    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    
    // Print off a hello world message
    // printf("Hello from rank %d out of %d processors\n", rank, p);
    
    
    if(rank==0){
        printf("Enter Number:\n");
        scanf("%ld",&n);
	    // printf("ECHO: %ld\n",n);

    }
    
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast(&n,1,MPI_INT,0,MPI_COMM_WORLD);

    int rtn,rtrtn,count;
    count=0;
    rtn = sqrt(n);
    rtrtn=sqrt(rtn);

    if (n/p < rtn){
    	if(rank==0)
    	printf("You have either entered very small number or more processes than required which is making our parallel code worse than sequential. Please enter number at least square of no. of processes entered. \nThank You for your time.\n");
    	MPI_Finalize();
    	exit(0);
    }
    

    // printf("Hello from rank %d NUMBER %d \n", rank, n);
    
    int start,end;
    start = (n/p)*rank +1;
    start=start-start%2+1;
    end = (n/p)*(rank+1);
    if (rank==p-1){
        end=n;
    }
    // printf("%d %d %d\n",start,end,rank);
    primebool = (char*)malloc(sizeof(char)* ((end-start)/2+1));
    
    for(i=start;i<=end;i+=2){
        j = i-start;
        // printf("%d",i/2);
        primebool[j/2]='1';
    }

    
    MPI_Request req;
    
    if (rank==0){
        primebool[0]='0';
        for(i=3;i<=rtrtn;i+=2){
            if (primebool[i/2]=='1'){
                // printf("%d, %d\n",rank, i );
                MPI_Ibcast(&i,1,MPI_INT,0,MPI_COMM_WORLD,&req);
                count++;
                for(j=i*i;j<=rtn;j=j+2*i){
                    primebool[j/2]='0';
                }
            }
        }
        
        i=-1;
        MPI_Ibcast(&i,1,MPI_INT,0,MPI_COMM_WORLD,&req);
    }
    else{
        MPI_Ibcast(&i,1,MPI_INT,0,MPI_COMM_WORLD,&req);
        MPI_Wait(&req,MPI_STATUS_IGNORE);
        // printf("first%d, %d\n",rank, i );
        // start=start-start%2+1;
        while(i!=-1){
            if(start%i!=0)
                j=(start+i-(start%i));
            else j = start;
            // printf("%d \n",j);
            if (j%2==0) j=j+i;
            // printf("something%d %d\n",rank,i);
            while(j<=end){
                k=j-start;
                primebool[k/2]='0';
                j+=2*i;
            }
            MPI_Ibcast(&i,1,MPI_INT,0,MPI_COMM_WORLD,&req);
            MPI_Wait(&req,MPI_STATUS_IGNORE);
        }
        
    }

    if(rank==0){
        primertn=(int*)malloc(count*sizeof(int));
        int count1,size1;
        size1=count;
        count1=0;
        for (i=3;i<=rtn;i+=2){
            if(primebool[i/2]=='1'){
                primertn[count1++]=i;
            }
            if(size1==count1){
                primertn = (int*)realloc(primertn,(count1+count)*sizeof(int));
                size1=count1+count;
            }
        primesize = count1;
        // prime1=(int*)realloc(prime1,count1*sizeof(int));
        }
        i = count1-count;
        MPI_Ibcast(&i,1,MPI_INT,0,MPI_COMM_WORLD,&req);
        MPI_Ibcast(&primertn[count],i,MPI_INT,0,MPI_COMM_WORLD,&req);
    }else{
        MPI_Ibcast(&primesize,1,MPI_INT,0,MPI_COMM_WORLD,&req);
        MPI_Wait(&req,MPI_STATUS_IGNORE);
        primertn = (int*)malloc(sizeof(int)*primesize);
        MPI_Ibcast(primertn,primesize,MPI_INT,0,MPI_COMM_WORLD,&req);
        MPI_Wait(&req,MPI_STATUS_IGNORE);
    }

    int from=start;
    if (rank==0) from=rtn+1;
    for(i=0;i<primesize;i++){
        k = primertn[i];
        // printf("%d %d %d\n",k,rank,i);
        j=from;
        if (j%k!=0) j=(from+k-(from%k));
        if (j%2==0) j=j+k;
        if (j<k*k) j = k*k;
        // printf("%d %d jprintj\n",j,k);
        // if (k==7) printf("Hey %d %d %d\n",j,rank,from);
        for (j;j<=end;j+=2*k){
            // if(j==77) printf("Hi");
            primebool[(j-start)/2]='0';
        }
    }



    int* primenumbers = (int*)malloc(10*sizeof(int));
    int totalprime = 10;
    count = 0;
    for(i=start;i<=end;i+=2){

        if(primebool[(i-start)/2]=='1'){
            primenumbers[count++]=i;
            if(count==totalprime){
                primenumbers = (int*)realloc(primenumbers,totalprime*2*sizeof(int));
                totalprime=totalprime*2;
            }
        }
    }

    // MPI_Barrier(MPI_COMM_WORLD);

    int finalcount;
    int *finalprimes,*countarr=NULL;
    if(rank==0){
        countarr=(int*)malloc(p*sizeof(int));
    }
    MPI_Gather(&count, 1, MPI_INT,countarr,1, MPI_INT,0, MPI_COMM_WORLD);
    int *displs = NULL;
    if(rank==0){
        displs=(int*)malloc(p*sizeof(int));
        displs[0]=0;
        i=1;
        while(i<p){
            finalcount+=countarr[i-1];
            displs[i]=finalcount;
            i++;
        }
        finalcount+=countarr[i-1]+1;
        finalprimes=(int*)malloc(finalcount*sizeof(int));
        finalprimes[0]=2;

    }
    MPI_Gatherv(primenumbers, count, MPI_INT,&finalprimes[1], countarr, displs, MPI_INT,0, MPI_COMM_WORLD);


	if (rank==0)  printf("Total %d\n",finalcount);


    // if(rank==0){
    //     i=0;
    //     while(i<finalcount){
    //         printf("%d\n",finalprimes[i] );
    //         i++;

    //     }
    // }
    // MPI_Ibcast(&n,1,MPI_INT,0,MPI_COMM_WORLD,&req);
    // MPI_Wait(&req,MPI_STATUS_IGNORE);

    // Finalize the MPI environment.
    MPI_Finalize();
    // printf("End\n");
}
