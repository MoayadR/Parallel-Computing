#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include "mpi.h"

int **alloc_2d_int(int rows, int cols)

{

    int *data = (int *)malloc(rows * cols * sizeof(int));

    int **array = (int **)malloc(rows * sizeof(int *));

    int i;

    for (i = 0; i < rows; i++)

        array[i] = &(data[cols * i]);

    return array;
}

int **multiplyMatrix(int **matrix1, int **matrix2, int row1, int column2)

{

    int i, j;

    int **resMatrix = alloc_2d_int(row1, column2);

    int k;

    for (i = 0; i < row1; i++)

    {

        for (j = 0; j < column2; j++)

        {

            resMatrix[i][j] = 0;

            for (k = 0; k < column2; k++)

            {

                resMatrix[i][j] += (matrix1[i][k] * matrix2[k][j]);
            }
        }
    }

    return resMatrix;
}

int **matrixFactory(int row, int column)

{

    int i, j;

    int **matrix = alloc_2d_int(row, column);

    printf("\nPlease Enter its elements: \n");

    for (i = 0; i < row; i++)

    {

        for (j = 0; j < column; j++)

        {

            scanf("%d", &matrix[i][j]);
        }
    }

    return matrix;
}

void divideAndSendMatrix(int p, int tag, int **matrix1, int row1, int column1)

{

    int otherSlavesRows = row1 / (p - 1);

    int firstSlaveRows = otherSlavesRows;

    int slaveColumn = column1;

    if (row1 % (p - 1) != 0)

    {

        firstSlaveRows += (row1 % (p - 1));
    }

    // firstSlave size;

    int slaveRow = firstSlaveRows;

    MPI_Send(&slaveRow, 1, MPI_INT, 1, tag, MPI_COMM_WORLD);

    MPI_Send(&slaveColumn, 1, MPI_INT, 1, tag, MPI_COMM_WORLD);

    int **slaveMatrix = alloc_2d_int(slaveRow, slaveColumn);

    int i, j;

    for (i = 0; i < firstSlaveRows; i++)

    {

        for (j = 0; j < slaveColumn; j++)

        {

            slaveMatrix[i][j] = matrix1[i][j];
        }
    }

    MPI_Send(&slaveMatrix[0][0], firstSlaveRows * slaveColumn, MPI_INT, 1, tag, MPI_COMM_WORLD);

    free(slaveMatrix);

    slaveRow = otherSlavesRows;

    int k;

    for (k = 2; k < p; k++)

    {

        MPI_Send(&slaveRow, 1, MPI_INT, k, tag, MPI_COMM_WORLD);

        MPI_Send(&slaveColumn, 1, MPI_INT, k, tag, MPI_COMM_WORLD);

        int **slaveMatrix = alloc_2d_int(slaveRow, slaveColumn);

        int index = 0, condition = slaveRow + i;

        for (i = i; i < condition; i++)

        {

            for (j = 0; j < slaveColumn; j++)

            {

                slaveMatrix[index][j] = matrix1[i][j];
            }

            index++;
        }

        MPI_Send(&slaveMatrix[0][0], slaveRow * slaveColumn, MPI_INT, k, tag, MPI_COMM_WORLD);

        free(slaveMatrix);
    }
}

void receiveAndDisplayResMatrix(int p, int tag, MPI_Status *status)

{

    int resRow, resColumn;

    int **resMatrix;

    int i;

    for (i = 1; i < p; i++)

    {

        MPI_Recv(&resRow, 1, MPI_INT, i, tag, MPI_COMM_WORLD, status);

        MPI_Recv(&resColumn, 1, MPI_INT, i, tag, MPI_COMM_WORLD, status);

        resMatrix = alloc_2d_int(resRow, resColumn);

        MPI_Recv(&resMatrix[0][0], resRow * resColumn, MPI_INT, i, tag, MPI_COMM_WORLD, status);

        printf("Slave: %d", i);

        printf("\n");

        int j, k;

        for (j = 0; j < resRow; j++)

        {

            for (k = 0; k < resColumn; k++)

            {

                printf("%d", resMatrix[j][k]);

                printf(" ");
            }

            printf("\n");
        }

        free(resMatrix);
    }
}

