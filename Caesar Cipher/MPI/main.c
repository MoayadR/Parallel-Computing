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

    int step;

    int size;

    char string[100];

    char data[200];

    int sz;

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

        char fileName[100] ;

        printf("Please Enter the file name and key \n");

        scanf("%s" , fileName);
        scanf("%d" , &step);

        FILE *fptr = fopen(fileName, "r");

        fseek(fptr, 0L, SEEK_END);

        sz = ftell(fptr) + 1;

        fseek(fptr, 0L, SEEK_SET);

        int i;

        for (i = 0; i < sz; i++)

        {

            data[i] = fgetc(fptr);
        }

        fclose(fptr);

        int index = sz;

        while (sz % p != 0)

        {

            data[index] = ' ';

            index++;

            sz++;
        }

        size = sz / (p);
    }

    MPI_Bcast(&step, 1, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Scatter(&data, size, MPI_CHAR, &string, size, MPI_CHAR, 0, MPI_COMM_WORLD);


    int i = 0;

    for (i; i <= size; i++)

    {

        if (string[i] == ' ')

            continue;

        string[i] -= 65;

        string[i] = (string[i] + step) % 26;

        string[i] += 65;
    }

    MPI_Gather(&string, size, MPI_CHAR, &data, size, MPI_CHAR, 0, MPI_COMM_WORLD);

    if (my_rank == 0)
    {
        
        char res [50];
        int j = 0;
        for(j; j<50; j++)
        {
            res[j] = ' ';
        }
        int index = 0;
        int i =0 ;
        for(i; i < sz ; i ++)
        {
            if((data[i] >= 65 && data[i] <= 90 ) || data[i] == 32)
                {
                    res[index] = data[i];
                    index++;
                }
        }
        printf("%s", res);
        
        FILE *fp = fopen("output.txt", "w");
        fputs(res, fp);
        fclose(fp);
        
    }

    /* shutdown MPI */

    MPI_Finalize();

    return 0;
}