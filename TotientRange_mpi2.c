// =============================================================================
// PARALELLIZED VERSION WITH MPI -- by Quentin DUCASSE
// =============================================================================
// ORIGINAL COMMENT
// TotientRange.c - Sequential Euler Totient Function (C Version)
// compile: gcc -Wall -O -o TotientRange TotientRange.c
// run:     ./TotientRange lower_num uppper_num

// Greg Michaelson 14/10/2003
// Patrick Maier   29/01/2010 [enforced ANSI C compliance]

// This program calculates the sum of the totients between a lower and an
// upper limit using C longs. It is based on earlier work by:
// Phil Trinder, Nathan Charles, Hans-Wolfgang Loidl and Colin Runciman
// =============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

// Computes the Highest Common Factor of x and y.
// hcf x 0 = x
// hcf x y = hcf y (rem x y)

long hcf(long x, long y)
{
  long t;

  while (y != 0) {
    t = x % y;
    x = y;
    y = t;
  }
  return x;
}

// Checks if x and y are relatively prime (i.e. highest common factor of x and y is 1).
// relprime x y = hcf x y == 1

int relprime(long x, long y)
{
  return hcf(x, y) == 1;
}

// Computes the number of integers that are smaller than n and relatively prime to n.
// euler n = length (filter (relprime n) [1 .. n-1])

long euler(long n)
{
  long length, i;

  length = 0;
  for (i = 1; i < n; i++)
    if (relprime(n, i))
      length++;
  return length;
}

// The sum of Euler totient will perform the above function on the specified range.
// sumTotient lower upper = sum (map euler [lower, lower+1 .. upper])

long sumTotient(long lower, long upper)
{
  long sum, i;
  sum = 0;
  for (i = lower; i <= upper; i++)
    sum = sum + euler(i);
  return sum;
}

// Runs a benchmark on the Euler totient function computation.
// For i= 1 -> 1,000,000 with 100,000 steps

void runBenchmark()
{
  clock_t start, end;
  double time_taken;

  for (long i = 1; i < 1000000 ; i = i + 100000) {
    start = clock();
    euler(i);
    end = clock();
    time_taken = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("euler(%lu) = %f seconds\n", i, time_taken);
  }
}

int main(int argc, char ** argv)
{
  long lower, upper; /* bounds */
  int p,id;          /* nb of processes/ process id */
  long p_range, p_sum, p_bounds[2]; /* range of values for each process / sum computed by each process / lower-upper tuple */
  long result, *all_bounds; /* final sum / all bounds for each process */

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &p);
  MPI_Comm_rank(MPI_COMM_WORLD, &id);

  if (id == 0){ /* master */
    if (argc != 3) {
      printf("not 2 arguments\n");
      return 1;
    } // end arg check
    sscanf(argv[1], "%ld", &lower);
    sscanf(argv[2], "%ld", &upper);
    // Bounds handling
    all_bounds = (long*) malloc(p*2*sizeof(long)); /* the array will consist of [lower1,upper1,lower2,upper2,...] */
    p_range = (upper-lower)/p; /* each process will cover p_range values */
    all_bounds[0] = lower;
    for (int i=1; i<2*p; i++){
      if(i%2 != 0){ /* upper bound */
        all_bounds[i] = all_bounds[i-1] + p_range;
      }
      else{ /*lower bound */
        all_bounds[i] = all_bounds[i-1] + 1;
      }
    }
    all_bounds[2*p-1] = upper;
  } // end master

  MPI_Barrier(MPI_COMM_WORLD);
  // START TIME
  double start = MPI_Wtime();
  /* The process 0 will scatter the all_bounds array holding the lower and
     upper bounds for each process and distribute it over to each process'
     p_bounds tuple. */
  MPI_Scatter(all_bounds, 2, MPI_LONG, p_bounds, 2, MPI_LONG, 0, MPI_COMM_WORLD);

  p_sum = sumTotient(p_bounds[0], p_bounds[1]);

  /* The process 0 will collect the different processes' p_sums and add them
     into the result variable. */
  MPI_Reduce(&p_sum, &result, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
  // STOP TIME
  double stop = MPI_Wtime() - start;

  if (id==0){ /* master */
    printf("Sum of Totients  between [%ld..%ld] is %ld and was obtained in %f\n",
           lower, upper, result, stop, id);
  }

  MPI_Finalize();
  return 0;
}
