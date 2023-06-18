#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>
#include "mpi.h"

bool isPrime(int x)

{

    if (x == 1 || x == 0)

    {

        return false;
    }

    int i;

    for (i = 2; i * i <= x; i++)

    {

        if (i == x)

        {

            continue;
        }

        if (x % i == 0)

        {

            return false;
        }
    }

    return true;
}

int primeCountInRange(int x, int y)

{

    printf("(");

    int i, counter = 0;

    for (i = x; i <= y; i++)

    {

        if (isPrime(i))

        {

            printf(" %d ", i);

            counter++;
        }
    }

    printf(") \n");

    return counter;
}

int main(int argc, char *argv[])

{

    int my_rank; /* rank of process	*/

    int p; /* number of process	*/

    int source; /* rank of sender	*/

    int dest; /* rank of reciever	*/

    int tag = 0; /* tag for messages	*/

    int x;

    int y;

    int size;

    int range;

    int countReduced = 0;

    int count = 0;

    int allCount = 0;

    clock_t begin;

    clock_t end;

    double time_spent;

    MPI_Status status; /* return status for 	*/

    /* recieve		*/

    /* Start up MPI */

    MPI_Init(&argc, &argv);

    /* Find out process rank */

    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    /* Find out number of process */

    MPI_Comm_size(MPI_COMM_WORLD, &p);

    if (my_rank == 0) // Master

    {

        printf("Please Enter the lower bound and upper bound \n");

        scanf("%d", &x);

        scanf("%d", &y);

        begin = clock();

        range = y - x;

        size = range / (p - 1);
    }

    MPI_Bcast(&x, 1, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Bcast(&range, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (my_rank != 0) // Slaves

    {

        size = range / (p - 1);

        int startingIndex = size * (my_rank - 1);

        startingIndex += x;

        y = startingIndex + size;

        if (my_rank != 1)

        {

            startingIndex++;
        }

        printf("p%d", my_rank);

        printf(" calculate partial count of prime numbers from %d", startingIndex);

        printf(" to %d", y);

        printf("\n");

        count = primeCountInRange(startingIndex, y);
    }

    MPI_Reduce(&count, &countReduced, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (my_rank == 0)

    {

        count, allCount = 0;

        if (range % (p - 1) != 0) // remainder handling

        {

            printf("p%d", my_rank);

            printf(" calculate partial count of prime numbers from %d", y - (range % (p - 1)) + 1);

            printf(" to %d", y);

            printf("\n");

            count = primeCountInRange(y - (range % (p - 1)) + 1, y);

            allCount += count;
        }

        allCount += countReduced;

        printf("After reduction, P0 will have Count = %d", allCount);

        printf("\n");

        end = clock();

        time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

        printf("Problem1 : Time: %.9f \n", time_spent);
    }

    /* shutdown MPI */

    MPI_Finalize();

    return 0;
}
