#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#include "mpi.h"
int main(int argc, char *argv[])
{
    int my_rank; /* rank of process */
    int p;       /* number of process */
    int source;  /* rank of sender */
    int dest;    /* rank of reciever */
    int tag = 0; /* tag for messages */

    MPI_Status status;

    int *data;
    int allSize = 0;
    int *pData;
    int pSize;
    int remainder = 0;
    int masterSize = 0;
    int localSum = 0;
    int totalSum = 0;
    double mean;
    double sumOfDeviations=0;
    double totalSumOfDeviations = 0;
    double start;
    double end;
    double totalTime = 0;
    double communicationOverhead = 0;

    /* Start up MPI */
    MPI_Init(&argc, &argv);
    /* Find out process rank */
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    /* Find out number of process */
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    if (my_rank == 0)
    {
        printf("Please Enter the size of the array and the elements \n");
        scanf("%d", &allSize);
        data = (int *)malloc(allSize * sizeof(int));
        int i;
        for (i = 0; i < allSize; i++)
        {
            scanf("%d", &data[i]);
        }

        // division of the data
        pSize = allSize / p;

        if (allSize % p != 0) // remainder handling in master
        {
            remainder = allSize % p;
        }
    }
    start = MPI_Wtime();
    MPI_Bcast(&pSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
    end = MPI_Wtime();
    communicationOverhead += end - start;

    if (my_rank == 0)
    {
        masterSize = pSize + remainder;
        pData = (int *)malloc(masterSize * sizeof(int));
    }
    else
        pData = (int *)malloc(pSize * sizeof(int));


    start = MPI_Wtime();

    MPI_Scatter(data, pSize, MPI_INT, pData, pSize, MPI_INT, 0, MPI_COMM_WORLD);

    end = MPI_Wtime();
    communicationOverhead += end - start;

    if (my_rank == 0) // handling remainder
    {
        int i = pSize;
        int j = allSize - remainder;
        for (i; i < masterSize; i++)
        {
            pData[i] = data[j];
            j++;
        }
    }

    // local sum

    if(my_rank == 0 )
        pSize = masterSize;

    start = MPI_Wtime();
    int i = 0;
    for( i = 0 ; i < pSize ; i++)
    {
        localSum += pData[i]; 
    }
    end = MPI_Wtime();
    totalTime += end-start;

    start = MPI_Wtime();

    MPI_Reduce(&localSum , &totalSum , 1 , MPI_INT , MPI_SUM , 0 , MPI_COMM_WORLD );

    end = MPI_Wtime();
    communicationOverhead += end - start;


    if(my_rank == 0)
    {
        mean = (double)totalSum / (double)allSize;
    }
 
    start = MPI_Wtime();
    
    MPI_Bcast(&mean , 1 , MPI_DOUBLE , 0 , MPI_COMM_WORLD);

    end = MPI_Wtime();
    communicationOverhead += end - start;

    // sum of Deviations 
    i = 0;
    start = MPI_Wtime();
    for(i ; i<pSize ; i++)
    {
        sumOfDeviations += pow(pData[i] - mean , 2);
    }
    end = MPI_Wtime();
    totalTime += end- start;

    start = MPI_Wtime();

    MPI_Reduce(&sumOfDeviations , &totalSumOfDeviations , 1 , MPI_DOUBLE , MPI_SUM , 0 , MPI_COMM_WORLD);

    end = MPI_Wtime();
    communicationOverhead += end - start;

    if(my_rank == 0)
    {
        printf("The Mean = %f \n" , mean);
        printf("The Variance = %f \n" , totalSumOfDeviations / allSize);
        printf("The Standard Deviation = %f \n" , sqrt(totalSumOfDeviations/allSize));
        printf("Time Taken for Serial Part = %f \n" , totalTime);
        printf("Time Taken for Communication Part = %f \n" , communicationOverhead);
    }

    /* shutdown MPI */
    MPI_Finalize();
    return 0;
}