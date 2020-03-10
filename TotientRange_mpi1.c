// =============================================================================
// PARALELLIZED VERSION WITH MPI -- by
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
  long lower, upper;

  if (argc != 3) {
    printf("not 2 arguments\n");
    return 1;
  }
  sscanf(argv[1], "%ld", &lower);
  sscanf(argv[2], "%ld", &upper);
  // START TIME
  long sum = sumTotient(lower, upper);
  // STOP TIME
  printf("C: Sum of Totients  between [%ld..%ld] is %ld\n",
         lower, upper, sum);
  return 0;
}
