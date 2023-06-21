# Value of PI

Using Collective Communication only 
Each slave process will receive the step and the range it will work on, then
calculate its partial sum.
Master process will divide the number of steps into ranges and broadcast
the value of step, then perform reduce to output the value of PI using the
reduced sum.
