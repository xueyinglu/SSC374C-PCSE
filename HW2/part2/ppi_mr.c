#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>
#define f(x)   ( 4.0e0 / (1.0e0 + (x)*(x)) )
#define MTAG1 1
#define MTAG2 2
double mysecond();

int main(int argc, char *argv[])
{

  double PI25DT = 3.141592653589793238462643e0;

  double  pi, mypi, h, sum, x, t0_int,t1_int,t0_red,t1_red,avg_int,max_int,min_int,avg_red,max_red,min_red;
  int     n, i, ierr, myid, numprocs, islave;
 
  char line[12];
  MPI_Status status;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    if (myid ==0) scanf("%d",&n);
    MPI_Bcast(&n,1,MPI_INT, 0, MPI_COMM_WORLD);
        h    = 1.0e0/n;           /* Calculate the interval size */
        sum  = 0.0e0;
  
        t0_int = mysecond();
        for(i = myid+1; i <= n; i+=numprocs) {
            x = h * ( (double)(i) - 0.5e0 );
            sum = sum + f(x);
        }
        t1_int = mysecond()-t0_int;
        mypi = h * sum;
        t0_red = mysecond();
        //MPI_Reduce (&mypi, &pi, 1, MPI_DOUBLE, MPI_SUM, 0 , MPI_COMM_WORLD);
        if (myid != 0) {
            MPI_Ssend(&mypi,1, MPI_DOUBLE_PRECISION, 0, MTAG2, MPI_COMM_WORLD);
        }
        else {
            pi =mypi;
            for (islave=1; islave<numprocs;islave++) {
                MPI_Recv (&mypi, 1, MPI_DOUBLE_PRECISION, islave, MTAG2, MPI_COMM_WORLD, &status);
                pi += mypi;
            }
        }
        
        t1_red = mysecond()-t0_red;
        MPI_Reduce (&t1_int, &avg_int, 1, MPI_DOUBLE, MPI_SUM, 0 , MPI_COMM_WORLD);
        MPI_Reduce (&t1_int, &max_int, 1, MPI_DOUBLE, MPI_MAX, 0 , MPI_COMM_WORLD);
        MPI_Reduce (&t1_int, &min_int, 1, MPI_DOUBLE, MPI_MIN, 0 , MPI_COMM_WORLD);
        MPI_Reduce (&t1_red, &avg_red, 1, MPI_DOUBLE, MPI_SUM, 0 , MPI_COMM_WORLD);
        MPI_Reduce (&t1_red, &max_red, 1, MPI_DOUBLE, MPI_MAX, 0 , MPI_COMM_WORLD);
        MPI_Reduce (&t1_red, &min_red, 1, MPI_DOUBLE, MPI_MIN, 0 , MPI_COMM_WORLD);

        if (myid == 0){
            printf(" calc. pi:%20.16f  Error:%20.16f\n", pi, pi - PI25DT);
            printf(" avg integreation: %13.9f(sec) max integration %13.9f(sec) min integration %13.9f(sec)\n", avg_int/numprocs, max_int,min_int );
            printf(" avg reduction: %13.9f(sec) max reduction %13.9f(sec) min reduction %13.9f(sec)\n", avg_red/numprocs, max_red,min_red );
    }
  MPI_Finalize();
  return(0);
}

double mysecond()
{
   struct timeval tp;
   struct timezone tzp;
   int i;
   i = gettimeofday(&tp,&tzp);
   return ( (double) tp.tv_sec + (double) tp.tv_usec * 1.e-6 );
}


/* http://www-unix.mcs.anl.gov/mpi/www/ */
