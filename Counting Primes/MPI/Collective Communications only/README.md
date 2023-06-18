# Counting Prime Numbers within an Input Range
This program counts the number of prime numbers within a given input range using **Collective Communication**.
The program is parallelized by splitting the range into subranges and assigning each subrange to a different process. Then, the partial results are collected and combined to compute the final result.