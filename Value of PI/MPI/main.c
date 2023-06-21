#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include "mpi.h"

int main(int argc, char *argv[])

{

    int my_rank; /* rank of process	*/

    int p; /* number of process	*/

    int source; /* rank of sender	*/

    int dest; /* rank of reciever	*/

    int tag = 0; /* tag for messages	*/

    double step, x, pi, sum;
    int  range;
    double allSum;

    MPI_Status status; /* return status for 	*/

    /* recieve		*/

    /* Start up MPI */

    MPI_Init(&argc, &argv);

    /* Find out process rank */

    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    /* Find out number of process */

    MPI_Comm_size(MPI_COMM_WORLD, &p);

    if (my_rank == 0)
    {
        sum = 0.0;
        allSum = 0.0;
        static long  num_steps = 1000000;
        step = 1.0 / (double)num_steps;


        range = num_steps / (p - 1);

        if (num_steps % (p - 1) != 0)
        {
            int remainder = num_steps % (p - 1);
            int i = num_steps - remainder;
            for (i; i < num_steps; i++)
            {
                x = ((double)(i + 0.5)) * step;
                double x2 = x * x;
                sum += 4.0 / (1.0 + x2);
            }
        }
    }

    MPI_Bcast(&step, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(&range, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (my_rank != 0)
    {
        sum = 0.0;
        int  i = range * (my_rank - 1);
        for (i; i < range * my_rank; i++)
        {
            x = ((double)(i + 0.5)) * step;
            double x2 = x * x;
            sum += 4.0 / (1.0 + x2);
        }
    
    }

    MPI_Reduce(&sum, &allSum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (my_rank == 0)
    {
        pi = step * allSum;
        printf("%.20f", pi);
    }

    // for (i = 0; i < num_steps; i++)
    // {
    //     x = ((double)(i + 0.5)) * step;
    //     sum += 4.0 / (1.0 + x * x);
    // }
    // pi = step * sum;
    // printf("%.20f", pi);

    /* shutdown MPI */

    MPI_Finalize();

    return 0;
}