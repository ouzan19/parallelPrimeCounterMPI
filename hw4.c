#include <stdio.h>
#include "mpi.h"
#include <stdlib.h>
#include <math.h>

int isPrime(int n);
int totalWorkLoad(int N);
int* calculateLoads(int* numbers,int loadPerProcessor,int size,int n);
int countPrimes(int* numbers,int load);
int main( argc, argv )
int  argc;
char **argv;
{
	int id, size;
	int* numbers; 
	int* received;
	double t1,t2;
	double n=atof(argv[1]);
	MPI_Init( &argc, &argv );
	MPI_Comm_size( MPI_COMM_WORLD, &size );
	MPI_Comm_rank( MPI_COMM_WORLD, &id );
	int localCount,globalCount;
	
	
	if(id == 0){

		int* numbers = (int *) malloc(((n/2)-1)*sizeof(int));
		int i;
    
		//numbers[0] = 1;
		// numbers[1] = 2;
		for(i=3;i<=n;i+=2){
				numbers[(i-3)/2] = i;
		}
		int loadPerProcessor = totalWorkLoad(n) / size;
		//printf("%d\n",loadPerProcessor);
		int* loads = calculateLoads(numbers,loadPerProcessor,size,n/2-1);
		
		i=1;
		for(;i<size;i++){
			int count = loads[i]-loads[i-1];
			MPI_Send(&count,1,MPI_INT,i,1,MPI_COMM_WORLD);
			MPI_Send(&(numbers[loads[i-1]]),count,MPI_INT,i,0,MPI_COMM_WORLD);
		}
		t1=MPI_Wtime();
		localCount = 1 + countPrimes(numbers,loads[0]);
		
		
	}else{
		int count;
		MPI_Recv(&count,1,MPI_INT,0,1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		received = (int*) malloc(count*sizeof(int));
		MPI_Recv(received,count,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		localCount = countPrimes(received,count);
	}
	
	MPI_Reduce(&localCount,&globalCount,1,MPI_INT,MPI_SUM, 0,MPI_COMM_WORLD);
	if(id==0){
		t2=MPI_Wtime();
		 printf("%d %lf\n",globalCount,(t2-t1));
	}
    MPI_Finalize();
    return 0;
}

int totalWorkLoad(int N){
    
    int k = sqrt(N) / 2;
    return (8*k+1)*k*(k+1) / 3 - N;
}

int* calculateLoads(int* numbers,int loadPerProcessor,int size,int n){
    
    int* loads = (int *) malloc((size)*sizeof(int));
   
    int i,index=0;
    int count = 0;
    for(i=0;i<size-1;i++){
        int cost=0;
        while(cost < loadPerProcessor ){
            count++;
            cost += (int)sqrt(numbers[index++]);
        }
       // printf("%d\n",cost);
        loads[i] = count;
       // printf("%d\n",loads[i]);
    }
    
   loads[i] = n ;
     // printf("%d\n",loads[i]);
    return loads;
}

int countPrimes(int* numbers,int load){
    int count=0;
    int i=0;
    for(i=0;i<load;i++){
        if(isPrime(numbers[i]))
            count++;
    }
    
    return count;
    
}

int isPrime(int n){
    int i;
    int temp = sqrt(n);
    for (i=3;i<=temp;i+=2)
        if (n%i==0)
            return 0;
    return 1;
}


