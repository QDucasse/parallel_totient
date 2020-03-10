// =============================================================================
// PARALELLIZED VERSION WITH OPENMP -- by
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

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

long euler(long n, int num_threads)
{
  long length;
  
  omp_set_num_threads(num_threads);
  length = 0;
  // #pragma omp parallel + #pragma omp for + atomic?
  // OR
  // #pragma omp parallel for num_threads() reduction(+:length)
  //#pragma omp parallel for num_threads(num_threads) private(i) //reduction(+:length)
  #pragma omp parallel
  {
    int i, id;
    id = omp_get_thread_num();
    #pragma omp for
    for (i = 1; i < n; i++)
      if (relprime(n, i))
        #pragma omp atomic
        length++;
  }
  return length;
}

// The sum of Euler totient will perform the above function on the specified range.
// sumTotient lower upper = sum (map euler [lower, lower+1 .. upper])

long sumTotient(long lower, long upper, int num_threads)
{
  long sum;

  omp_set_num_threads(num_threads);
  sum = 0;
  // #pragma omp parallel + #pragma omp for + atomic?
  // OR
  // #pragma omp parallel for num_threads() reduction(+:sum)
  #pragma omp parallel
  {
    int i, id;
    id = omp_get_thread_num();
    //printf("Num threads: %i\n", id);
    #pragma omp for
    for (i = lower; i <= upper; i++)
    {
      //#pragma omp critical reduction(+: sum)
      #pragma omp atomic
      sum += euler(i, num_threads);
    }
  }
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
    euler(i, 4);
    end = clock();
    time_taken = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("euler(%lu) = %f seconds\n", i, time_taken);
  }
}

int main(int argc, char ** argv)
{
  long lower, upper;
  int num_threads;
  clock_t start, end;
  double time_taken;
  struct timeval timer_start, timer_end;

  if (argc != 4) {
    printf("not 3 arguments\n");
    return 1;
  }
  sscanf(argv[1], "%ld", &lower);
  sscanf(argv[2], "%ld", &upper);
  sscanf(argv[3], "%i", &num_threads);
  // START TIME
  gettimeofday(&timer_start, NULL);
  start = omp_get_wtime();
  long sum = sumTotient(lower, upper, num_threads);
  // STOP TIME
  gettimeofday(&timer_end, NULL);
  end = omp_get_wtime();

  // Calculate time spent in seconds by OMP method.
  time_taken = ((end-start));
  // Calculate time spent in milliseconds by gettimeofday method.
  double time_spent = timer_end.tv_sec - timer_start.tv_sec + (timer_end.tv_usec - timer_start.tv_usec) / 1000000.0;
  
  // Print the results.
  printf("C: Sum of Totients  between [%ld..%ld] is %ld\n",
         lower, upper, sum);
  printf("Total time: %f seconds\n", time_taken);
  printf("Total time: %.6f milliseconds\n", time_spent);
  return 0;
}
