// =============================================================================
// BASE VERSION USED TO PARALELLIZE
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

long op_number = 0;
// Computes the Highest Common Factor of x and y.
// hcf x 0 = x
// hcf x y = hcf y (rem x y)

long hcf(long x, long y)
{
  long t;

  while (y != 0) {
    op_number++; // comparison
    t = x % y;
    op_number++; // division
    x = y;
    y = t;
  }
  return x;
}

// Checks if x and y are relatively prime (i.e. highest common factor of x and y is 1).
// relprime x y = hcf x y == 1

int relprime(long x, long y)
{
  op_number++; // comparison
  return hcf(x, y) == 1;
}

int relprime2(long x, long y)
{
  if ((x%2==0) && (y%2==0)) return 0;
  else return hcf(x, y) == 1;
}

// Computes the number of integers that are smaller than n and relatively prime to n.
// euler n = length (filter (relprime n) [1 .. n-1])

long euler(long n)
{
  long length, i;

  length = 0;
  for (i = 1; i < n; i++) {
    op_number++; // comparison
    if (relprime(n, i)){
      length++;
      op_number++; // addition
    }
  }

  return length;
}

// The sum of Euler totient will perform the above function on the specified range.
// sumTotient lower upper = sum (map euler [lower, lower+1 .. upper])

long sumTotient(long lower, long upper)
{
  long sum, i;

  sum = 0;
  for (i = lower; i <= upper; i++) {
    op_number++; // addition
    sum = sum + euler(i);
  }

  return sum;
}


// Runs a benchmark on the Euler totient function computation.
// For i= 1 -> 1,000,000 with 100,000 steps

void runBenchmark()
{
  clock_t start_bench, end_bench;
  double time_taken_bench;

  for (long i = 1; i < 1000000 ; i = i + 100000) {
    start_bench = clock();
    euler(i);
    end_bench = clock();
    time_taken_bench = ((double) (end_bench - start_bench)) / CLOCKS_PER_SEC;
    printf("euler(%lu) = %f seconds\n", i, time_taken_bench);
  }
}

int main(int argc, char ** argv)
{
  long lower, upper;
  clock_t start, end;
  double time_taken;

  if (argc != 3) {
    printf("not 2 arguments\n");
    return 1;
  }
  sscanf(argv[1], "%ld", &lower);
  sscanf(argv[2], "%ld", &upper);
  // START TIME
  start = clock();
  long sum = sumTotient(lower, upper);
  // STOP TIME
  end = clock();
  time_taken = ((double) (end - start)) / CLOCKS_PER_SEC;
  printf("C: Sum of Totients  between [%ld..%ld] is %ld in %f\n",
         lower, upper, sum, time_taken);
  // printf("Running benchmark\n");
  // runBenchmark();
  printf("Number of operations: %ld\n",op_number);
  return 0;
}
