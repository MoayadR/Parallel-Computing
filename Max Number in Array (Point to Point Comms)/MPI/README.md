# Maximum number in an Array
This program finds the maximum number in an array of numbers using Master-Slave Paradigm

## Master Process
**The master process performs the following tasks:**
- Read the size of the array.
- Read the elements inside the array.
- Distribute the work among slave processes by sending them the size of the array and then the data of the array.
- After each slave finishes its work, the master receives the maximum number and its index from each process.
- Compute the final maximum number and its index by comparing the maximum numbers returned from each slave.
- Output the final maximum number and its index in the original array to the user.

## Slave processes
**Each slave process performs the following tasks:**
- Receive the size of the array from the master process.
- Receive a part of the data of the array sent by the master process.
- Calculate the maximum number and its index in their part of the data.
- Send the maximum number and its index back to the master process.

**Note:** The program handles the case where the size of the array isn't divisible by the number of processes, by assigning the reminder to the first slave.