void sendMatrixToAllSlaves(int **matrix, int row, int column, int p, int tag)

{

    int i;

    for (i = 1; i < p; i++)

    {

        MPI_Send(&row, 1, MPI_INT, i, tag, MPI_COMM_WORLD);

        MPI_Send(&column, 1, MPI_INT, i, tag, MPI_COMM_WORLD);

        MPI_Send(&matrix[0][0], row * column, MPI_INT, i, tag, MPI_COMM_WORLD);
    }
}

void consoleMatrix(int p, int tag, MPI_Status *status)

{

    int row1, row2, column1, column2;

    printf("Please enter dimensions of the first matrix: ");

    scanf("%d", &row1);

    scanf("%d", &column1);

    int **matrix1 = matrixFactory(row1, column1);

    printf("Please enter dimensions of the second matrix: ");

    scanf("%d", &row2);

    scanf("%d", &column2);

    int **matrix2 = matrixFactory(row2, column2);

    divideAndSendMatrix(p, tag, matrix1, row1, column1);

    sendMatrixToAllSlaves(matrix2, row2, column2, p, tag);

    receiveAndDisplayResMatrix(p, tag, status);
}

int **matrixFileFactory(FILE *file, int row, int column)

{

    int i, j;

    int **matrix = alloc_2d_int(row, column);

    for (i = 0; i < row; i++)

    {

        for (j = 0; j < column; j++)

        {

            fscanf(file, "%d", &matrix[i][j]);
        }
    }

    return matrix;
}

void fileMatrix(int p, int tag, MPI_Status *status)
{

    int row1, row2, column1, column2;

    FILE *file;

    file = fopen("./fileMatrix.txt", "r");

    fscanf(file, "%d", &row1);

    fscanf(file, "%d", &column1);

    int **matrix1 = matrixFileFactory(file, row1, column1);

    fscanf(file, "%d", &row2);

    fscanf(file, "%d", &column2);

    int **matrix2 = matrixFileFactory(file, row2, column2);

    divideAndSendMatrix(p, tag, matrix1, row1, column1);

    sendMatrixToAllSlaves(matrix2, row2, column2, p, tag);

    receiveAndDisplayResMatrix(p, tag, status);

    fclose(file);
}

int main(int argc, char *argv[])

{

    int my_rank; /* rank of process	*/

    int p; /* number of process	*/

    int source; /* rank of sender	*/

    int dest; /* rank of reciever	*/

    int tag = 0; /* tag for messages	*/

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

        int choice;

        printf("Welcome to vector Matrix multiplication program! \n");

        printf("To read dimensions and values from file press 1 \n");

        printf("To read dimensions and values from console press 2 \n");

        scanf("%d", &choice);

        switch (choice)

        {

        case 1:

            fileMatrix(p, tag, &status);

            break;

        case 2:

            consoleMatrix(p, tag, &status);

            break;

        default:

            break;
        }
    }

    else // Slaves

    {

        int slaveRow; /**/

        int slaveColumn; /**/

        MPI_Recv(&slaveRow, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);

        MPI_Recv(&slaveColumn, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);

        int **slaveMatrix = alloc_2d_int(slaveRow, slaveColumn);

        MPI_Recv(&slaveMatrix[0][0], slaveRow * slaveColumn, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);

        int row2, column2;

        MPI_Recv(&row2, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);

        MPI_Recv(&column2, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);

        int **matrix2 = alloc_2d_int(row2, column2);

        MPI_Recv(&matrix2[0][0], row2 * column2, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);

        int **resMatrix = multiplyMatrix(slaveMatrix, matrix2, slaveRow, column2);

        MPI_Send(&slaveRow, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);

        MPI_Send(&column2, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);

        MPI_Send(&resMatrix[0][0], slaveRow * column2, MPI_INT, 0, tag, MPI_COMM_WORLD);
    }

    /* shutdown MPI */

    MPI_Finalize();

    return 0;
}
