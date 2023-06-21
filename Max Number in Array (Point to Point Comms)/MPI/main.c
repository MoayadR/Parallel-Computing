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

    int size; /*size of the array for each process*/

    int index; /* index of the max number */

    int max = -999999; /* max number value */

    int *slaveArray; /* the array for each process*/

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

        int arrSize;

        int numOfSlaves = p - 1;

        printf("Hello From Master Process: \n");

        printf("Number of Slave Processes is: %d", numOfSlaves);

        printf(" \n Please Enter the Size of the Array: \n");

        scanf("%d", &arrSize);

        // Dynamic Allocation of the Array

        int *allElementsArray = (int *)malloc(arrSize * sizeof(int));

        // Reading the Elements

        printf("Please Enter the Array Elements: \n");

        int i;

        for (i = 0; i < arrSize; i++)

        {

            scanf("%d", &allElementsArray[i]);
        }

        // Spliting the Array between Processes

        int otherSlavesSize = arrSize / numOfSlaves;

        int firstSlaveSize = otherSlavesSize;

        if (arrSize % numOfSlaves != 0)

        {

            firstSlaveSize += (arrSize % numOfSlaves);
        }

        slaveArray = (int *)malloc(firstSlaveSize * sizeof(int));

        for (i = 0; i < firstSlaveSize; i++)

        {

            slaveArray[i] = allElementsArray[i];
        }

        // First Slave Data

        size = firstSlaveSize;

        MPI_Send(&size, 1, MPI_INT, 1, tag, MPI_COMM_WORLD);

        MPI_Send(slaveArray, size, MPI_INT, 1, tag, MPI_COMM_WORLD);

        free(slaveArray);

        // starting index for rest of the slaves

        int index = firstSlaveSize;

        int j;

        // sending data for the rest of the slaves

        for (i = 2; i < p; i++)

        {

            slaveArray = (int *)malloc(otherSlavesSize * sizeof(int));

            for (j = 0; j < otherSlavesSize; j++)

            {

                slaveArray[j] = allElementsArray[index + j];
            }

            size = otherSlavesSize;

            MPI_Send(&size, 1, MPI_INT, i, tag, MPI_COMM_WORLD);

            MPI_Send(slaveArray, size, MPI_INT, i, tag, MPI_COMM_WORLD);

            index += otherSlavesSize;

            free(slaveArray);
        }

        free(allElementsArray);

        int maxFinal = -99999;

        int indexFinal;

        // !! Spliting the data between slaves done !!

        // Recieving the max from each slave

        int prevSizes = 0;

        for (i = 1; i < p; i++)

        {

            // Receiving processed data from the slave

            MPI_Recv(&max, 1, MPI_INT, i, tag, MPI_COMM_WORLD, &status);

            MPI_Recv(&index, 1, MPI_INT, i, tag, MPI_COMM_WORLD, &status);

            // Comparison

            if (max > maxFinal)

            {

                maxFinal = max;

                indexFinal = index + prevSizes;
            }

            // To caluclate index based on the allElementsArray

            if (i == 1)

            {

                prevSizes += firstSlaveSize;
            }

            else

            {

                prevSizes += otherSlavesSize;
            }
        }

        // The announcment

        printf("Master process announce the final max which is %d", maxFinal);

        printf(" and its index is %d", indexFinal);
    }

    else // Slaves

    {

        MPI_Recv(&size, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);

        slaveArray = (int *)malloc(size * sizeof(int));

        MPI_Recv(slaveArray, size, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);

        int i = 0;

        for (i; i < size; i++)

        {

            if (slaveArray[i] > max)

            {

                max = slaveArray[i];

                index = i;
            }
        }

        printf("Hello from slave #%d", my_rank);

        printf(" Max number in my partition is %d", max);

        printf(" and index is %d", index);

        printf("\n");

        MPI_Send(&max, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);

        MPI_Send(&index, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);

        free(slaveArray);
    }

    /* shutdown MPI */

    MPI_Finalize();

    return 0;
}
