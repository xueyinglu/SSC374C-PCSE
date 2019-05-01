#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <math.h>
#include <mpi.h>
void initialize(float **x,int n){
    srand(time(NULL));
    for (int i= 0; i<n; i++)
    for (int j= 0; j<n; j++){
        x[i][j]=(float)rand()/(float)RAND_MAX;
        //printf("x[i][j]= %f\n",x[i][j]);
    }
}

void smooth(float **x,float **y,int n,float a,float b,float c, int myid, int dim, int numprocs){
    /* define a column type*/
    MPI_Datatype column;
    MPI_Type_vector(n,1,n,MPI_FLOAT,&column);
    MPI_Type_commit(&column);

    /* send and receive row vectors*/
    if (myid<numprocs-dim){
    // send the bottom row to the adjacent processor at the bottom
        MPI_Send(&x[n-2][0],n,MPI_FLOAT,myid+dim,myid,MPI_COMM_WORLD);
    }
    if (myid>=dim){
        // receive the bottom row from the adjacent processor on the top
        MPI_Recv(&x[0][0],n,MPI_FLOAT,myid-dim,myid-dim,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        // send the top row to the adjacent processor on the top
        MPI_Send(&x[1][0],n,MPI_FLOAT,myid-dim,myid,MPI_COMM_WORLD);
        
    }
    if (myid<numprocs-dim){
        // receive the top row from the adjacent processor at the bottom
        MPI_Recv(&x[n-1][0],n,MPI_FLOAT,myid+dim, myid+dim, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    }
    
    /* send and receive the column vectors*/

    if ((myid+1)%dim !=0){
        // send the right column to the adjacent processor on the right
        MPI_Send(&x[n-2][0],1,column,myid+1,myid,MPI_COMM_WORLD);

    }
    if (myid%dim !=0){
        // receive from the left
        MPI_Recv(&x[0][0],1,column,myid-1,myid-1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        // send the left column to the adjacent processor on the left
        MPI_Send(&x[1][0],1,column,myid-1,myid,MPI_COMM_WORLD);
    }
    if ((myid+1)%dim !=0){
        // receive from the right
        MPI_Recv(&x[n-1][0],1,column,myid+1,myid+1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

    }
    for (int i= 1; i<n-1; i++){

        int j;
        for (j= 1; j<n-1; j++){
            y[i][j] = a*(x[i-1][j-1] + x[i-1][j+1] + x[i+1][j-1] + x[i+1][j+1]) 
                        + b*(x[i-1][j+0] + x[i+1][j+0] + x[i+0][j-1] + x[i+0][j+1]) 
                        + c* x[i+0][j+0];
        }
}
}

void count(float **y,int n,float t,int *ct){
int localct=*ct;
int i,j;
    for (i= 1; i<n-1; i++){
    for (j= 0; j<n-1; j++){
        if (y[i][j]<t) {localct+=1;}
    }
}
*ct=localct;
}

int main( int argc, char*argv[]){

    int numprocs, myid;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    int dim = (int)(sqrt(numprocs)); // number of processors in each direction.
    int n = 16384;

//    int n=20;
    float a = 0.05;
    float b = 0.1;
    float c = 0.4;
    float t= 0.1;
    int ctx = 0;
    int cty = 0;
    float a1=clock();
    float **x = malloc(sizeof *x * n);
    if (x)
    {
        for (int i = 0; i < n; i++)
        {
            x[i] = malloc(sizeof *x[i] * n);
        }
    }
    float a2=clock();
    float t1= (a2 - a1) / (float)CLOCKS_PER_SEC;

    float a3=clock();
    float **y = malloc(sizeof *y * n);
    if (y)
    {
        for (int i = 0; i < n; i++)
        {
            y[i] = malloc(sizeof *y[i] * n);
        }
    }
    float a4=clock();
    float t2= (a4 - a3)/(float)CLOCKS_PER_SEC;
    initialize(x,n);
    smooth(x,y,n,a,b,c,myid,dim,numprocs);
    count(x,n,t,&ctx);
    count(y,n,t,&cty);

/*
    printf("Summary:\n");
    printf("Number of elements in a row/column: %d\n",n);
    printf("Number of inner elements in a row/column: %d\n",n-2);
    printf("Total number of elements: %d\n",n*n);
    printf("Total number of inner elements: %d\n",(n-2)*(n-2));
    printf("Memory (GB) used per array: %f\n", n*n*4*1e-9);
    printf("Threshold: %f\n", t);
    printf("Smoothing constants (a, b, c): %f %f %f \n",a,b,c);
    printf("Number of elements below threshold (X): %d\n",ctx);
    printf("Fraction of elements below threshold: %f\n", (float)ctx/((float)n*n));
    printf("Number of elements below threshold (Y): %d\n",cty);
    printf("Fraction of elements below threshold: %f\n", (float)cty/((float)n*n));
    printf("Action    Time     Time resolution = %f\n",1.0/(float)CLOCKS_PER_SEC);
    printf("CPU: Alloc-X: %f\n",t1);    
    printf("CPU: Alloc-Y: %f\n",t2);    
    printf("CPU: Init-X: %f\n",t3);    
    printf("CPU: Smooth: %f\n",t4);    
    printf("CPU: Count-X: %f\n",t5);    
    printf("CPU: Count-Y: %f\n",t6);    
*/
    return 0;
};



