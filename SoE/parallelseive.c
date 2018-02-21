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
    double t1,t2;
    MPI_Init(&argc, &argv);
    t1 = MPI_Wtime();
    
    int rank, p;
    int primesize;
    long int n, i, j,k;
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
        // n=1000000000;
    }
    
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast(&n,1,MPI_LONG,0,MPI_COMM_WORLD);

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
    
    long int start,end;
    start = (n/p)*rank +1;
    start=start-start%2+1;
    end = (n/p)*(rank+1);
    if (rank==p-1){
        end=n;
    }
    // printf("%d %d %d\n",start,end,rank);
    primebool = (char*)malloc(sizeof(char)* (int)((end-start)/2+1));
    
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
                MPI_Ibcast(&i,1,MPI_LONG,0,MPI_COMM_WORLD,&req);
                count++;
                for(j=i*i;j<=rtn;j=j+2*i){
                    primebool[j/2]='0';
                }
            }
        }
        
        i=-1;
        MPI_Ibcast(&i,1,MPI_LONG,0,MPI_COMM_WORLD,&req);
    }
    else{
        MPI_Ibcast(&i,1,MPI_LONG,0,MPI_COMM_WORLD,&req);
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
            MPI_Ibcast(&i,1,MPI_LONG,0,MPI_COMM_WORLD,&req);
            MPI_Wait(&req,MPI_STATUS_IGNORE);
        }
        
    }

    if(rank==0){
        primertn=(int*)malloc(count*sizeof(int));
        int count1,size1,i1;
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
        i1 = count1-count;
        MPI_Ibcast(&i1,1,MPI_INT,0,MPI_COMM_WORLD,&req);
        MPI_Ibcast(&primertn[count],i1,MPI_INT,0,MPI_COMM_WORLD,&req);
    }else{
        MPI_Ibcast(&primesize,1,MPI_INT,0,MPI_COMM_WORLD,&req);
        MPI_Wait(&req,MPI_STATUS_IGNORE);
        primertn = (int*)malloc(sizeof(int)*primesize);
        MPI_Ibcast(primertn,primesize,MPI_INT,0,MPI_COMM_WORLD,&req);
        MPI_Wait(&req,MPI_STATUS_IGNORE);
    }

    long int from=start,j1,k1,i1;
    if (rank==0) from=rtn+1;
    for(i1=0;i1<primesize;i1++){
        k1 = primertn[i1];
        // printf("%d %d %d\n",k,rank,i);
        j1=from;
        if (j1%k1!=0) j1=(from+k1-(from%k1));
        if (j1%2==0) j1=j1+k1;
        if (j1<k1*k1) j1= k1*k1;
        // printf("%d %d jprintj\n",j,k);
        // if (k==7) printf("Hey %d %d %d\n",j,rank,from);
        for (j1;j1<=end;j1+=2*k1){
            // if(j==77) printf("Hi");
            primebool[(j1-start)/2]='0';
        }
    }



    long int* primenumbers = (long int*)malloc(10*sizeof(long int));
    int totalprime = 10;
    count = 0;
    for(i1=start;i1<=end;i1+=2){

        if(primebool[(i1-start)/2]=='1'){
            primenumbers[count++]=i1;
            if(count==totalprime){
                primenumbers = (long int*)realloc(primenumbers,totalprime*2*sizeof(long int));
                totalprime=totalprime*2;
            }
        }
    }

    // MPI_Barrier(MPI_COMM_WORLD);

    int finalcount=0;
    long int *finalprimes;
    int *countarr=NULL;
    if(rank==0){
        countarr=(int*)malloc(p*sizeof(int));
    }
    // printf("Hello World from %d\n",rank);
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Gather(&count, 1, MPI_INT,countarr,1, MPI_INT,0, MPI_COMM_WORLD);
    int *displs = NULL;
    if(rank==0){
        displs=(int*)malloc(p*sizeof(int));
        displs[0]=0;
        i1=1;
        while(i1<p){
            finalcount+=countarr[i1-1];
            displs[i1]=finalcount;
            i1++;
        }
        finalcount+=countarr[i1-1]+1;
        finalprimes=(long int*)malloc(finalcount*sizeof(long int));
        finalprimes[0]=2;

    }
    // printf("Hello World from %d\n",rank);
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Gatherv(primenumbers, count, MPI_LONG,&finalprimes[1], countarr, displs, MPI_LONG,0, MPI_COMM_WORLD);

    if (rank==0)  printf("Total %d\n",finalcount);

   FILE * fp;
   fp = fopen ("output", "w+");

    if(rank==0){
        i=0;
        while(i<finalcount){
            fprintf(fp,"%ld\n",finalprimes[i] );
            i++;
        }
    }


   // fclose(fp);
    // MPI_Ibcast(&n,1,MPI_INT,0,MPI_COMM_WORLD,&req);
    // MPI_Wait(&req,MPI_STATUS_IGNORE);

    // Finalize the MPI environment.
    t2 = MPI_Wtime();
    MPI_Barrier(MPI_COMM_WORLD);
    printf("MPI_Wtime measured by process %d: %1.2f\n",rank, t2-t1);fflush(stdout);
    MPI_Finalize();
    // printf("End\n");
}
