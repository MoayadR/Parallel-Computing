#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int main(int argc, char *argv)
{
    int localSum, totalSum;
    int allSize;
    int *data;
    double start , end , totalTime = 0;

    printf("Please Enter the size of the array and the elements \n");
    scanf("%d", &allSize);
    data = (int )malloc(allSize * sizeof(int));

    int i;
    for (i = 0; i < allSize; i++)
    {
        scanf("%d", &data[i]);
    }

    #pragma omp parallel private(localSum , i) shared(totalSum , data , allSize)
    {
        localSum = 0;
        totalSum = 0;

        start = omp_get_wtime(); 

    #pragma omp for
    for ( i = 0; i < allSize ; i++)
    {
        localSum += data[i];
    }

    // Create thread safe region.
    #pragma omp critical
        {
            // add each threads partial sum to the total sum
            totalSum += localSum;
        }
    }

    end = omp_get_wtime(); 
    totalTime += end - start;
    
    double mean = totalSum / allSize;
    double localDeviationSum , toatlDeviationSum;  
    i = 0;
    #pragma omp parallel private(localDeviationSum , i) shared(toatlDeviationSum , data , allSize , mean)
    {
        localDeviationSum = 0 ;
        toatlDeviationSum = 0 ;

        start = omp_get_wtime();

        #pragma omp for
        for ( i = 0; i < allSize ; i++)
        {
            localDeviationSum += pow((data[i] - mean) , 2);
        }

        #pragma omp critical
        {
            toatlDeviationSum += localDeviationSum;
        }

        end = omp_get_wtime(); 
        totalTime += end - start;
    }

    double variance = toatlDeviationSum / allSize;

    double standardDeviation  = sqrt(variance);

    printf("The Mean : %f\n" , mean);
    printf("The Variance : %f\n" , variance);
    printf("The Standard Deviation : %f\n" , standardDeviation);
    printf("Time Taken for Parallel Part = %f \n" , totalTime);

    return 0;
}
